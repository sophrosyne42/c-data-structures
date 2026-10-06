// Katherine Agent
// scapegoat.c

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "scapegoat.h"

#define LOG_3_OVER_2(n) (log(n) / log(3.0/2.0))

// Yields the size of `node`.
int sg_size(sg_node_t* node) {
    if (node == NULL) return 0;
    return 1 + sg_size(node->left) + sg_size(node->right);
}

// Helper function for _sg_find_parent.
static sg_node_t* _sg_find_parent_helper(sg_tree_t* tree, sg_node_t* node, sg_node_t* parent, int value) {
    if (node == NULL) {
        return NULL;
    } else if (value == node->value) {
        return parent;
    } else {
    sg_node_t* next_node = value > node->value ? node->right : node->left;
        return _sg_find_parent_helper(tree, next_node, node, value);
    }

}

// Given a tree and a node in the tree, give the parent of the node.
static sg_node_t* _sg_find_parent(sg_tree_t* tree, sg_node_t* node) {
    return _sg_find_parent_helper(tree, tree->head, NULL, node->value);
}

// Accepts a node and its parent.
// Returns 1 if the node given should be placed as the parent's
// right child.
// Returns 0 if otherwise.
int sg_should_be_right_child(sg_node_t* node, sg_node_t* parent) {
    // If the value of the current node is greater than the value of the parent,
    // then the current node is the parent's right node. So the current node's 
    // sibling is the parent's left node.
    // Otherwise, it's the reverse.
    if (node->value > parent->value) 
        return 1;
    else return 0;
}

// Returns the size of the node if it is a scapegoat.
// Returns 0 if it is not a scapegoat.
static int _sg_get_size_if_scapegoat(sg_node_t* node, sg_node_t* parent) {
    sg_node_t* sibling = sg_should_be_right_child(node, parent) ? parent->left : parent->right;

    int node_size = sg_size(node);
    int sibling_size = sg_size(sibling);

    assert(parent->value != node->value);

    int parent_size = 1 + node_size + sibling_size;

    int is_scapegoat = ((double) node_size / (double) parent_size) > (2.0 / 3.0);

    if (is_scapegoat) {
        assert(node_size != 0 && "A scapegoat should never have size zero. Something went wrong.");
        return parent_size;
    } else return 0;
}

// This function header is declared because the next function is mutually recursive with it.
static sg_node_t* _sg_insert_helper(
    sg_tree_t* tree, 
    sg_node_t* node, 
    sg_node_t* parent, 
    int value, 
    int* depth, 
    int* should_rebuild
);


// A tertiary helper function for _sg_insert_helper.
// This function will advance tree recursion by checking whether the current `value`
// is greater or less than the value of the current `node`. The next node is determined
// based on that comparison, and recursion is continued by calling the original
// _sg_insert_helper() function.
//
// A crucial addition for this function, which motivated splitting it off into its own 
// definition, is the code after the recursive call. That code is intended to execute after
// the program has reached the base case and returned, i.e., while the stack is being unwound.
// At the base case in _sg_insert_helper(), a flag `should_rebuild` is set to determine whether
// a rebuild needs to be done. That flag is used to trigger a check on whether the current node's
// parent is a scapegoat. If it is, then a rebuild is triggered on the parent.
//
// This code is split off into its own definition because this logic is substantially the same
// between two recursive calls - when the next node is the left node, and when the next node
// is the right node.
static sg_node_t* _sg_insert_continue_or_trigger_rebuild(
    sg_tree_t* tree,
    sg_node_t* node,
    sg_node_t* parent,
    int value,
    int* depth,
    int* should_rebuild
) {
    // Shouldn't ever have gotten here, since the caller already checks this.
    if (node->value == value) {
        assert(1 && "Shouldn't have gotten this possibility since the caller checks "
                      "this condition before this function is called." );
        return NULL;
    }
    // (*depth)++;
    sg_node_t* next_node = (node->value > value) ? node->left : node->right;
    sg_node_t* new_node = _sg_insert_helper(tree, 
                                          next_node, 
                                          node, 
                                          value, 
                                          depth, 
                                          should_rebuild
                                          );
    if (new_node == NULL) return NULL;
    if (*should_rebuild) {
        int parent_size = _sg_get_size_if_scapegoat(node, parent);
        // Truthy if it's a scapegoat
        if (parent_size) {
            *should_rebuild = 0;
            sg_node_t* new_subtree = sg_rebuild(parent, parent_size);
            // Sg_Node** node_to_rewire = sg_should_be_right_child(node, parent) ? &parent->right : &parent->left;
            sg_node_t* grandparent = _sg_find_parent(tree, parent);
            sg_node_t** node_to_rewire = sg_should_be_right_child(parent, grandparent) ? &grandparent->right : &grandparent->left;
            *node_to_rewire = new_subtree;
            return *node_to_rewire;

        }
    }
    return new_node;
}

