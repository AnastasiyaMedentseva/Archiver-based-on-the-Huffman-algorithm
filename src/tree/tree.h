#pragma once

#include "../frequency_table/frequency.h"

typedef struct Node {    // Structure for node in binary tree
        unsigned char symbol;
        unsigned int frequency;
        struct Node *left, *right;
} Node;

Node* build_tree(const FrequencyTable* table);
void free_tree(Node* root);
