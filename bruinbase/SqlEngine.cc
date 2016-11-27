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
  int    count;	// TODO: remember to initialize to 0 at some point...
  int    diff;

  /* New variables, added for our implementation of B+ tree  */
  BTreeIndex   bti;
  IndexCursor  cur;
  
  /* Open table file to retrieve query results  */
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  /* In the simplest case, we can't open the index file (likely doesn't exist) and
   * thus we have to perform the select by scanning through each tuple.  */
  if (bti.open(table + ".idx", 'r'))
  	goto no_index_select;



  /* TODO: Begin index stuff here...  */
  //////////////////////////////////////
  //////////////////////////////////////

  SelCond  currCond;
  bool     useIndex  = FALSE;
  bool     needValue = FALSE;

  // TODO: Add more variables here...

  for (unsigned i = 0; i < cond.size(); i++) {
  	//TODO: Implement code here
  	fprintf(stdout, "Condition\n");
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  /* END TODO: End of added index stuff... */




  /* This is the select code that was already written for us. It scans through all tuples
   * and checks if each tuple satisifies the condition, printing it if it does. We perform
   * this type of select if there is no index to access, or if using the index does not
   * make sense.  */
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


  /* This code should be executed after performing select.  */
  end_of_select:
	  // print matching tuple count if "select count(*)"
	  if (attr == 4) {
	    fprintf(stdout, "%d\n", count);
	  }
	  rc = 0;

  /* Close the table file, the index file (if applicable), and return.  */
  exit_select:
	  rf.close();
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