// `tree` is the pointer to the tree structure.
// `node` is the head node of a tree, unless its a recursive call.
// `parent` is the parent of `node`. It should be null on the first call.
// `value` is the value to be inserted.
// `depth` is the depth at which the node is inserted. Should always start at 0 if calling with head node.
// `should_rebuild` is a boolean which indicates whether the tree should be rebuilt when the stack unwinds.
// Returns pointer to new node if successfully inserted value into tree.
// Returns NULL if assertion failed for some reason.
static sg_node_t* _sg_insert_helper(
    sg_tree_t* tree, 
    sg_node_t* node, 
    sg_node_t* parent, 
    int value, 
    int* depth, 
    int* should_rebuild
) {
    (*depth)++;
    if (node == NULL) {
        node = (sg_node_t*) malloc(sizeof(sg_node_t));
        if (node == NULL) return NULL; 

        node->value = value;
        node->left  = NULL;
        node->right = NULL;

        if (sg_should_be_right_child(node, parent)) 
            parent->right = node; 
        else parent->left = node;

        tree->nodes++;
        tree->q++;

        *should_rebuild = (*depth > LOG_3_OVER_2(tree->q));

        return node;
    } 

    else if (node->value == value) {
        return NULL;
    } 

    // If the value to insert is greater or less than the value of the current node.
    else {
        // This function checks if the next child should be left or right.
        return _sg_insert_continue_or_trigger_rebuild(tree, 
                                                      node, 
                                                      parent, 
                                                      value, 
                                                      depth, 
                                                      should_rebuild
                                                      );
    }
}

// `node` is the the right child of `parent`.
// `parent` is the parent of the node given.
// This is a helper function for sg_delete_rewiring_helper().
// This function enters at the step where the deleted node has two children, and the
// in-order successor needs to be found to replace the deleted node.
// This function finds the in-order successor of `node`, and then rewires the `parent`
// of `node` to prepare for the caller to assign new children to the node to avoid
// clobbering any possible right child of the in-order successor.
static sg_node_t* _sg_prepare_inorder_successor(sg_node_t* node, sg_node_t* parent) {
    // Found a successor
    if (node->left == NULL) {
        // The node is the right child of the parent.
        // Maybe should do this in the caller because this always needs to happen somehow.
        if (parent->right == node) {
            node->left = parent->left;
            // return node;
        // The node is somewhere deeper inside the tree.
        } else {
            parent->left = node->right;
        }
        return node;
    } else return _sg_prepare_inorder_successor(node->left, node);
}

// This function determines which child to rewire of the parent during the deletion process.
// It covers all four possibilities of deletion: Both children are empty, one left or right child
// are empty, and when no children are empty.
static void _sg_delete_rewiring_helper(
    sg_node_t* node, 
    sg_node_t* parent
) {
    sg_node_t** node_to_rewire = sg_should_be_right_child(node, parent) ? &parent->right : &parent->left; 

    // Both children of current node are empty.
    if (node->left == NULL && node->right == NULL) {
        *node_to_rewire = NULL;
    }

    // Right child of current node is empty.
    else if (node->left != NULL && node->right == NULL) {
        *node_to_rewire = node->left;
    }

    // Left child of current node is empty.
    else if (node->left == NULL && node->right != NULL) {
        *node_to_rewire = node->right;   
    }

    // No child is empty.
    else if (node->left != NULL && node->right != NULL) {
        // Setup new hierarchy
        // This function will ensure that the nodes are rewired deeper in the tree,
        // if necessary.
        sg_node_t* new_node = _sg_prepare_inorder_successor(node->right, node);
        // In-order successor is somewhere deeper in the tree.
        if (new_node != node->right) {
            // If the new_node originally had a right child, 
            // it is already rewired in _sg_prepare_inorder_successor().
            // So this wont clobber anything.
            new_node->right = node->right;
            // If new_node is not the immediate right child of node, then this step
            // still needs to be done.
            new_node->left  = node->left;
        }
        *node_to_rewire = new_node;
    }
}

