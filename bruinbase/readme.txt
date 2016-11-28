Kyle Haacker, Kim Svatos
904467146, 604425426
haackerk@ucla.edu, kimsvatos@ucla.edu

Project 2 - Part D

The deliverable for this part of the project is a single zip file "P2.zip" containing:

	- This README file
	- A team.txt file, containing both team member's UIDs
	- A Makefile to build and subsequently run Bruinbase
	- All source files

For this part of the project, the 'load' and 'select' functions were fully implemented
in file SqlEngine.cc, thereby completing our attempt at implementation of a B+ tree.
In summary, the following source files were modified:

	BTreeNode.h   - Add functions to simplify implementation in BTreeNode.cc
	BTreeNode.cc  - Implement functionality of an independent leaf / non-leaf node
	BTreeIndex.h  - Add functions, member variables to simplify implementation
	BTreeIndex.cc - Implement functionality of a complete B+ tree index
	SqlEngine.cc  - Implement 'load' and 'select' functions, which now use the index

The greatest challenge in this part of the project was handling all possible cases of
WHERE-clause conditions and figuring out when shortcuts / the index should be used to
improve performance. We believe we have covered the majority of cases in which a query
would considerably improve in performance from use of an index. One optimization we
added is that, even if the set of conditions suggests not using an index, we still use
the index is the query is selecting 'count(*)'. This is because we can count more quickly
if we do not have to retrieve values from the RecordFile, which incur disk I/Os. When tested
with the provided 'movie.del' data, we noticed an improvement from 403 page reads to 65 page
reads. There are some other "optimizations" we perform when checking the conditions. For
example, if the WHERE conditions are "key=3 and key>3", there is no such tuple that could
satisfy this case and thus we can quit immediately. Similarly, for "key=3 and key=5", there
is no way a tuple could have two values for key and thus we return empty set.

For testing, we used test.sh provided by the instructor, and built off of that script
to include our own test cases. For each individual test case, we also ran an identical
query for a table not using index, to track the performance improvement. All of our
test cases saw significant improvement when using index, and matched the number of page
reads (off-by-one) as noted in the sample output file.

Through our testing, we believe each query returns the correct answer while also 
greatly improving the performance.  


The teamwork for this part had an approximate 90-10, 10-90 breakdown:

Kyle:  90% implementation, 10% testing
Kim:   10% implementation, 90% testing


