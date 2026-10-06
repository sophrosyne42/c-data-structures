// Katherine Agent
// demo.c

#include "../scapegoat.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    // No command-line arguments allowed.
    if (argc != 1 ){
        printf("%d, %s", argc, *argv);
        return 1;
    }
    sg_tree_t tree = { .nodes = 0, .q = 0, .head = NULL };
    
    // No specification on the string size of the input, so...
    // For now, I assume it is at most 10000.
    char line[10000];
    char cmd = '\0';
    char subcmd = '\0';
    int input = -1;

    while (cmd != 'q') {

        scanf("%s", line);
        // Per the input specification, the first character of an input group is
        // the commad identifier.
        cmd = *line;
        switch (cmd) {
            case 'i':
                scanf("%i", &input);
                sg_insert(&tree, input);
            break;
            case 'd':
                scanf("%i", &input);
                sg_delete(&tree, input);
            break;
            case 's':
                scanf("%i", &input);
                sg_find(&tree, input)
                    ? printf("%d is present\n", input) 
                    : printf("%d is missing\n", input);
            break;
            // Setting case 'q' like this ensures that the program
            // cleans up all the dynamic memory before exiting.
            case 'q':
            case 'e':
                while (tree.head) {
                    sg_delete(&tree, tree.head->value);
                }
            break;
            case 't':
                scanf("%s", &subcmd);
                switch (subcmd) {
                    case 'i': print_tree_inorder(&tree); break;
                    case 'l': print_tree_preorder(&tree); break;
                    case 'r': print_tree_postorder(&tree); break;
                }
            break;
        }
    }
    return 0;
}

