#include "code.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SymbolCode {    // Structure for matching symbol with its code
        unsigned char symbol;
        char* code;
};

struct CodeTable {    // Structure for code table
        SymbolCode* symbol_codes;
        size_t counter;
        size_t capacity;
};

unsigned char symbol_SymbolCode(const SymbolCode* symbol_code) {    // Access to symbol field in SymbolCode structure
        return symbol_code->symbol;
}

char* code_SymbolCode(const SymbolCode* symbol_code) {    // Access to code field in SymbolCode structure
        return symbol_code->code;
}

const SymbolCode*
symbol_codes_CodeTable(const CodeTable* table) {    // Access to symbol_codes field in CodeTable structure
        return table->symbol_codes;
}

size_t counter_CodeTable(const CodeTable* table) {    // Access to counter field in CodeTable structure
        return table->counter;
}

size_t capacity_CodeTable(const CodeTable* table) {    // Access to capacity field in CodeTable structure
        return table->capacity;
}

const SymbolCode* symbol_code_CodeTable(const CodeTable* table,
                                        size_t index) {    // Access to code table element by index
        if (!table || index >= table->counter)
                return NULL;

        return &table->symbol_codes[index];
}

void set_symbol_SymbolCode(SymbolCode* symbol_code,
                           unsigned char symbol) {    // Setter for symbol field in SymbolCode structure
        if (symbol_code)
                symbol_code->symbol = symbol;
}

void set_code_SymbolCode(SymbolCode* symbol_code, char* code) {    // Ssetter for code field in SymbolCode structure
        if (symbol_code)
                symbol_code->code = code;
}

void init_code_table(CodeTable* table) {    // Initialize code table
        table->symbol_codes = NULL;
        table->counter = 0;
        table->capacity = 0;
}

void free_code_table(CodeTable* table) {    // Free code table iteratively
        if (!table)
                return;

        for (size_t i = 0; i < table->counter; i++)
                free(table->symbol_codes[i].code);

        free(table->symbol_codes);

        table->symbol_codes = NULL;
        table->counter = 0;
        table->capacity = 0;
}

CodeTable* create_CodeTable(void) {    // Create code table
        CodeTable* table = malloc(sizeof(CodeTable));

        if (!table)
                return NULL;

        init_code_table(table);

        return table;
}

void destroy_CodeTable(CodeTable* table) {
        if (!table)
                return;

        free_code_table(table);
        free(table);
}

static int add_code(CodeTable* table, unsigned char symbol,
                    const char* code) {    // Add symbol code to code table dynamically
        if (!table || !code)
                return CODE_INVALID_ARGUMENT;

        if (table->counter == table->capacity) {
                size_t new_capacity;

                if (table->capacity == 0)
                        new_capacity = 1;
                else
                        new_capacity = table->capacity * 2;

                SymbolCode* new_symbol_codes = realloc(table->symbol_codes, new_capacity * sizeof(SymbolCode));

                if (!new_symbol_codes)
                        return CODE_MEMORY_ERROR;

                table->symbol_codes = new_symbol_codes;
                table->capacity = new_capacity;
        }

        size_t code_length = strlen(code);
        char* new_code = malloc(code_length + 1);

        if (!new_code)
                return CODE_MEMORY_ERROR;

        memcpy(new_code, code, code_length + 1);

        table->symbol_codes[table->counter].symbol = symbol;
        table->symbol_codes[table->counter].code = new_code;
        table->counter++;

        return 0;
}

static int DFS(const Node* node, char** path, size_t depth,
               CodeTable* table) {    // Build symbol code using the Huffman algorithm (DFS)
        if (!node)
                return 0;

        char* new_path = realloc(*path, depth + 2);

        if (!new_path)
                return CODE_MEMORY_ERROR;

        *path = new_path;

        const Node* node_left = left_Node(node);
        const Node* node_right = right_Node(node);
        unsigned char node_symbol = symbol_Node(node);

        if (!node_left && !node_right) {    // Leaf node has symbol
                if (depth == 0) {
                        (*path)[0] = '0';
                        (*path)[1] = '\0';
                } else
                        (*path)[depth] = '\0';

                return add_code(table, node_symbol, *path);
        }

        (*path)[depth] = '0';    // Go left and write "0"

        if (DFS(node_left, path, depth + 1, table) != 0)
                return CODE_MEMORY_ERROR;

        (*path)[depth] = '1';    // Go right and write "1"

        if (DFS(node_right, path, depth + 1, table) != 0)
                return CODE_MEMORY_ERROR;

        return 0;
}

int build_code_table(const Node* root, CodeTable* table) {    // Build code table
        if (!root || !table)
                return CODE_INVALID_ARGUMENT;

        char* path = NULL;
        int result = DFS(root, &path, 0, table);
        free(path);

        return result;
}
