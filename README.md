An assortment of data structures I have implemented for class projects and assignments.

# scapegoat

I implemented an binary search tree called a scapegoat tree. This implementation only stores integers.

A (https://en.wikipedia.org/wiki/Scapegoat_tree)[scapegoat tree] is a kind of self-balancing binary search tree which defers rebalancing. It does this by keeping track of the α-height, rebalancing when it inserts a node (aka, the 'scapegoat') which breaks the invariant:

height <= floor(log_1/α ( size )) + 1

where height is the depth of the tree, and size is the number of non-empty nodes, and α is some arbitrary constant selected based on one's performance and balance-strictness criteria. I select α = 3/2 as a reasonable heuristic for ensuring the tree remains balanced without forcing rebalancing too frequently.

# hash_table

This is a relatively standard implementation of a hash table. This implementation can store keys and values of any type, requiring the caller to provide comparison and deallocation functions for the data types they wish to store on creation of the hash table.

This implementation of a hash table utilizes separate chaining with a linked list to deal with collisions.

For the hash function, I use a 64-bit cyclic-redundancy-check algorithm, which was provided by my professor in the assignment prompt that involved creating a hash table.
