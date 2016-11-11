#include "BTreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//																			 //
//							       BTLeafNode								 //
//																			 //
///////////////////////////////////////////////////////////////////////////////

/*
 * Constructor for BTLeafNode class.
 * @success variable 'm_nKeys' is initialized to zero.
 * @return none.
 */
BTLeafNode::BTLeafNode()
{
	m_nKeys = 0;
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	return pf.read(pid, buffer);
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 
	return pf.write(pid, buffer);
 }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
	return m_nKeys; 
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ 
	/* Entry consists of a key and RecordId  */
	int entrySize = sizeof(int) + sizeof(RecordId);

	/* Number of entries = total page size (except last pointer) divided by size of entry  */
	int totalEntries = (PageFile::PAGE_SIZE - sizeof(PageId)) / entrySize;
	
	if (getKeyCount() >= totalEntries)
		return RC_NODE_FULL;

	/* Loop through all entries in the buffer, searching for the first entry with 
	 * a key value that is greater than the key value of the entry to insert. If
	 * we search through all but the last entry in the node, we know from above
	 * that the node is not full and thus can insert the entry into the last spot.
	 */
	int i;
	int currentKey;
	for (i = 0; i < totalEntries - 1; i++) {

		if (i >= m_nKeys)  /* The next entry is empty, we can insert here  */
			break;

		/* Otherwise, check if key being inserted is <= key in current entry  */
		memcpy(&currentKey, buffer + (i * entrySize), sizeof(int));
		if (key <= currentKey)
			break;
	}
	int offset = i * entrySize;

	/* Create new buffer / page that we will eventually write out as updated node  */
	char* newBuff = (char*) malloc (PageFile::PAGE_SIZE);
	if (newBuff == NULL) {
		fprintf(stderr, "Error updating node with new insertion\n");
	}

	/* Copy all entries up to current offset into new buffer  */
	memcpy(newBuff, buffer, offset);

	/* Copy the new entry, starting at position of offset  */
	memcpy(newBuff + offset, &key, sizeof(int));
	memcpy(newBuff + offset + sizeof(int), &rid, sizeof(RecordId));

	/* Copy all entries (if any) that come after the newly inserted entry  */
	memcpy(newBuff + offset + entrySize, buffer + offset, m_nKeys * entrySize - offset);

	/* End the last 4 bytes of the buffer / page with pid pointing to the next neighbor node  */
	PageId nextNode = getNextNodePtr();
	memcpy(newBuff + PageFile::PAGE_SIZE - sizeof(PageId), &nextNode, sizeof(PageId));

	/* Copy the new node contents into the 'buffer' member variable to update node  */
	memcpy(buffer, newBuff, PageFile::PAGE_SIZE);
	free(newBuff);

	/* Update m_nKeys to reflect successful insertion of entry to leaf node  */
	m_nKeys++;

	return 0; 
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	///do the thing HERE TODO


	// update m_nKeys, we will call insert twice which will account for two 
	// new nodes, so remove one from count for split
	m_nKeys--;

	return 0; 

}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	/* Entry consists of a key and RecordId  */
	int entrySize = sizeof(int) + sizeof(RecordId);

	/* Search for an entry with key matching searchKey. If none found,
	   set eid to entry after entry with largest key smaller than searchKey.  */
	int i;
	int matchFound = 0;
	int currentKey;
	for(i = 0; i < m_nKeys; i++) {

		memcpy(&currentKey, buffer + (i * entrySize), sizeof(int));

		if (currentKey == searchKey) {
			matchFound = 1;
			break;
		}

		if (currentKey > searchKey) {
			break;
		}
	}
    
    /* Set eid as specified and return appropriate code  */
    eid = i;
	return (matchFound == 1) ? 0 : RC_NO_SUCH_RECORD; 
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	/* Check for out-of-bounds entry id  */
	if (eid >= m_nKeys || eid < 0)
		return RC_NO_SUCH_RECORD;

	/* Entry consists of a key and RecordId  */
	int entrySize = sizeof(int) + sizeof(RecordId);

	memcpy(&key, buffer + (eid * entrySize), sizeof(int));
	memcpy(&rid, buffer + (eid * entrySize) + sizeof(int), sizeof(RecordId));

	return 0;  // Entry successfully read
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	/* A NodePtr is just a pid; return next page id  */
	PageId nextNode;

	/* Go to the last 4 bytes of the page to read pid */
	int offset = (PageFile::PAGE_SIZE) - sizeof(PageId);

	/* Copy the last 4 bytes of the page to get next node ptr  */
	memcpy(&nextNode, buffer + offset, sizeof(PageId));

	return nextNode; 
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	/* Check for out-of-bounds pid  */
	if (pid < 0)
		return RC_INVALID_PID; 

	/* Go to the last 4 bytes of the page to write pid  */
	int offset = PageFile::PAGE_SIZE - sizeof(PageId);

	/* Copy input to last 4 bytes of page to set pid  */
	memcpy(buffer + offset, &pid, sizeof(PageId));

	return 0; 
}

///////////////////////////////////////////////////////////////////////////////
//																			 //
//							     BTNonLeafNode								 //
//																			 //
///////////////////////////////////////////////////////////////////////////////

/*
 * Constructor for BTNonLeafNode class.
 * @success variable 'm_nKeys' is initialized to zero.
 * @return none.
 */
BTNonLeafNode::BTNonLeafNode()
{
	m_nKeys = 0;
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ 
	return pf.read(pid, buffer); 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ 
	return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ 
	return m_nKeys; 
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ 

	//TODO actual insert


	//update m_nKeys
	m_nKeys++;
	return 0; 
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ 
	//TO DO actual split, call insert twice


	//update num keys bc we call insert twice
	m_nKeys--;

	return 0; 
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 

	memcpy(temp, &pid1, sizeof(pageId));
	ret = insert(key, pid2);
	if(ret == 0)
		return 0;
	else
		return ret;

}












