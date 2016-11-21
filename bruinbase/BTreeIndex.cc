/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <iostream>
#include <cstring>
#include <stdio.h>
//#include <stdlib.h>

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;	  /* No root node yet, set rootPid to invalid value    */
    treeHeight = 0;	  /* No nodes created yet, thus height should be zero  */
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
	RC ret;
	char buffer[PageFile::PAGE_SIZE];

	/* Open index file in appropriate mode, returning error code if open fails  */
	if (ret = pf.open(indexname, mode))
		return ret;

	/* Store file mode in member variable. If the file was opened in 'r' mode and
	   we call 'close', we should avoid writing any changes to the pf, as this
	   will result in an error  */
	pfMode = mode;

	/* If index has been modified, retrieve rootPid and treeHeight from the index
	 * file being opened  */
	if(pf.endPid() != BTreeIndex::METADATA_PID) {

		if(ret = pf.read(BTreeIndex::METADATA_PID, buffer))
			return ret;

		memcpy(&rootPid, buffer, sizeof(PageId));
		memcpy(&treeHeight, buffer + sizeof(PageId), sizeof(int));
	}

	return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	RC ret;
	char buffer[PageFile::PAGE_SIZE];

	/* If the file was opened in 'w' mode, save rootPid and treeHeight to file  */
	if (pfMode == 'w') {

		memcpy(buffer, &rootPid, sizeof(PageId));
		memcpy(buffer + sizeof(PageId), &treeHeight, sizeof(int));

		if (ret = pf.write(BTreeIndex::METADATA_PID, buffer))
			return ret;
	}

    return pf.close();
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    RC ret;

    /* For tree that already exists (i.e., has nodes), perform recursive insert,
       starting at a current height of 1  */
	if (treeHeight != 0) {
		int midKey = -1;
		int endPid = -1;
		ret = recInsert(key, rid, 1, rootPid, midKey, endPid);
	}

	/* For a new tree, we start with an empty leaf node and insert the pair  */
	else {
		BTLeafNode newNode;
		if (ret = newNode.insert(key, rid))
			return ret;

		rootPid = (pf.endPid() == BTreeIndex::METADATA_PID ? BTreeIndex::ROOT_PID : pf.endPid());
		
		if ((ret = newNode.write(rootPid, pf)) == 0)
			treeHeight++; 
	}

    return ret;
}

RC BTreeIndex::recInsert(int key, const RecordId& rid, int currHeight, PageId currPid, int& midKey, PageId& endPid)
{
	RC ret; 

	/* Reached deepest level of tree (leaf nodes)  */
	if (treeHeight == currHeight) {

		/* Access current leaf node, whose contents are in PageId 'currPid'  */
		BTLeafNode currLeaf; 
		if (ret = currLeaf.read(currPid, pf))
			return ret;

		/* Easy case: leaf node is not full, simply insert the new pair  */
		if (currLeaf.insert(key, rid) == 0)
			return currLeaf.write(currPid, pf);

		/* Harder case: leaf node is full, we need to insert and split  */
		BTLeafNode emptyLeaf; 
		if (ret = currLeaf.insertAndSplit(key, rid, emptyLeaf, midKey))
			return ret;

		/* Rearrange neighbor pointers so that current leaf comes before new sibling  */
		endPid = pf.endPid();
		emptyLeaf.setNextNodePtr(currLeaf.getNextNodePtr());
		currLeaf.setNextNodePtr(endPid);

		if (ret = currLeaf.write(currPid, pf))
			return ret;

		if (ret = emptyLeaf.write(endPid, pf))
			return ret;

		/* Just split the root leaf node, need a root non-leaf node to be added to tree */
		if (treeHeight == 1) {
			BTNonLeafNode root;
			root.initializeRoot(currPid, midKey, endPid);
			treeHeight++;
			rootPid = pf.endPid();
			root.write(rootPid, pf);
		}
	}

	/* Still traversing tree, haven't reached leaf nodes yet  */
	else {

		/* Access current non-leaf node, whose contents are in PageId 'currPid'  */
		BTNonLeafNode currNonLeaf;
		if (ret = currNonLeaf.read(currPid, pf))
			return ret;
		
		/* Determine which way to traverse tree, based on key we're inserting  */
		PageId childPid;
		currNonLeaf.locateChildPtr(key, childPid);

		/* Attempt insertion at next the next level of the tree (at childPid)  */
		int recKey = -1;
		int recPid = -1;
		recInsert(key, rid, currHeight + 1, childPid, recKey, recPid);

		//if we get here there has been an error
		if (!(recKey == -1 && recPid == -1)) {
			if (currNonLeaf.insert(recKey, recPid) == 0)
				return currNonLeaf.write(currPid, pf);

			BTNonLeafNode newInsideNode;
			currNonLeaf.insertAndSplit(recKey, recPid, newInsideNode, midKey);

			int endPid = pf.endPid();

			if(ret = currNonLeaf.write(currPid, pf))
				return ret;

			if(ret = newInsideNode.write(endPid, pf))
				return ret;

			fprintf(stderr, "treeheight is....%i", treeHeight);
			if(treeHeight == 1){
				BTNonLeafNode root;
				root.initializeRoot(currPid, midKey, endPid);
				fprintf(stderr, "endpid is.....%i\ncurrpid is....%i\n", endPid, currPid);
				treeHeight++;
				rootPid = pf.endPid();
				root.write(rootPid, pf);
			}
			
		}
		return 0;
	}
		
	return 0; 
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	RC ret;

	int curEid = cursor.eid; 
	PageId curPid = cursor.pid;
	if(curPid <= 0)
		return RC_INVALID_CURSOR;


	BTLeafNode node; 
	if(!(ret = node.read(curPid, pf)))
		return ret;

	if(!(ret = node.readEntry(curEid, key, rid)))
		return ret; 

	if(curEid < node.getKeyCount() -1)
		curEid++;
	else{
		curEid = 0;
		curPid = node.getNextNodePtr();
	}

	cursor.eid = curEid;
	cursor.pid = curPid; 
	
    return 0;
}

