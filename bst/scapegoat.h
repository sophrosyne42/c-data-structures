// Katherine Agent
// scapegoat.h

#ifndef SCAPEGOAT_H
#define SCAPEGOAT_H

typedef struct sg_node_t {
    int value;
    struct sg_node_t* left;
    struct sg_node_t* right;
} sg_node_t;

typedef struct sg_tree_t {
    int nodes;
    int q;
    sg_node_t* head;
} sg_tree_t;

// Yields the size of the subtree given by `head`.
int sg_size(sg_node_t* head);

// Determines if `node` should be the right child of the `parent` based on their values.
// Since this is based on values, this only says where the node should be, it does not 
// indicate that the node is actually a child of parent.
int sg_should_be_right_child(sg_node_t* node, sg_node_t* parent);

/** Scapegoat Insertion.
 *
 * Insert n into the tree.
 *  1. Perform standard BST insertion.
 *  2. If `n` was actually inserted (was not already in the tree):
 *      2a. Increment `nodes` and `q` in `Sg_Tree`.
 *      2b. If depth of the new node is greater than log_(3/2) of `q`:
 *          2bi. Walk up tree until a node `w` is found such that `size(w) > 2/3 size(w.parent)`. In other words, the tree rooted at `w` accounts for more than 2/3 of nodes in its parent's tree.
 *          2bii. Rebuild the subtree rooted at `w.parent`.
*/
int sg_insert(sg_tree_t* tree, int n);

/** Scapegoat Delection.
 *
 *  Removes value `n` from the tree.
 *  1. Perform standard BST deletion.
 *  2. If `n` was actually removed (was in the tree):
 *      2a. Decrement nodes field in `Sg_Tree`.
 *      2b. If `nodes < q/2`, rebuild whole tree and set `q = n`.
*/
int sg_delete(sg_tree_t* tree, int n);

/** Scapegoat Search.
 *
 * Returns pointer to node containing `n`.
 * Returns `NULL` otherwise.
*/
sg_node_t* sg_find(sg_tree_t* tree, int n);

/** Scapegoat Flatten.
 *
 * Calling `sg_flatten(node_ptr_x, NULL)` will return a list of the nodes rooted at `node_ptr_x`
 * sorted in non-descending order.
 * Calling `sg_flatten(node_ptr_x, node_ptr_y)` will flatten the tree pointed to by `node_ptr_x`
 * into a list and then append the list headed by `node_ptr_y` to the end. 
 *
 * @param `node_left`  The left node of a scapegoat tree
 * @param `node_right` The right node of a scapegoat tree
 * @returns            a linked link of nodes in non-decreasing order stored using the right child pointers of the nodes
*/
sg_node_t* sg_flatten(sg_node_t* node_ptr_x, sg_node_t* node_ptr_y);

/** Scapegoat Build 
 * Accepts a pointer to a node, `rt_list`, and an integer length, `length`.
 *
 * @param `rt_list` A right-child-pointer list with length of at least `size + 1`
 * @param `size`    The number of nodes of the new tree to be created.
 * @returns         The (`size+1`)th node in the list, `s`, modified so that s.left points
 *                  to the root (`r`) of the (`size`)-node tree it just created.
*/
sg_node_t* sg_build(sg_node_t* rt_list, int size);

/** Scapegoat Rebuild 
 * @param `scapegoat` A pointer to the root of the subtree to be rebuilt
 * @param `size` The number of nodes of the subtree
*/
sg_node_t* sg_rebuild(sg_node_t* scapegoat, int size);


void print_tree_preorder(sg_tree_t* tree);

void print_tree_inorder(sg_tree_t* tree);

void print_tree_postorder(sg_tree_t* tree);

#endif

