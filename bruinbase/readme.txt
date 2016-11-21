Kyle Haacker, Kim Svatos
904467146, 604425426
haackerk@ucla.edu, kimsvatos@ucla.edu

Project 2 - Part C

The deliverable for this part of the project is a single zip file "P2.zip" containing:

	- This README file
	- A team.txt file, containing both team member's UIDs
	- A Makefile to build and subsequently run Bruinbase
	- All source files

For this part of the project, BTreeIndex.h and BTreeIndex.cc were modified. BTreeNode.h
and BTreeNode.cc, which were both modified in Part B, were not modified in this part of
the project. Furthermore, SqlEngine.cc, which was modified in Part A, was not modified
in this part of the project.

One of the tasks in this part was to decide where to store the member variables of
the BTIndex. We decided to reserve PageId 0 (i.e., METADATA_PID in BTreeIndex.h) for
storing the metadata (i.e., rootPid and treeHeight) associated with the index.

Another task was deciding when to implement B+ tree operations with recursive algorithms.
Adding a recursive insert function, recInsert, greatly simplified our implementation of
insert. We decided that a recursive algorithm was not necessary for locate, as the logic
was already fairly straightforward and we could keep track of the current level of the
tree while looping.

We believe we have implemented all functionality for this part of the project. Each
function was tested against multiple cases / index files of varying sizes. Additionally,
we used a 'print' function to check the contents of leaf nodes and the root node. The
'print' function was removed from our final submission of 'BTreeIndex.h' and 'BTreeIndex.cc'
to save space and avoid 'clutter' in the code.

The teamwork for this part had an approximate 80-20, 20-80 breakdown:

Kim:  80% implementation, 20% testing
Kyle: 20% implementation, 80% testing


