#pragma once

#include "../tree/tree.h"
#include <stddef.h>

#define CODE_MEMORY_ERROR 5
#define CODE_INVALID_ARGUMENT 6

typedef struct {    // Structure for matching symbol with its code
        unsigned char symbol;
        char* code;
} SymbolCode;

typedef struct {    // Structure for code table
        struct SymbolCode* symbol_codes;
        size_t counter;
        size_t capacity;
} CodeTable;

void init_code_table(CodeTable* table);
void free_code_table(CodeTable* table);
int build_code_table(const Node* root, CodeTable* table);
