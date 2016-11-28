/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  /* The following are all new variables, added for our implementation of B+ tree    */

  /* Variables for accessing index file  										     */
  BTreeIndex   bti;					 /* The index, which we access when appropriate  */
  IndexCursor  cur;				/* Cursor used to read consecutive entries in index  */
  int          searchKey;          /* The key we will initially search for in index  */
  bool         indexOpen = false;       /* Tells us whether we need to close at end  */

  /* Constants that describe what attributes are returned in query (more readable)   */
  const int    ATTR_KEY = 1;			                  /* Query returns key only  */
  const int    ATTR_VAL = 2;							/* Query returns value only  */
  const int    ATTR_ALL = 3;					  	 /* Query returns key and value  */
  const int    ATTR_CNT = 4;    /* Query returns # tuples satisfying all conditions  */

  /* Numerous variables to track all WHERE conditions, and if they make sense        */
  SelCond      currCond;						 /* Current condition being checked  */
  int          currCompValue;	/* Current compare value of condition being checked  */
  
  bool         useIndex  = false;	   /* Does it make sense to use index for query  */
  bool         needValue = false;	  /* Will we need to get value when reading rf?  */
  
  int          minKey = -1;				      /* What is minimum key value allowed?  */
  bool		   minKeySet = false;     		 /* Has a minimum key been set in cond?  */
  bool		   minKeyGT = false;  	  /* True means key must be > minKey (false >=)  */
  
  int          maxKey = -1;					  /* What is maximum key value allowed?  */
  bool         maxKeySet = false;			 /* Has a maximum key been set in cond?  */
  bool         maxKeyLT = false;  	  /* True means key must be < maxKey (false <=)  */
  
  int          keyEQValue = -1;				    /* What must key value be equal to?  */
  bool         keyEQValueSet = false;	         /* Has key value been set in cond?  */
  
  string 	   valEQValue = "";					/* What must str value be equal to?  */
  bool		   valEQValueSet = false;		     /* Has str value been set in cond?  */

  /* Open table file to retrieve query results  */
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  /* Before doing any reads of RecordFile or index PageFile, check validity of all
   * conditions in the WHERE clause and determine if index should be used for query. */
  for (unsigned i = 0; i < cond.size(); i++) {

  	/* Obtain current condition and int-representation of value to compare against   */
  	currCond = cond[i];
  	currCompValue = atoi(currCond.value);

  	/* The current condition is on the "key" attribute  */
  	if (currCond.attr == ATTR_KEY) {

  		/* Ignore NE. If a query only contains NE on a key, we should not use B+ tree
  		 * as it does not make sense from a performance standpoint.  */
  		if (currCond.comp == SelCond::NE) {
  			continue;
  		}

  		/* Otherwise, comp is not NE, which suggests we should use index for select.  */
  		useIndex = true;
  		
  		/* Depending on current comparison (EQ, LT, GT, LE, GE), correctly set the
  		 * appropriate condition variables and check for any invalidities.  */
  		switch (currCond.comp) {
  			
  			/* There can only be 1 equality on a key. If there are multiple equalities,
  			   we can finish now because no tuple will satisify the conditions.  */
  			case SelCond::EQ:
  				if ((!keyEQValueSet) || (keyEQValue == currCompValue)) {
  					keyEQValue    = currCompValue;
  					keyEQValueSet = true;
  				}
  				else
  					goto end_of_select;
  				break;

  			/* If a LT value is already defined, the condition variable only changes
  			 * if we have found a smaller (or equal) LT value  */
  			case SelCond::LT:
  				if ((maxKey == -1) || (currCompValue <= maxKey)) {
  					maxKey    = currCompValue;
  					maxKeyLT  = true;
  					maxKeySet = true;
  				}
  				break;

  			/* If a GT value is already defined, the condition variable only changes
  			 * if we have found a larger (or equal) GT value  */
  			case SelCond::GT:
  				if ((minKey == -1) || (currCompValue >= minKey)) {
  					minKey    = currCompValue;
  					minKeyGT  = true;
  					minKeySet = true;
  				}
  				break;

  			/* If a LE value is already defined, the condition variable only changes
  			 * if we have found a smaller LE value  */
  			case SelCond::LE:
  				if ((maxKey == -1) || (currCompValue < maxKey)) {
  					maxKey    = currCompValue;
  					maxKeyLT  = false;
  					maxKeySet = true;
  				}
  				break;

  			/* If a GE value is already defined, the condition variable only changes
  			 * if we have found a larger GE value  */
  			case SelCond::GE:
  				if ((minKey == -1) || (currCompValue > minKey)) {
  					minKey    = currCompValue;
  					minKeyGT  = false;
  					minKeySet = true;
  				}
  				break;

  			default:
  				break;
  		}
  	}
  	
  	/* The current condition is on the "value" attribute  */
  	else if (currCond.attr == ATTR_VAL) {

  		/* Similar for the key attribute, there can only be 1 equality on a value.
  		 * If there are multiple, we can finish now; no attributes will be returned.  */
  		switch (currCond.comp) {
  			case SelCond::EQ:
  				if ((!valEQValueSet) || (strcmp(valEQValue.c_str(), cond[i].value) == 0)) {
  					valEQValue = string(cond[i].value);
  					valEQValueSet = true;
  				}
  				else
  					goto end_of_select;
  			default:
  				needValue = true;
  				break;
  		}

  	}

    /* We shouldn't end up here, condition must be on key or value attribute  */
  	else
  		continue;  
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  /* Finished going through all conditions, time to check them. If we can, we'll take
   * shortcuts to avoid page reads. Also, we'll decide if we should use index.  */

  /* e.g. select * from table where key>5 and key<3  */
  if (minKeySet && maxKeySet && (minKey > maxKey))
  	goto end_of_select;

  /* e.g. select * from table where key<=5 and key>5  */
  if (minKeySet && maxKeySet && (minKey == maxKey) && (minKeyGT || maxKeyLT))
  	goto end_of_select;

  /* e.g. select * from table where key=5 and key<5  */
  if (keyEQValueSet && maxKeySet && (maxKey == keyEQValue) && maxKeyLT)
  	goto end_of_select;

  /* e.g. select * from table where key=5 and key>5  */
  if (keyEQValueSet && minKeySet && (minKey == keyEQValue) && minKeyGT)
  	goto end_of_select;

  /* e.g. select * from table where key=5 and key<=4  */
  if (keyEQValueSet && maxKeySet && (maxKey < keyEQValue))
  	goto end_of_select;

  /* e.g. select * from table where key=5 and key>6  */
  if (keyEQValueSet && minKeySet && (minKey > keyEQValue))
  	goto end_of_select;

  /* If we cannot open the index file (likely doesn't exist), we
   * have to perform the select by scanning through each tuple.  */
  if (bti.open(table + ".idx", 'r'))
  	goto no_index_select;
  else
  	indexOpen = true;

  /* If input suggests we don't use the index, we won't unless we are doing a
   * count(*) which can be optimized if there are no conditions on the value.
   * See (or search) "possible optimization" below.  */
  if (!useIndex && attr != ATTR_CNT)
  	goto no_index_select;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  
  /* If we have not jumped, we will now perform an index-based select here  */
  rid.pid = rid.sid = 0;
  count = 0;

  /* If there is an equality, we should search directly for that key  */
  if (keyEQValueSet)
  	searchKey = keyEQValue;

  /* Else, if we know the min key value, we can skip all those keys < (or <=) minKey  */
  else if (minKeySet)
  	searchKey = (minKeyGT ? minKey+1 : minKey);
  
  /* Otherwise, start with the smallest possible int value to start at first tuple (+1 to avoid warning)  */
  else
  	searchKey = -2147483647;

  /* Once our search key is decided, go to the correct location on leaf level to begin reading.
   * Continue to perform 'readForward' on tuples until they will no longer satisfy conditions.  */
  bti.locate(searchKey, cur);
  while (bti.readForward(cur, key, rid) == 0) {

  	/* Possible optimization: if there are no conditions on values, and we are only returning the count, we can
  	 * run through this code without having to read tuples from the RecordFile.  */
  	if (!needValue && attr == ATTR_CNT) {

  		if (keyEQValueSet && keyEQValue != key)
  			goto end_of_select;  		

  		if (minKeySet) {
  			if ((minKeyGT && key <= minKey) || (!minKeyGT && key < minKey))
  				goto end_of_select;
  		}

  		if (maxKeySet) {
  			if ((maxKeyLT && key >= maxKey) || (!maxKeyLT && key > maxKey))
  				goto end_of_select;
  		}

  		count++;
  		goto next_tuple_index;
  	}

  	/* Read the record, just like in the pre-existing select code  */
  	if (rc = rf.read(rid, key, value) < 0) {
  		fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
  		goto exit_select;
  	}

  	/* The remaining code is very similar to that for the pre-existing select code (see no_index_select:).
  	   Modifications that we've made have been commented.  */
  	for (unsigned i = 0; i < cond.size(); i++) {

  		switch (cond[i].attr) {
  			case 1:
  				diff = key - atoi(cond[i].value);
  				break;
  			case 2:
  				diff = strcmp(value.c_str(), cond[i].value);
  				break;
  		}

  		switch (cond[i].comp) {

  			/* If we are dealing with keys, a diff!=0 indicates the key will never again be equivalent to the
  			 * comparison key, meaning the select is finished. For values, continue scanning tuples.  */
  			case SelCond::EQ:
  				if (diff != 0) {
  					if (cond[i].attr == ATTR_KEY) goto end_of_select;
  					else                          goto next_tuple_index;
  				}
  				break;

  			/* Nothing's changed here.  */
  			case SelCond::NE:
  				if (diff == 0) goto next_tuple_index;
  				break;

  			/* Nothing's changed here.  */
  			case SelCond::GT:
  				if (diff <= 0) goto next_tuple_index;
  				break;

  			/* If we are dealing with keys, a diff>=0 indicates a key will never again be less than the
  			 * comparison key, meaning the select is finished. For values, continue scanning tuples.  */
  			case SelCond::LT:
  				if (diff >= 0) {
  					if (cond[i].attr == ATTR_KEY) goto end_of_select;
  					else                          goto next_tuple_index;
  				}
  				break;

  			/* Nothing's changed here.  */
  			case SelCond::GE:
  				if (diff < 0) goto next_tuple_index;
  				break;

  			/* If we are dealing with keys, a diff>0 indicates a key will never again be less than or
  			 * equal to the comparison key, meaning select is finished. For values, continue scanning.  */
  			case SelCond::LE:
  				if (diff > 0) {
  					if (cond[i].attr == ATTR_KEY) goto end_of_select;
  					else                          goto next_tuple_index;
  				}
  				break;
  		}
  	}

  	// the condition is met for the tuple.
  	// increasing matching tuple counter 
  	count++;

    // print the tuple 
    switch (attr) {
    	case 1:  // SELECT key
      		fprintf(stdout, "%d\n", key);
      		break;
    	case 2:  // SELECT value
      		fprintf(stdout, "%s\n", value.c_str());
      		break;
    	case 3:  // SELECT *
      		fprintf(stdout, "%d '%s'\n", key, value.c_str());
      		break;
    }
  	
  	next_tuple_index: ;	/* No-op, go to next tuple  */
  }

  goto end_of_select;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  /* This is the select code that was already written for us. It scans through all tuples
   * and checks if each tuple satisifies the condition, printing it if it does. We perform
   * this type of select if there is no index to access, or if using the index does not
   * make sense from a performance standpoint.  */
  no_index_select:
	  // scan the table file from the beginning
	  rid.pid = rid.sid = 0;
	  count = 0;
	  while (rid < rf.endRid()) {
	    // read the tuple
	    if ((rc = rf.read(rid, key, value)) < 0) {
	      fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
	      goto exit_select;
	    }

	    // check the conditions on the tuple
	    for (unsigned i = 0; i < cond.size(); i++) {
	      
	      // compute the difference between the tuple value and the condition value
	      switch (cond[i].attr) {
	      	case 1:
				diff = key - atoi(cond[i].value);
				break;
	      	case 2:
				diff = strcmp(value.c_str(), cond[i].value);
				break;
	      }

	      // skip the tuple if any condition is not met
	      switch (cond[i].comp) {
	      	case SelCond::EQ:
				if (diff != 0) goto next_tuple;
				break;
	      	case SelCond::NE:
				if (diff == 0) goto next_tuple;
				break;
	      	case SelCond::GT:
				if (diff <= 0) goto next_tuple;
				break;
	      	case SelCond::LT:
				if (diff >= 0) goto next_tuple;
				break;
	      	case SelCond::GE:
				if (diff < 0) goto next_tuple;
				break;
	      	case SelCond::LE:
				if (diff > 0) goto next_tuple;
				break;
	      }
	    }

	    // the condition is met for the tuple. 
	    // increase matching tuple counter
	    count++;

	    // print the tuple 
	    switch (attr) {
	    	case 1:  // SELECT key
	      		fprintf(stdout, "%d\n", key);
	      		break;
	    	case 2:  // SELECT value
	      		fprintf(stdout, "%s\n", value.c_str());
	      		break;
	    	case 3:  // SELECT *
	      		fprintf(stdout, "%d '%s'\n", key, value.c_str());
	      		break;
	    }

	    // move to the next tuple
	    next_tuple:
	    ++rid;
	  }

	  /* At the end of no_index_select, go to end_of_select code  */
	  goto end_of_select;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  end_of_select:

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  exit_select:

  /* Close the table file, the index file (if applicable), and return.  */
  rf.close();
  if (indexOpen) bti.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  ifstream    loadFile(loadfile.c_str());	/* File for loading contents of table  */
  RecordFile  tableFile;					/* File for storing contents of table  */
  BTreeIndex  indexFile;					/* File for storing contents of index  */

  /* Variables for reading input data / creating table & index files  */
  string    line;
  int       key;
  string    value;
  RecordId  tableRecord;
  RC        ret;

  /* Open file where table contents will be stored  */
  if ((ret = tableFile.open(table + ".tbl", 'w')) < 0) {
    fprintf(stderr, "Error creating table '%s' during LOAD\n", table.c_str());
    return ret;
  }

  /* If applicable, open file where index contents will be stored  */
  if (index) {
  	if (ret = indexFile.open(table + ".idx", 'w')) {
  		fprintf(stderr, "Error opening index file for write\n");
  		return ret;
  	}
  }

  /* If load file successfully opened, parse all input and construct table / index  */
  if (loadFile.is_open()) {

    while (getline(loadFile, line)) {
      
      if ((ret = parseLoadLine(line, key, value)) < 0) {
		fprintf(stderr, "Error parsing line from load file '%s'", loadfile.c_str());
		return ret;
      }
      
      if ((ret = tableFile.append(key, value, tableRecord)) < 0) {
		fprintf(stderr, "Error adding tuple with key '%i', value '%s' into table '%s'\n", key, value.c_str(), table.c_str());
		return ret;
      }

      if (index) {
      	if (ret = indexFile.insert(key, tableRecord)) {
      		fprintf(stderr, "Error adding (key, rid) to index for table '%s'\n", table.c_str());
      		return ret;
      	}
      }

    }

    loadFile.close();
    tableFile.close();
  }

  /* Load file did not successfully open for read  */
  else {
    fprintf(stderr, "Error opening load file '%s'", loadfile.c_str());
    return RC_FILE_OPEN_FAILED;
  }

  if (index) {
  	indexFile.close();
  }

  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