void BTreeIndex::printContents(const char* file)
{
	string separator = "////////////////////////////////////////////////////////////";
	const char* buffer;
	PageId pid;
	int key;
	int keyCount;
	RecordId rid;

	FILE* logFile = fopen(file, "w");
	if (logFile == NULL)
		return;

	fprintf(logFile, "%s\n%s\n\nTREE METADATA:\n\trootPid = %i\n\ttreeHeight = %i\n\tendPid = %i\n\n%s\n\n", 
		separator.c_str(), separator.c_str(), rootPid, treeHeight, pf.endPid(),
		separator.c_str());

	if (treeHeight == 1) {
		BTLeafNode root;
		root.read(rootPid, pf);
		buffer = root.getContents();
		keyCount = root.getKeyCount();
		pid = root.getNextNodePtr();

		fprintf(logFile, "Root Leaf Node (treeHeight = 1)\n");
		fprintf(logFile, "keyCount = %i\n", keyCount);
		fprintf(logFile, "next pid = %i\n\n", pid);

		for (int i = 1; i <= keyCount; i++) {
			memcpy(&key, buffer + (i * 12), sizeof(int));
			memcpy(&rid, buffer + (i * 12) + sizeof(int), sizeof(RecordId));
			fprintf(logFile, "entry %i:\n", i);
			fprintf(logFile, "\tkey = %i\n", key);
			fprintf(logFile, "\trid = (%i, %i)\n\n", rid.pid, rid.sid);
		}
		return;
	}

	else {
		BTLeafNode leaf;
		leaf.read(1, pf);
		buffer = leaf.getContents();
		keyCount = leaf.getKeyCount();
		pid = leaf.getNextNodePtr();

		fprintf(logFile, "Leaf Node (treeHeight = 2)\n");
		fprintf(logFile, "keyCount = %i\n", keyCount);
		fprintf(logFile, "next pid = %i\n\n", pid);

		for (int i = 1; i <= keyCount; i++) {
			memcpy(&key, buffer + (i * 12), sizeof(int));
			memcpy(&rid, buffer + (i * 12) + sizeof(int), sizeof(RecordId));
			fprintf(logFile, "entry %i:\n", i);
			fprintf(logFile, "\tkey = %i\n", key);
			fprintf(logFile, "\trid = (%i, %i)\n\n", rid.pid, rid.sid);
		}

		fprintf(logFile, "%s\n\n", separator.c_str());

		leaf.read(2, pf);
		buffer = leaf.getContents();
		keyCount = leaf.getKeyCount();
		pid = leaf.getNextNodePtr();

		fprintf(logFile, "Leaf Node (treeHeight = 2)\n");
		fprintf(logFile, "keyCount = %i\n", keyCount);
		fprintf(logFile, "next pid = %i\n\n", pid);

		for (int i = 1; i <= keyCount; i++) {
			memcpy(&key, buffer + (i * 12), sizeof(int));
			memcpy(&rid, buffer + (i * 12) + sizeof(int), sizeof(RecordId));
			fprintf(logFile, "entry %i:\n", i);
			fprintf(logFile, "\tkey = %i\n", key);
			fprintf(logFile, "\trid = (%i, %i)\n\n", rid.pid, rid.sid);
		}

		fprintf(logFile, "%s\n\n", separator.c_str());

		BTNonLeafNode nonLeaf;
		nonLeaf.read(3, pf);
		buffer = nonLeaf.getContents();
		keyCount = nonLeaf.getKeyCount();

		fprintf(logFile, "Root Node (treeHeight = 1)\n");
		fprintf(logFile, "keyCount = %i\n", keyCount);

		for (int i = 1; i <= keyCount; i++) {
			memcpy(&key, buffer + (i * 8), sizeof(int));
			fprintf(logFile, "entry %i:\n", i);
			if (i == 1) {
				memcpy(&pid, buffer + sizeof(int), sizeof(PageId));
				fprintf(logFile, "\tpid1 = %i\n", pid);
			}
			memcpy(&pid, buffer + (i * 8) + sizeof(int), sizeof(PageId));
			fprintf(logFile, "\tkey = %i\n", key);
			fprintf(logFile, "\tpid = %i\n", pid);
		}

	}

}
