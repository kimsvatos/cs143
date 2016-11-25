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
#include <cassert>
#include "BTreeIndex.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
#define RUN_TEST_CODE 1

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  if (RUN_TEST_CODE == 0)
    SqlEngine::run(stdin);

  else {
    BTreeIndex bti;
    IndexCursor cur;
    int key;
    RecordId rid;
    assert(bti.open("test2d.idx", 'r') == 0);
    for (int i=1; i<5; i++) {
      assert(bti.locate(i, cur) == 0);
      assert(bti.readForward(cur, key, rid) == 0);
      assert(key == i);
    }
    assert(bti.locate(0, cur) == RC_NO_SUCH_RECORD);
    assert(bti.locate(5, cur) == RC_NO_SUCH_RECORD);
    assert(bti.locate(1, cur) == 0);
    for (int i=1; i<5; i++) {
      assert(bti.readForward(cur, key, rid) == 0);
      assert(key == i);
    }
    assert(bti.readForward(cur, key, rid) == RC_INVALID_CURSOR);
    assert(bti.close() == 0);
    fprintf(stderr, "ALL TEST CASES PASSED\n");
  }

  return 0;
}
