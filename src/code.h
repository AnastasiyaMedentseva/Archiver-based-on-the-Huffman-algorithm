#pragma once

#include "tree.h"
#include <stddef.h>

#define CODE_MEMORY_ERROR 5
#define CODE_INVALID_ARGUMENT 6

typedef struct SymbolCode SymbolCode;

typedef struct CodeTable CodeTable;

unsigned char symbol_SymbolCode(const SymbolCode* symbol_code);
char* code_SymbolCode(const SymbolCode* symbol_code);

const SymbolCode* symbol_codes_CodeTable(const CodeTable* table);
size_t counter_CodeTable(const CodeTable* table);
size_t capacity_CodeTable(const CodeTable* table);
const SymbolCode* symbol_code_CodeTable(const CodeTable* table, size_t index);

void set_symbol_SymbolCode(SymbolCode* symbol_code, unsigned char symbol);
void set_code_SymbolCode(SymbolCode* symbol_code, char* code);

CodeTable* create_CodeTable(void);
void destroy_CodeTable(CodeTable* table);

int build_code_table(const Node* root, CodeTable* table);
