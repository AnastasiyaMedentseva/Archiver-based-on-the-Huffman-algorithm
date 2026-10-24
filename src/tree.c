#include "tree.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct Node {    // Structure for node in binary tree
        unsigned char symbol;
        unsigned int frequency;
        struct Node *left, *right;    // Pointer to left and right subtree
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
        if (node)                                           // Check for non-empty input
                node->symbol = symbol;
}

void set_frequency_Node(Node* node, unsigned int frequency) {    // Setter for frequency field in Node structure
        if (node)                                                // Check for non-empty input
                node->frequency = frequency;
}

void set_left_Node(Node* node, Node* left) {    // Setter for left field in Node structure
        if (node)                               // Check for not empty input
                node->left = left;
}

void set_right_Node(Node* node, Node* right) {    // Setter for right field in Node structure
        if (node)                                 // Check for not empty input
                node->right = right;
}

static Node* create_node(unsigned char symbol,
                         unsigned int frequency) {    // Create tree node for symbol with left and right subtrees
        Node* node = (Node*)malloc(sizeof(Node));     // Allocate memory for node

        if (!node)    // Check for malloc
                return NULL;

        node->symbol = symbol;    // Set values for node
        node->frequency = frequency;
        node->left = NULL;    // Yet no subtrees
        node->right = NULL;

        return node;
}

void free_tree(Node* root) {    // Free tree recursively by freeing nodes
        if (!root)              // Check for empty input
                return;

        free_tree(root->left);     // Free memory for left subtree
        free_tree(root->right);    // Free memory for right subtree
        free(root);                // Free memory for tree from root
}

static void free_tree_array(Node** tree_array, size_t length) {    // Free tree array
        for (size_t i = 0; i < length; i++)                        // In range of length of tree array
                free_tree(tree_array[i]);                          // Free memory of elements in tree array
}

static void find_lowest_frequency(
    Node** tree_array, size_t length, size_t* low_index_1,
    size_t* low_index_2) {    // Find two symbols with the lowest frequencies in order to merge subtrees
        *low_index_1 = 0;     // Set index 0 for low_index_1
        *low_index_2 = 1;     // Set index 1 for low_index_2

        if (tree_array[*low_index_1]->frequency >
            tree_array[*low_index_2]->frequency) {    // Guarantee the right order of subtrees with the lowest
                                                      // frequencies (low_index_1 < low_index_2)
                size_t temp = *low_index_1;           // Set frequencies in right order
                *low_index_1 = *low_index_2;
                *low_index_2 = temp;
        }

        for (size_t i = 2; i < length; i++) {    // Check for frequencies of other elements in tree array
                if (tree_array[i]->frequency <=
                    tree_array[*low_index_1]
                        ->frequency) {    // Check if frequency of element is lower than current lowest frequency
                        *low_index_2 = *low_index_1;    // Set new frequencies
                        *low_index_1 = i;
                } else if (tree_array[i]->frequency <
                           tree_array[*low_index_2]->frequency)    // Check if frequency of element is lower than
                                                                   // frequency of elemet with low_index_2
                        *low_index_2 = i;                          // Set new frequencies
        }
}

Node* build_tree(const FrequencyTable* table) {    // Build tree according to the Huffman algorithm
        if (!table)                                // Check for empty input
                return NULL;

        size_t table_counter = counter_FrequencyTable(table);    // Value for counter of frequncy table

        if (table_counter == 0)    // Check if frequency table is empty
                return NULL;

        Node** tree_array = malloc(table_counter * sizeof(Node*));    // Allocate memory for tree array

        if (!tree_array)    // Check for malloc
                return NULL;

        size_t tree_counter = 0;    // Value of counter of subtrees

        for (size_t i = 0; i < table_counter; i++) {    // Create tree for every symbol
                tree_array[i] = create_node(symbol_SymbolFrequency(symbol_frequency_FrequencyTable(table, i)),
                                            frequency_SymbolFrequency(symbol_frequency_FrequencyTable(
                                                table, i)));    // Create subtree with according values

                if (!tree_array[i]) {                                 // Check for create_node function
                        free_tree_array(tree_array, tree_counter);    // Free memory for tree array by elements
                        free(tree_array);                             // Free memory for tree array

                        return NULL;
                }

                tree_counter++;    // Increase tree counter
        }

        while (tree_counter > 1) {                  // Merge trees according to the frequencies
                size_t low_index_1, low_index_2;    // Values for indexes of lowest frequencies
                find_lowest_frequency(tree_array, tree_counter, &low_index_1,
                                      &low_index_2);    // Set lowest frequencies

                if (UINT_MAX - tree_array[low_index_1]->frequency <
                    tree_array[low_index_2]->frequency) {             // Check for overflow
                        free_tree_array(tree_array, tree_counter);    // Free memory for tree array by elements
                        free(tree_array);                             // Free memory for tree array

                        return NULL;
                }

                Node* parent =
                    create_node(0, tree_array[low_index_1]->frequency +
                                       tree_array[low_index_2]->frequency);    // Create parent node for merged trees

                if (!parent) {                                        // Check for create_node function
                        free_tree_array(tree_array, tree_counter);    // Free memory for tree array by elements
                        free(tree_array);                             // Free memory for tree array

                        return NULL;
                }

                parent->left = tree_array[low_index_1];     // Left subtree
                parent->right = tree_array[low_index_2];    // Right subtree

                tree_array[low_index_1] = parent;    // Set parent node as tree node with lowest frequency
                tree_array[low_index_2] =
                    tree_array[tree_counter -
                               1];    // Set last element in tree array as tree node with frequency with low_index_2

                tree_counter--;    // Decrease counter of subtrees because esubtrees were merged
        }

        Node* root = tree_array[0];    // Set first element in tree array as root of Huffman tree

        free(tree_array);    // Free memory for tree array

        return root;
}
