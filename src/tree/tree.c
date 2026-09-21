#include <tree.h>
#include <stdlib.h>

static Node *create_node(unsigned char symbol, unsigned int frequency) {    //Create tree node for a symbol with left and right subtrees
    Node *node = (Node *)malloc(sizeof(Node));

    if (!node)
        return NULL;

    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void free_tree(Node *root) {    //Free tree recursively by freeing nodes
    if (!root)
        return;

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static void free_tree_array(Node **tree_array, size_t length) {  //Free tree array via freeing subtrees one by one
    for (size_t i = 0; i < length; i++)
        free_tree(tree_array[i]);
}

static void find_lowest_frequency(Node **tree_array, size_t length, size_t *low_index_1, size_t *low_index_2) { //Find two symbols with lowest frequencies in order to merge the subtrees
    *low_index_1 = 0;
    *low_index_2 = 1;

    if (tree_array[*low_index_1]->frequency > tree_array[*low_index_2]->frequency) {    //Guarantee the right order of subtrees with lowest frequencies (low_index_1 < low_index_2)
        size_t temp = *low_index_1;
        *low_index_1 = *low_index_2;
        *low_index_2 = temp;
    }

    for (size_t i = 2; i < length; i++) {
        if (tree_array[i]->frequency <= tree_array[*low_index_1]->frequency) {
            *low_index_2 = *low_index_1;
            *low_index_1 = i;
        } else if (tree_array[i]->frequency < tree_array[*low_index_2]->frequency) {
            *low_index_2 = i;
        }
    }
}

Node *build_tree(const FrequencyTable *table) { //Build tree according tot the Huffman algorithm
    if (!table || table->counter == 0)
        return NULL;

    size_t tree_counter = 0;

    for (size_t i = 0; i < table->counter; i++) {   //Create tree for every symbol
        trees[i] = create_node(table->symbols[i].symbol, table->symbols[i].frequency);

        if (!trees[i]) {
            free_tree_array(trees, tree_counter);
            free(tree_array);

            return NULL;
        }

        tree_counter++;
    }

    while (tree_counter > 1) {  //Merge trees according to the frequencies
        size_t low_index_1, low_index_2;
        find_lowest_frequency(tree_array, tree_counter, &low_index_1, &low_index_2);

        Node *parent = malloc(sizeof(Node));    //Create parent node for merged trees

        if (!parent) {
            free_tree_array(tree_array, tree_counter);
            free(tree_array);
            return NULL;
        }

        parent->symbol = 0;
        parent->frequency = tree_array[low_index_1]->frequency + tree_array[low_index_2]->frequency;    //Frequency of two merged subtrees with the lowest frequencies
        parent->left = tree_array[low_index_1]; //Left subtree
        parent->right = tree_array[low_index_2];    //Right subtree

        tree_array[low_index_1] = parent;
        tree_array[low_index_2] = tree_array[tree_counter - 1];

        tree_counter--;
    }

    Node *root = tree_array[0];

    free(tree_array);

    return root;
}