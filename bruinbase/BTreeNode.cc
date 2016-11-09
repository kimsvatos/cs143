#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "BTreeNode.h"

using namespace std;

/*
 * Constructor for BTLeafNode class
 * On success, member variables 'buffer' and 'm_nKeys' are initialized.
 * @return none
 */
BTLeafNode::BTLeafNode()
{
	m_nKeys = 0;
	//memset(buffer, 0, PageFile::PAGE_SIZE);
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
	//actually insert it TODO
	int entrySize = sizeof(int) + sizeof(RecordId);

	//total page size (except last pointer) divided by size of pair
	int totalEntries = (PageFile::PAGE_SIZE - sizeof(PageId))/entrySize;

	
	/* RecordId = size 8, key is size 4, so 12 bytes per entry
	 * 1020 bytes per page / 12 bytes per entry (we disregard last 4 bytes for pid) 
	 * = totalEntries
	*/
	if(getKeyCount() >= totalEntries)
		return RC_NODE_FULL;

	/* Loop through all entries in the buffer, searching for the first entry with 
	 * a key value that is greater than the key value of the entry to insert. If
	 * we search through the first 84 entries in the node, we know that the node is
	 * not full and thus can simply insert the entry into the last spot in the node
	 * (starting at index 1008 in the buffer).
	 */
	int i;
	for (i = 0; i <= 1007; i += entrySize) {

		int currentKey;
		memcpy(&currentKey, buffer + (i * entrySize), sizeof(int));

		if (i >= m_nKeys)  /* The next entry is empty, we can insert here  */
			break;

		if (key <= currentKey)  /* Key being inserted is <= key in current entry  */
			break;
	}

	/* Create new buffer / page that we will eventually write out as updated node  */
	char* newBuff = (char*) malloc (PageFile::PAGE_SIZE);
	if (newBuff == NULL) {
		fprintf(stderr, "Error updating node with new insertion\n");
	}
	//memset(newBuff, 0, PageFile::PAGE_SIZE);

	/* Copy all entries up to i (i.e., the pre-existing nodes) into new buffer  */
	memcpy(newBuff, buffer, i);

	/* Copy the new entry, starting at position i  */
	memcpy(newBuff + i, &key, sizeof(int));
	memcpy(newBuff + i + sizeof(int), &rid, sizeof(RecordId));

	/* Copy all entries (if any) that come after the newly inserted entry  */
	memcpy(newBuff + i + entrySize, buffer + i, m_nKeys * entrySize - i);

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
	int entrySize = sizeof(int) + sizeof(RecordId);
	int i; 
	for(i = 0; i < m_nKeys; i++){
		int currentKey;
		memcpy(&currentKey, buffer + (i * entrySize), sizeof(int));
		if(currentKey >= searchKey){
			break;
		}
	}

    eid = i;
	return 0; 
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
	//error check
	if(eid >= getKeyCount())
			return RC_NO_SUCH_RECORD;
	if(eid < 0)
			return RC_NO_SUCH_RECORD;

	//we want to read key, rid from eid 
	//size
	int entrySize = sizeof(RecordId) + sizeof(int);
	char* holder = buffer;

	int offset = eid*entrySize;
	//
	// |    buffer     | key (int)  | rid    |
	//
	memcpy(&key, holder + offset, sizeof(int));
	memcpy(&rid, holder + offset + sizeof(int), sizeof(RecordId));
	return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 

	//nodeptr is just pid, return next page id
	PageId ret_pid; 
	int offset = (PageFile::PAGE_SIZE) - sizeof(PageId);
	memcpy(&ret_pid, buffer + offset, sizeof(PageId));

	//debugging statement poentially
	//fprintf(stdout, "nextNodePtr: %i \n", ret_pid);

	return ret_pid; 
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	if (pid < 0) // not possible
		return RC_INVALID_PID; 

	int offset = PageFile::PAGE_SIZE - sizeof(PageId);
	memcpy(buffer + offset, &pid, sizeof(PageId));

	return 0; 

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////


BTNonLeafNode::BTNonLeafNode()
{
	m_nKeys = 0;
	//memset(buffer, 0, PageFile::PAGE_SIZE);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ 
	//read into member buffer
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
{ return 0; }


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
{ return 0; }
