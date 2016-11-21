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

using namespace std;

/*
 * BTreeIndex constructor
 * Initializes 'rootPid' and 'treeHeight' to reflect an empty tree.
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
	   will result in an error.  */
	pfMode = mode;

	/* If index has been modified, retrieve rootPid and treeHeight from the index
	 * file being opened  */
	if (pf.endPid() != BTreeIndex::METADATA_PID) {

		if (ret = pf.read(BTreeIndex::METADATA_PID, buffer))
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
       starting at a current height of 1 (root node) */
	if (treeHeight != 0) {
		int    midKey = -1;
		PageId endPid = -1;
		ret = recInsert(key, rid, 1, rootPid, midKey, endPid);
	}

	/* For a new tree, we start with an empty leaf node and insert the pair  */
	else {

		/* Should be able to successfully insert into empty leaf  */
		BTLeafNode newNode;
		if (ret = newNode.insert(key, rid))
			return ret;

		/* Assign new value to rootPid  */
		rootPid = (pf.endPid() == BTreeIndex::METADATA_PID ? 1 : pf.endPid());
		
		/* Write leaf contents to the page it belongs to  */
		if ((ret = newNode.write(rootPid, pf)) == 0)
			treeHeight++; 
	}

    return ret;
}

/**
 * Function to recursively insert a (key, rid) pair into the tree. Accounts for leaf,
 * non-leaf overflow and the splitting of nodes that results from said overflow.
 */
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

		/* Rearrange neighbor pointers so that current leaf comes before new sibling.
		   The sibling (i.e., newest node) goes to the current end pid.  */
		endPid = pf.endPid();
		emptyLeaf.setNextNodePtr(currLeaf.getNextNodePtr());
		currLeaf.setNextNodePtr(endPid);

		if (ret = currLeaf.write(currPid, pf))
			return ret;

		if (ret = emptyLeaf.write(endPid, pf))
			return ret;

		/* Current root (leaf) has been split, must create new root (non-leaf) */
		if (treeHeight == 1) {

			BTNonLeafNode root;
			root.initializeRoot(currPid, midKey, endPid);
			rootPid = pf.endPid();
			treeHeight++;
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

		/* If recKey or recPid (realistically both) were set, an insertAndSplit was
		 * was performed on a node below this node. Must account for this in current
		 * node.  
		 */
		if (recKey != -1 || recPid != -1) {

			/* Easy case: Insert midKey and child pid from insertAndSplit into parent
			 * non-leaf node.  */
			if (currNonLeaf.insert(recKey, recPid) == 0)
				return currNonLeaf.write(currPid, pf);

			/* Harder case: Parent non-leaf node is full, we need to insert & split.  */
			BTNonLeafNode emptyNonLeaf;
			if (ret = currNonLeaf.insertAndSplit(recKey, recPid, emptyNonLeaf, midKey))
				return ret;

			endPid = pf.endPid();

			if (ret = currNonLeaf.write(currPid, pf))
				return ret;

			if (ret = emptyNonLeaf.write(endPid, pf))
				return ret;

			/* Current root (non-leaf) has been split, must create new root (non-leaf) */
			if (treeHeight == 1) {

				BTNonLeafNode root;
				root.initializeRoot(currPid, midKey, endPid);
				rootPid = pf.endPid();
				treeHeight++;
				root.write(rootPid, pf);

			}
		}
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
	RC             ret;
	BTNonLeafNode  currNonLeaf;
	BTLeafNode     currLeaf;
	int            eid;

	/* Starting at the root node, traverse through tree using locateChildPtr until
	 * we reach the deepest level, consisting of leaf nodes. At this point, we will
	 * have narrowed down the search to the appropriate leaf node.
	 */
	PageId currPid = rootPid;
	for (int currHeight = 1; currHeight != treeHeight; currHeight++) {

		if (ret = currNonLeaf.read(currPid, pf))
			return ret;

		if (ret = currNonLeaf.locateChildPtr(searchKey, currPid))
			return ret; 
	}

	/* Reached the appropriate leaf node that would contain the searchKey (if it
	 * does exist). Read the leaf node's contents.
	 */
	if (ret = currLeaf.read(currPid, pf))
		return ret;

	/* Attempt to locate the key. Will return 0 if found, RC_NO_SUCH_RECORD if not.
	 * Regardless of outcome, cursor's pid will be set to pid of leaf node.
	 * Cursor's eid will either be correct entry containing key or entry after the
	 * largest key smaller than searchKey.
	 */
	ret = currLeaf.locate(searchKey, eid);
	cursor.eid = eid;
 	cursor.pid = currPid;
 	
    return ret;		/* Either 0 or RC_NO_SUCH_RECORD  */
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

	int nextEid;	/* What will 'eid' be set to when we move the cursor forward?  */
	PageId nextPid;	/* What will 'pid' be set to when we move the cursor forward?  */

	nextEid = cursor.eid;	
	nextPid = cursor.pid;

	/* Some boundary checks for an invalid cursor. pid cannot be 0 as this page is
	 * reserved for metadata (i.e., rootPid and treeHeight). pid cannot exceed the
	 * last readable page in pf. eid cannot be negative, and should not be zero as
	 * this space is reserved for keyCount. First eid begins at eid=1.
	 */
	if((cursor.pid <= 0) || (cursor.pid >= pf.endPid()) || (cursor.eid <= 0))
		return RC_INVALID_CURSOR;

	/* Read the contents at the location specified by the index cursor.  */
	BTLeafNode currLeaf;
	if (ret = currLeaf.read(cursor.pid, pf))
		return ret;
	if (ret = currLeaf.readEntry(cursor.eid, key, rid))
		return ret; 

	/* Boundary checks are based on our structure / implementation in BTreeNode.cc
     * The first eid of a page begins at 1. 0 is reserved for keyCount of the node
     */
	if (cursor.eid < currLeaf.getKeyCount())
		nextEid++;	
	else {
		nextEid = 1;
		nextPid = currLeaf.getNextNodePtr();
	}

	/* Set index cursor to next entry  */
	cursor.eid = nextEid;
	cursor.pid = nextPid; 

    return 0;
}

/**
 * Print contents of B+-tree. We used this during testing / debugging, but had to
 * comment out what we had because we used temporary (now non-existent) functions
 * in BTreeNode.
 */
void BTreeIndex::printContents(const char* file)
{
	/*string separator = "////////////////////////////////////////////////////////////";
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
	}*/
}
