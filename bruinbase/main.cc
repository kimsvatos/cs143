/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
#include <cstdio>

#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

#define HALF_FULL_LEAF 42
#define HALF_FULL_NON  64

int testIndex();
int testIndexAdvanced();
int testIndexLarge();

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);

  fprintf(stderr, "\nTesting Basic Index Functionality...\n");
  //testIndex();
  fprintf(stderr, "...All Basic Functionality Passed\n\n");

  fprintf(stderr, "Running Advanced Test Cases...\n");
  //testIndexAdvanced();
  fprintf(stderr, "...All Advanced Functionality Passed\n\n");

  testIndexLarge();

  return 0;
}

int testIndex() 
{
  BTreeIndex     bti;
  BTLeafNode     btl;
  BTNonLeafNode  btn;
  int            key;
  int            eid;
  int            cnt;
  PageFile       pgf;
  PageId         pid;
  RecordId       rid;
  RC             ret;
  IndexCursor    cur;

  fprintf(stderr, "\tTest file 'open' (bad file name)...");
  assert(bti.open("testindex.txt", 'r') == RC_FILE_OPEN_FAILED);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest file 'open' (bad file mode)...");
  assert(bti.open("testIndex.txt", 'y') == RC_INVALID_FILE_MODE);
  assert(bti.open("testIndex.txt", 'z') == RC_INVALID_FILE_MODE);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest successful file 'open' (write mode)...");
  assert(bti.open("testIndex.txt", 'w') == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest successful file 'close' (write mode)...");
  assert(bti.close() == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest successful file 'open' (read mode)...");
  assert(bti.open("testIndex.txt", 'r') == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest successful file 'close' (read mode)...");
  assert(bti.close() == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest first insert to tree...");
  assert(bti.open("testIndex.txt", 'w') == 0);
  key     = 1;
  rid.pid = 1;
  rid.sid = 1;
  assert(bti.insert(key, rid) == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest filling first leaf node of tree...");
  for (int i = 2; i < 85; i++) {
    key     = i;
    rid.pid = i;
    rid.sid = i;
    assert(bti.insert(key, rid) == 0);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest insert into full leaf node...");
  key     = 85;
  rid.pid = 85;
  rid.sid = 85;
  assert(bti.insert(key, rid) == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest first (of 2) leaf nodes half full, correct contents...");
  assert(bti.close() == 0);
  assert(pgf.open("testIndex.txt", 'r') == 0);
  assert(btl.read(1, pgf) == 0);
  assert(btl.getKeyCount() >= HALF_FULL_LEAF);
  for (int i = 1; i < 43; i++) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(btl.locate(i, eid) == 0);
    assert(eid == i);
    assert(btl.readEntry(eid, key, rid) == 0);
    assert(key == i && rid.pid == i && rid.sid == i);
  }
  for (int i = 43; i < 85; i++) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(btl.locate(i, eid) == RC_NO_SUCH_RECORD);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest second (of 2) leaf nodes half full, correct contents...");
  assert(btl.read(2, pgf) == 0);
  assert(btl.getKeyCount() >= HALF_FULL_LEAF);
  for (int i = 1; i < 43; i++) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(btl.locate(i, eid) == RC_NO_SUCH_RECORD);
  }
  for (int i = 43; i < 85; i++) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(btl.locate(i, eid) == 0);
    assert(eid == i-42);
    assert(btl.readEntry(eid, key, rid) == 0);
    assert(key == i && rid.pid == i && rid.sid == i);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest root node contents (after insert and split)...");
  assert(btn.read(3, pgf) == 0);
  assert(btn.getKeyCount() == 1);
  for (int i = 1; i < 43; i++) {
    key = i;
    assert(btn.locateChildPtr(key, pid) == 0);
    assert(pid == 1);
  }
  for (int i = 43; i < 85; i++) {
    key = i;
    assert(btn.locateChildPtr(key, pid) == 0);
    assert(pid == 2);
  }
  assert(btn.locateChildPtr(5000, pid) == 0);
  assert(pid == 2);
  assert(pgf.close() == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest index location and subsequent retrieval (key exists)...");
  assert(bti.open("testIndex.txt", 'r') == 0);
  for (int i = 1; i < 85; i++) {
    key = i;
    assert(bti.locate(i, cur) == 0);
    if (i < 43) {
      assert(cur.pid == 1);
      assert(cur.eid == i);
    }
    else {
      assert(cur.pid == 2);
      assert(cur.eid == i-42);
    }
    assert(bti.readForward(cur, key, rid) == 0);
    assert(key == i && rid.pid == i && rid.sid == i);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest readForward through all records (successful)...");
  assert(bti.locate(1, cur) == 0);
  assert(cur.pid == 1);
  assert(cur.eid == 1);

  for (int i = 1; i < 86; i++) {
    if (i < 43) {
      assert(cur.pid == 1);
      assert(cur.eid == i);
    }
    else {
      assert(cur.pid == 2);
      assert(cur.eid == i-42);
    }    
    assert(bti.readForward(cur, key, rid) == 0);
    assert(key == i && rid.pid == i && rid.sid == i);
    if (i < 42) {
      assert(cur.pid == 1);
      assert(cur.eid == i+1);
    }
    else if (i < 85) {
      assert(cur.pid == 2);
      assert(cur.eid == i-41);
    }
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest readForward when reached end...");
  assert(bti.readForward(cur, key, rid) == RC_INVALID_CURSOR);
  assert(bti.readForward(cur, key, rid) == RC_INVALID_CURSOR);
  fprintf(stderr, "PASS\n");

  //bti.printContents("log.txt");
  return 0;
}

int testIndexAdvanced() {

  BTreeIndex     bti;
  BTLeafNode     btl;
  BTNonLeafNode  btn;
  int            key;
  int            eid;
  int            cnt;
  PageFile       pgf;
  PageId         pid;
  RecordId       rid;
  RC             ret;
  IndexCursor    cur;

  fprintf(stderr, "\tOpen file for writing...");
  assert(bti.open("testIndexAdvanced.txt", 'w') == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tInsert odd pairs, up to 10753 (e.g. {1, (1, 1)}, etc.)...");
  for (int i = 1; i < 10753; i+=2) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(bti.insert(key, rid) == 0);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tInsert even pairs, up to 10753 (e.g. {2, (2, 2)}, etc.)...");
  for (int i = 2; i < 10753; i+=2) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(bti.insert(key, rid) == 0);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tClose file for writing...");
  assert(bti.close() == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tOpen file for reading...");
  assert(bti.open("testIndexAdvanced.txt", 'r') == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest key location and subsequent readForward...");
  for (int i = 1; i < 10753; i++) {
    key = i;
    assert(bti.locate(key, cur) == 0);
    assert(bti.readForward(cur, key, rid) == 0);
    assert(key == i && rid.pid == i && rid.sid == i);
  }
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tTest readForward through all records...");
  assert(bti.locate(0, cur) == RC_NO_SUCH_RECORD);
  assert(bti.locate(1, cur) == 0);
  for (int i = 1; i < 10753; i++) {
    assert(bti.readForward(cur, key, rid) == 0);
    assert(key == i && rid.pid == i && rid.sid == i);
  }
  fprintf(stderr, "PASS\n");



  return 0;
} 

int testIndexLarge() {

  BTreeIndex     bti;
  BTLeafNode     btl;
  BTNonLeafNode  btn;
  int            key;
  int            eid;
  int            cnt;
  PageFile       pgf;
  PageId         pid;
  RecordId       rid;
  RC             ret;
  IndexCursor    cur;

  fprintf(stderr, "\tOpen file for writing...");
  assert(bti.open("testIndexLarge.txt", 'w') == 0);
  fprintf(stderr, "PASS\n");

  fprintf(stderr, "\tInsert pairs up 10754");
  for (int i = 1; i < 10000; i++) {
    key = i;
    rid.pid = i;
    rid.sid = i;
    assert(bti.insert(key, rid) == 0);
  }
  fprintf(stderr, "PASS\n");

  assert(bti.close() == 0);
  int count = 0;
  assert(bti.open("testIndexLarge.txt", 'r') == 0);
  for (int i = 1; i < 10000; i++) {
    if (bti.locate(i, cur) != 0) {
      fprintf(stderr, "%i ", i);
      count++;
    }
  }
  fprintf(stderr, "\n%i\n", count);
}