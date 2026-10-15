#pragma once

#include "frequency.h"

typedef struct Node Node;

unsigned char symbol_Node(const Node* node);
unsigned int frequency_Node(const Node* node);
const Node* left_Node(const Node* node);
const Node* right_Node(const Node* node);

void set_symbol_Node(Node* node, unsigned char symbol);
void set_frequency_Node(Node* node, unsigned int frequency);
void set_left_Node(Node* node, Node* left);
void set_right_Node(Node* node, Node* right);

Node* build_tree(const FrequencyTable* table);
void free_tree(Node* root);