// A function to help sg_delete() perform its operations.
static sg_node_t* _sg_delete_helper(
    sg_tree_t* tree, 
    sg_node_t* node, 
    sg_node_t* parent, 
    int value
) {
    if (node == NULL) {
        return NULL;
    }  

    else if (node->value == value) {
        _sg_delete_rewiring_helper(node, parent);

        free(node);

        tree->nodes--;

        if ( ((double) tree->nodes) < ( (double) (tree->q) / 2.0 ) ) {
            sg_node_t* new_head = sg_rebuild(tree->head, tree->nodes);
            tree->head = new_head;
            tree->q = tree->nodes;
        }
        return parent;
    } 
    // If no node with value is found, deletion does not occur (nothing to delete).
    else {
        sg_node_t* next_node = (node->value > value) ? node->left : node->right;
        return _sg_delete_helper(tree, next_node, node, value);
    }
}
// Returns 1 if failed to delete for some reason.
// Returns 0 if insert succeeded.
int sg_delete(sg_tree_t* tree, int value) {
    // If tree is uninitialized, deletion failed.
    if (tree->head == NULL) return 1;
    // The node to be deleted happens to be the head node.
    if (value == tree->head->value) {
        // If the head has no children.
        if (tree->head->right == NULL && tree->head->left == NULL) {
            sg_node_t* old_head = tree->head;
            tree->head = NULL;
            free(old_head);
        }
        // The head has a left child but no right child.
        else if (tree->head->right != NULL && tree->head->left == NULL) {
            sg_node_t* old_head = tree->head;
            sg_node_t* new_head = tree->head->right;
            tree->head = new_head;

            free(old_head);
        }

        // The head has a right child but no left child.
        else if (tree->head->right == NULL && tree->head->left != NULL) {
            sg_node_t* old_head = tree->head;
            sg_node_t* new_head = tree->head->left;
            tree->head = new_head;

            free(old_head);
        }

        // The head has two direct children, and the in-order successor is the right child of the head.
        else if (tree->head->right != NULL && tree->head->left != NULL && tree->head->right->left == NULL) {
            sg_node_t* old_head = tree->head;
            sg_node_t* new_head = tree->head->right;
            new_head->left = tree->head->left;
            tree->head = new_head;

            free(old_head);
        } else {
        // The head has two direct children and (implicitly) the in-order successor is
        // not the right child of the head.
            sg_node_t* new_head = _sg_prepare_inorder_successor(tree->head->right, tree->head);
            sg_node_t* old_head = tree->head;
            new_head->left = tree->head->left;
            new_head->right = tree->head->right;
            tree->head = new_head;
            free(old_head);
        }

        tree->nodes--;
        if (( (double) tree->nodes) < ( (double) (tree->q) / 2.0 ) ) {
            sg_node_t* new_head = sg_rebuild(tree->head, tree->nodes);
            tree->head = new_head;
            tree->q = tree->nodes;
        }
        return 0;
    }

    sg_node_t* head = tree->head;
    sg_node_t* first_node = (head->value > value) ? head->left : head->right;
    sg_node_t* deleted_node = _sg_delete_helper(tree, first_node, head, value);
    if (deleted_node == NULL) return 1;
    else return 0;
}

