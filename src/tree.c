#include "tree.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct Node {    // Structure for node in binary tree
        unsigned char symbol;
        unsigned int frequency;
        struct Node *left, *right;
};

unsigned char symbol_Node(const Node* node) {    // Access to symbol field in Node structure
        return node->symbol;
}

unsigned int frequency_Node(const Node* node) {    // Access to frequency field in Node structure
        return node->frequency;
}

const Node* left_Node(const Node* node) {    // Access to left field in Node structure
        return node->left;
}

const Node* right_Node(const Node* node) {    // Acces to right field in Node structure
        return node->right;
}

void set_symbol_Node(Node* node, unsigned char symbol) {    // Setter for symbol field in Node structure
        if (node)
                node->symbol = symbol;
}

void set_frequency_Node(Node* node, unsigned int frequency) {    // Setter for frequency field in Node structure
        if (node)
                node->frequency = frequency;
}

void set_left_Node(Node* node, Node* left) {    // Setter for left field in Node structure
        if (node)
                node->left = left;
}

void set_right_Node(Node* node, Node* right) {    // Setter for right field in Node structure
        if (node)
                node->right = right;
}

static Node* create_node(unsigned char symbol,
                         unsigned int frequency) {    // Create tree node for symbol with left and right subtrees
        Node* node = (Node*)malloc(sizeof(Node));

        if (!node)
                return NULL;

        node->symbol = symbol;
        node->frequency = frequency;
        node->left = NULL;
        node->right = NULL;

        return node;
}

void free_tree(Node* root) {    // Free tree recursively by freeing nodes
        if (!root)
                return;

        free_tree(root->left);
        free_tree(root->right);
        free(root);
}

static void free_tree_array(Node** tree_array, size_t length) {    // Free tree array
        for (size_t i = 0; i < length; i++)
                free_tree(tree_array[i]);
}

static void find_lowest_frequency(
    Node** tree_array, size_t length, size_t* low_index_1,
    size_t* low_index_2) {    // Find two symbols with the lowest frequencies in order to merge subtrees
        *low_index_1 = 0;
        *low_index_2 = 1;

        if (tree_array[*low_index_1]->frequency >
            tree_array[*low_index_2]->frequency) {    // Guarantee the right order of subtrees with the lowest
                                                      // frequencies (low_index_1 < low_index_2)
                size_t temp = *low_index_1;
                *low_index_1 = *low_index_2;
                *low_index_2 = temp;
        }

        for (size_t i = 2; i < length; i++) {
                if (tree_array[i]->frequency <= tree_array[*low_index_1]->frequency) {
                        *low_index_2 = *low_index_1;
                        *low_index_1 = i;
                } else if (tree_array[i]->frequency < tree_array[*low_index_2]->frequency)
                        *low_index_2 = i;
        }
}

Node* build_tree(const FrequencyTable* table) {    // Build tree according to the Huffman algorithm
        if (!table)
                return NULL;

        size_t table_counter = counter_FrequencyTable(table);

        if (table_counter == 0)
                return NULL;

        Node** tree_array = malloc(table_counter * sizeof(Node*));

        if (!tree_array)
                return NULL;

        size_t tree_counter = 0;

        for (size_t i = 0; i < table_counter; i++) {    // Create tree for every symbol
                tree_array[i] = create_node(symbol_SymbolFrequency(symbol_frequency_FrequencyTable(table, i)),
                                            frequency_SymbolFrequency(symbol_frequency_FrequencyTable(table, i)));

                if (!tree_array[i]) {
                        free_tree_array(tree_array, tree_counter);
                        free(tree_array);

                        return NULL;
                }

                tree_counter++;
        }

        while (tree_counter > 1) {    // Merge trees according to the frequencies
                size_t low_index_1, low_index_2;
                find_lowest_frequency(tree_array, tree_counter, &low_index_1, &low_index_2);

                if (UINT_MAX - tree_array[low_index_1]->frequency < tree_array[low_index_2]->frequency) {
                        free_tree_array(tree_array, tree_counter);
                        free(tree_array);

                        return NULL;
                }

                Node* parent =
                    create_node(0, tree_array[low_index_1]->frequency +
                                       tree_array[low_index_2]->frequency);    // Create parent node for merged trees

                if (!parent) {
                        free_tree_array(tree_array, tree_counter);
                        free(tree_array);

                        return NULL;
                }

                parent->left = tree_array[low_index_1];     // Left subtree
                parent->right = tree_array[low_index_2];    // Right subtree

                tree_array[low_index_1] = parent;
                tree_array[low_index_2] = tree_array[tree_counter - 1];

                tree_counter--;
        }

        Node* root = tree_array[0];

        free(tree_array);

        return root;
}
