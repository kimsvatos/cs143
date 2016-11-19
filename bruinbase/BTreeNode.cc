#include "BTreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//																			 //
//							       BTLeafNode								 //
//																			 //
//																			 //
//	STRUCTURE:       														 //
//																			 //
//	| keyCount | empty | key | rid | key | rid | ... | ...  |  pid  |	     //
//	0		   4	   12								  1020	   1024		 //
//																		     //
//	keyCount tracks the number of entries inserted into the node.			 //
//	Assuming 4-byte key, 8-byte rid, we can fit 84 entries in a node.		 //
//																			 //
///////////////////////////////////////////////////////////////////////////////

/*
 * Constructor for BTLeafNode class.
 * First 4 bytes of buffer keeps track of the number of entries in the node.
 * If a new leaf node is being created, keyCount will be correctly set to zero.
 * If leaf node already exists, correct keyCount will be obtained during read.
 * @return none.
 */
BTLeafNode::BTLeafNode()
{
	int keyCount = 0;
	memcpy(buffer, &keyCount, sizeof(int));
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
	int keyCount;
	memcpy(&keyCount, buffer, sizeof(int));
	return keyCount;
}

/*
 * Update the number of keys stored in the node.
 * @return the updated number of keys in the node.
 */
int BTLeafNode::updateKeyCount(const int& keyCount)
{
	memcpy(buffer, &keyCount, sizeof(int));
	return keyCount;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{
	/* Compute:
	 *   - Size of a leaf node entry (consists of key and RecordId)
     *   - Total # of entries that can fit into a node
     *     - Have to subtract pid and entry reserved for keyCount
     *     - (see structure above)
     */
	int entrySize = sizeof(int) + sizeof(RecordId);
	int totalEntries = (PageFile::PAGE_SIZE - sizeof(PageId) - entrySize) / entrySize;
	
	int keyCount = getKeyCount();
	if (keyCount == totalEntries) { // Can't insert any more entries into node
		fprintf(stderr, "failed at node full");
		return RC_NODE_FULL;
}

	/* Loop through all entries in the buffer, searching for the first entry with 
	 * a key value that is greater than the key value of the entry to insert. If
	 * we search through all but the last entry in the node, we know from above
	 * that the node is not full and thus can insert the entry into the last spot.
	 *
	 * Start at entry 1 (i = 1) b/c entry 0 is reserved for keyCount.
	 */
	int i;
	int currentKey;
	for (i = 1; i < totalEntries; i++) {

		if (i > keyCount)  /* The next entry is empty, we can insert here  */
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
		return RC_FILE_WRITE_FAILED;
	}

	/* Copy all entries up to current offset into new buffer  */
	memcpy(newBuff, buffer, offset);

	/* Copy the new entry, starting at position of offset  */
	memcpy(newBuff + offset, &key, sizeof(int));
	memcpy(newBuff + offset + sizeof(int), &rid, sizeof(RecordId));

	/* Copy all entries (if any) that come after the newly inserted entry 
	 * Add back 'entrySize' because the first entry is reserved for keyCount
	 */
	memcpy(newBuff + offset + entrySize, buffer + offset, keyCount * entrySize - offset + entrySize);

	/* End the last 4 bytes of the buffer / page with pid pointing to the next neighbor node  */
	PageId nextNode = getNextNodePtr();
	memcpy(newBuff + PageFile::PAGE_SIZE - sizeof(PageId), &nextNode, sizeof(PageId));

	/* Copy the new node contents into the 'buffer' member variable to update node  */
	memcpy(buffer, newBuff, PageFile::PAGE_SIZE);
	free(newBuff);

	/* Update keyCount to reflect successful insertion of entry to leaf node  */
	keyCount++;
	updateKeyCount(keyCount);

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

	/* Compute:
	 *   - Size of a leaf node entry (consists of key and RecordId)
     *   - Total # of entries that can fit into a node
     *     - Have to subtract pid and entry reserved for keyCount
     *     - (see structure above)
     */
	int entrySize = sizeof(int) + sizeof(RecordId);
	int totalEntries = (PageFile::PAGE_SIZE - sizeof(PageId) - entrySize) / entrySize;

	int keyCount = getKeyCount();
	if (keyCount != totalEntries       // Should only do insert-split if node already full
		|| sibling.getKeyCount() != 0) // Sibling node MUST be EMPTY when function called
	{
		return RC_INVALID_FILE_FORMAT;
	}

	int halfKeys = int( ceil( float(totalEntries) / float(2) ));
	int splitOffset = halfKeys * entrySize + entrySize;

	memcpy(sibling.buffer + entrySize, buffer + splitOffset, PageFile::PAGE_SIZE - sizeof(PageId) - splitOffset);
	
	sibling.updateKeyCount(totalEntries - halfKeys);
	updateKeyCount(halfKeys);

	int firstSiblingKey;
	memcpy(&firstSiblingKey, sibling.buffer + entrySize, sizeof(int));
	if (key >= firstSiblingKey)
		sibling.insert(key, rid);
	else
		insert(key, rid);

	memcpy(&siblingKey, sibling.buffer + entrySize, sizeof(int));

	fprintf(stderr, "first key (of second page) is....%i\n", firstSiblingKey);

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
	int keyCount = getKeyCount();
	for(i = 1; i <= keyCount; i++) {

		memcpy(&currentKey, buffer + (i * entrySize), sizeof(int));

		fprintf(stderr, "%i ", currentKey);

		if (currentKey >= searchKey) {
			matchFound = (currentKey == searchKey ? 1 : 0);
			break;
		}
	}
    
    /* Set eid as specified and return appropriate code  */
    eid = i;
	return (matchFound == 1 ? 0 : RC_NO_SUCH_RECORD); 
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
	if (eid > getKeyCount() || eid < 0)
		return RC_NO_SUCH_RECORD;

	/* Entry consists of a key and RecordId  */
	int entrySize = sizeof(int) + sizeof(RecordId);

	memcpy(&key, buffer + (eid * entrySize), sizeof(int));
	memcpy(&rid, buffer + (eid * entrySize) + sizeof(int), sizeof(RecordId));

	return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	/* A NodePtr is just a pid; return next page id  */
	PageId nextNode;

	/* Copy the last 4 bytes of the page to get next node ptr  */
	memcpy(&nextNode, buffer + PageFile::PAGE_SIZE - sizeof(PageId), sizeof(PageId));

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

	/* Copy input to last 4 bytes of page to set pid  */
	memcpy(buffer + PageFile::PAGE_SIZE - sizeof(PageId), &pid, sizeof(PageId));

	return 0; 
}

///////////////////////////////////////////////////////////////////////////////
//																			 //
//							     BTNonLeafNode								 //
//																			 //
//																			 //
//	STRUCTURE:       														 //
//																			 //
//	| keyCount | pid/empty | key | pid | key | pid | ... | ...  |   pid  |	 //
//	0		   4	       8								   1020	   1024  //
//																		     //
//	keyCount tracks the number of entries inserted into the node.			 //
//  The 4-byte space after keyCount will be empty or (for root) pid.         //
//	Assuming 4-byte key, 4-byte pid, we can fit 127 entries in a node.		 //
//																			 //
///////////////////////////////////////////////////////////////////////////////

/*
 * Constructor for BTLeafNode class.
 * First 4 bytes of buffer keeps track of the number of entries in the node.
 * If a new leaf node is being created, keyCount will be correctly set to zero.
 * If leaf node already exists, correct keyCount will be obtained during read.
 * @return none.
 */
BTNonLeafNode::BTNonLeafNode()
{
	int keyCount = 0;
	memcpy(buffer, &keyCount, sizeof(int));
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
	int keyCount;
	memcpy(&keyCount, buffer, sizeof(int));
	return keyCount;
}

/*
 * Update the number of keys stored in the node.
 * @return the updated number of keys in the node.
 */
int BTNonLeafNode::updateKeyCount(const int& keyCount)
{
	memcpy(buffer, &keyCount, sizeof(int));
	return keyCount;
}

/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ 
	/* Compute:
	 *   - Size of a leaf node entry (consists of key and PageId)
     *   - Total # of entries that can fit into a node
     *     - Have to subtract entry reserved for keyCount and (if applicable) pid
     *     - (see structure above)
     */
	int entrySize = sizeof(int) + sizeof(PageId);
	int totalEntries = (PageFile::PAGE_SIZE - entrySize) / entrySize;
	
	int keyCount = getKeyCount();
	if (keyCount == totalEntries)	// Can't insert any more entries into node
		return RC_NODE_FULL;

	/* Loop through all entries in the buffer, searching for the first entry with 
	 * a key value that is greater than the key value of the entry to insert. If
	 * we search through all but the last entry in the node, we know from above
	 * that the node is not full and thus can insert the entry into the last spot.
	 *
	 * Start at entry 1 (i = 1) b/c entry 0 is reserved for keyCount.
	 */
	int i;
	int currentKey;
	for (i = 1; i < totalEntries; i++) {

		if (i > keyCount)  /* The next entry is empty, we can insert here  */
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
		return RC_FILE_WRITE_FAILED;
	}

	/* Copy all entries up to current offset into new buffer  */
	memcpy(newBuff, buffer, offset);

	/* Copy the new entry, starting at position of offset  */
	memcpy(newBuff + offset, &key, sizeof(int));
	memcpy(newBuff + offset + sizeof(int), &pid, sizeof(PageId));

	/* Copy all entries (if any) that come after the newly inserted entry  */
	memcpy(newBuff + offset + entrySize, buffer + offset, keyCount * entrySize - offset + entrySize);

	/* Copy the new node contents into the 'buffer' member variable to update node  */
	memcpy(buffer, newBuff, PageFile::PAGE_SIZE);
	free(newBuff);

	/* Update keyCount to reflect successful insertion of entry to non-leaf node  */
	keyCount++;
	updateKeyCount(keyCount);

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

	int entrySize = sizeof(int) + sizeof(PageId);
	int maxEntries = (PageFile::PAGE_SIZE - entrySize) / entrySize; 

	if(sibling.getKeyCount() != 0)
			return RC_INVALID_FILE_FORMAT; //check
	if(getKeyCount() < maxEntries)
			return RC_INVALID_FILE_FORMAT; //check error

	//half to stay
	//CHECK for pid offset
		//add one to entry size for first (numKeys, pid) entry
	int splitOffset = int(ceil(float(maxEntries)/float(2) )) * (entrySize) + entrySize;

	//need to determine which key is being removed
	/*
	|    ....   |  lastKey  |  pid   | *SPLIT OFFSET* |  firstKey   |    pid  | .......| 
														| keycount | frst pid | firstkey | ....
	*/
	int lastKey, firstKey;
	memcpy(&lastKey, buffer + splitOffset - entrySize, sizeof(int));
	memcpy(&firstKey, buffer + splitOffset, sizeof(int));

	if(key < lastKey){ //lastKey is removed
		memcpy(sibling.buffer + entrySize, buffer + splitOffset, PageFile::PAGE_SIZE - splitOffset); 

		//store keyCount in sibling
		int sibKeys = maxEntries - int(ceil(float(maxEntries)/float(2))); 
		sibling.updateKeyCount(sibKeys);
		//memcpy(sibling.buffer, &sibKeys, sizeof(int));

		//store initial PID
		memcpy(sibling.buffer + sizeof(int), buffer + splitOffset - sizeof(PageId), sizeof(PageId));

		//update number of keys here
		int newNumKeys = int(ceil(float(maxEntries)/float(2))) - 1;
		updateKeyCount(newNumKeys);
		//memcpy(buffer, &newNumKeys, sizeof(int));
 
		//save midKey and insert new key
		memcpy(&midKey, buffer + splitOffset - entrySize, sizeof(int));
		insert(key, pid);

	}
	else if (key > firstKey){ //firstKey is being kicked out
		memcpy(sibling.buffer + entrySize, buffer + splitOffset + entrySize, PageFile::PAGE_SIZE - splitOffset - entrySize);
		int sibKeys = maxEntries - int(ceil(float(maxEntries)/float(2))) - 1; 
		sibling.updateKeyCount(sibKeys);

		//set PID of new sibling
		memcpy(sibling.buffer + sizeof(int), buffer + splitOffset + sizeof(int), sizeof(PageId));

		//update key count
		int newNumKeys = int(ceil(float(maxEntries)/float(2)));
		updateKeyCount(newNumKeys);

		//save midkey and insert new key
		memcpy(&midKey, buffer + splitOffset, sizeof(int));
		sibling.insert(key, pid);
	}
	else //new key is to be removed
	{
		//copy right side to sibling
		memcpy(sibling.buffer + entrySize, buffer + splitOffset, PageFile::PAGE_SIZE - splitOffset);
		
		//set first pid of sibling
		memcpy(sibling.buffer + sizeof(int), &pid, sizeof(PageId));

		//set number of sib keys
		int sibKeys = maxEntries - int(ceil(float(maxEntries)/float(2)));
		sibling.updateKeyCount(sibKeys);

		//update key count
		int newNumKeys =  int(ceil(float(maxEntries)/float(2)));
		updateKeyCount(newNumKeys);

		//save midkey
		midKey = key; 

	}


	//fprintf(stderr, "first key (of first page) is....%i\n");	

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
{ 
	/* Entry consists of a key and PageId  */
	int entrySize = sizeof(int) + sizeof(PageId);
	
	int i;
	int currentKey;
	int keyCount = getKeyCount();
	for(i = 1; i <= keyCount; i++){
		memcpy(&currentKey, buffer + (i * entrySize), sizeof(int));

		if(searchKey < currentKey){
			// use pid on left
			if(i == 1)
				memcpy(&pid, buffer + sizeof(int), sizeof(PageId));
			else
				break;
				
			return 0;
		}
	}

	memcpy(&pid, (buffer + (i * entrySize) - sizeof(PageId)), sizeof(PageId) );
	return 0; 
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 
	memcpy(buffer + sizeof(int), &pid1, sizeof(PageId));
	return insert(key, pid2);
}












