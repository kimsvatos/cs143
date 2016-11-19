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
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>


//TODO: change int ret's to RC ret's

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
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

	int ret;
	char buffer[PageFile::PAGE_SIZE];

	if(ret = pf.open(indexname, mode)) //if ret is not zero, return error. 
		return ret;

	if(pf.endPid() != BTreeIndex::METADATA_PID){

		if(ret = pf.read(BTreeIndex::METADATA_PID, buffer))
			return ret;

		//NOTE: maybe check for error
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
	int ret;
	char buffer[PageFile::PAGE_SIZE];

	memcpy(buffer, &rootPid, sizeof(PageId));
	memcpy(buffer + sizeof(PageId), &treeHeight, sizeof(int));

	if(ret = pf.write(BTreeIndex::METADATA_PID, buffer))
		return ret;

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

	
	if(treeHeight){ //tree already exists
		//recursive
		int midKey = -1;
		int currPid = -1;
		if(ret = insert_help(key, rid, 1, rootPid, midKey, currPid))
			return ret;
		else
			return 0; 

	}

	else{ // new tree

		BTLeafNode newNode;
		newNode.insert(key, rid);

		rootPid = (pf.endPid() == 0 ? BTreeIndex::ROOT_PID : pf.endPid());
		
		ret = newNode.write(rootPid, pf);
		treeHeight = 1; 
	}
    return ret;
}

RC BTreeIndex::insert_help(int key, const RecordId& rid, int currHeight, PageId currPid, int& midKey, PageId& holderPid){
	
	RC ret; 

	if(treeHeight == currHeight){

		//new leaf node
		BTLeafNode currLeaf; 
		if(ret = currLeaf.read(currPid, pf))
			return ret;
		
		//easiest case, simply insert a leaf node
		if(!currLeaf.insert(key, rid)){
			return currLeaf.write(currPid, pf);
		}

		//insert failed if we get here, insert and split
		BTLeafNode emptyLeaf; 
		if(ret = currLeaf.insertAndSplit(key, rid, emptyLeaf, midKey))
			return ret;

		int endPid = pf.endPid();

		emptyLeaf.setNextNodePtr(currLeaf.getNextNodePtr());
		currLeaf.setNextNodePtr(endPid);

		if(ret = currLeaf.write(endPid, pf))
			return ret;

		if(ret = currLeaf.write(currPid, pf))
			return ret;

		//check if split root, holding midKey in midKey
		if(treeHeight ==1){
			BTNonLeafNode root;
			root.initializeRoot(currPid, midKey, endPid);
			treeHeight++;

			rootPid = pf.endPid();
			root.write(rootPid, pf);
		}

		return 0; 

	}
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
    return 0;
}