// Returns 1 if failed to insert for some reason.
// Returns 0 if insert succeeded.
int sg_insert(sg_tree_t* tree, int value) {
    int depth = 0;
    int should_rebuild = 0;
    if (tree->head == NULL) {
        sg_node_t* new_node = malloc(sizeof(sg_node_t));
        new_node->value   = value;
        new_node->left    = NULL;
        new_node->right   = NULL;
        tree->head        = new_node;
        tree->nodes++;
        tree->q++;
        return 0;
    } else {
        sg_node_t* head = tree->head;
        // If head already has the value, don't insert it.
        if (head->value == value) return 1;
        // otherwise...
        sg_node_t* first_node = (head->value > value) ? head->left : head->right;
        sg_node_t* new_node = _sg_insert_helper(tree, first_node, head, value, &depth, &should_rebuild);
        if (new_node == NULL) return 1;
        else return 0;
    }
}


// A function to handle recursion for sg_find.
static sg_node_t* _sg_find_helper(sg_node_t* node, int n) {
    if (node == NULL) return NULL;
    if (node->value == n) return node;

    assert(node != NULL && node->value != n && "The previous two if-statements should ensure this condition is false.");

    return (n < node->value) 
        ? _sg_find_helper(node->left, n) 
        : _sg_find_helper(node->right, n);
    
}

sg_node_t* sg_find(sg_tree_t* tree, int n) {
    if (tree->head == NULL) return NULL; 
    else return _sg_find_helper(tree->head, n);
}

sg_node_t* sg_flatten(sg_node_t* node_ptr_x, sg_node_t* node_ptr_y) {
    if (node_ptr_x == NULL) 
        return node_ptr_y;
    node_ptr_x->right = sg_flatten(node_ptr_x->right, node_ptr_y);
    return sg_flatten(node_ptr_x->left, node_ptr_x);
}

sg_node_t* sg_build(sg_node_t* rt_list, int size) {
    if (size == 0) {
        rt_list->left = NULL;
        return rt_list;
    }
    double n_minus_1_over_2 = (((double) size - 1.0) / 2.0);
    sg_node_t* root   = sg_build(rt_list, (int) ceil(n_minus_1_over_2));
    sg_node_t* source = sg_build(root->right, (int) floor(n_minus_1_over_2));

    root->right  = source->left;
    source->left = root;
    return source;
}

sg_node_t* sg_rebuild(sg_node_t* scapegoat, int size) {
    sg_node_t* dummy = malloc(sizeof(sg_node_t));
    sg_node_t* flattened = sg_flatten(scapegoat, dummy);
    sg_build(flattened, size);
    sg_node_t* source = dummy->left;
    free(dummy);
    return source;
}

void _print_tree_inorder_helper(sg_node_t* node) {
    if (node == NULL)
        return;

    if (node->left != NULL)
        _print_tree_inorder_helper(node->left);

    printf("%i ", node->value);

    if (node->right != NULL)
        _print_tree_inorder_helper(node->right);
    
}

// Given a tree, prints the values of all nodes in that tree 
// using in-order traversal.
void print_tree_inorder(sg_tree_t* tree) {
    _print_tree_inorder_helper(tree->head);
    printf("\n");
}

void _print_tree_preorder_helper(sg_node_t* node) {
    if (node == NULL) 
        return;

    printf("%i ", node->value);

    if (node->left != NULL)
        _print_tree_preorder_helper(node->left);

    if (node->right != NULL)
        _print_tree_preorder_helper(node->right);
}

// Given a tree, prints the values of all nodes in that tree 
// using pre-order traversal.
void print_tree_preorder(sg_tree_t* tree) {
    _print_tree_preorder_helper(tree->head);
    printf("\n");
}

void _print_tree_postorder_helper(sg_node_t* node) {
    if (node == NULL)
        return;

    if (node->left != NULL) 
        _print_tree_postorder_helper(node->left);

    if (node->right != NULL)
        _print_tree_postorder_helper(node->right);

    printf("%i ", node->value);
}

// Given a tree, prints the values of all nodes in that tree 
// using post-order traversal.
void print_tree_postorder(sg_tree_t* tree) {
    _print_tree_postorder_helper(tree->head);
    printf("\n");
}

