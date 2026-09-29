#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int add_code(CodeTable* table, unsigned char symbol,
                    const char* code) {    // Add symbol code to code table dynamically
        if (!table || !code) {
                printf("Error: invalid argment in add_code\n");

                return CODE_INVALID_ARGUMENT;
        }

        if (table->counter == table->capacity) {
                size_t new_capacity;

                if (table->capacity == 0)
                        new_capacity = 1;
                else
                        new_capacity = table->capacity * 2;

                SymbolCode* new_symbol_codes = realloc(table->symbol_codes, new_capacity * sizeof(SymbolCode));

                if (!new_symbol_codes) {
                        printf("Error: could not allocate memory for code table\n");

                        return CODE_MEMORY_ERROR;
                }

                table->symbol_codes = new_symbol_codes;
                table->capacity = new_capacity;
        }

        size_t code_length = strlen(code);
        char* new_code = malloc(code_length + 1);

        if (!new_code) {
                printf("Error: could not allocate memory for symbol code\n");

                return CODE_MEMORY_ERROR;
        }

        memcpy(new_code, code, code_length + 1);

        table->symbol_codes[table->counter].symbol = symbol;
        table->symbol_codes[table->counter].code = new_code;
        table->counter++;

        return 0;
}

static int DFS(const Node* node, char** path, size_t depth,
               CodeTable* table) {    // Build the symbol code using the Huffman algorithm (DFS)
        if (!node)
                return 0;

        char* new_path = realloc(*path, depth + 2);

        if (!new_path) {
                printf("Error: could not allocate memory for DFS path\n");

                return CODE_MEMORY_ERROR;
        }

        *path = new_path;

        if (!node->left && !node->right) {    // Leaf node has symbol
                if (depth == 0) {
                        (*path)[0] = '0';
                        (*path)[1] = '\0';
                } else
                        (*path)[depth] = '\0';

                return add_code(table, node->symbol, *path);
        }

        (*path)[depth] = '0';    // Go left and write "0"

        if (DFS(node->left, path, depth + 1, table) != 0)
                return CODE_MEMORY_ERROR;

        (*path)[depth] = '1';    // Go right and write "1"

        if (DFS(node->right, path, depth + 1, table) != 0)
                return CODE_MEMORY_ERROR;

        return 0;
}

int build_code_table(const Node* root, CodeTable* table) {    // Build code table
        if (!root || !table) {
                printf("Error: invalid argument in build_code_table\n");

                return CODE_INVALID_ARGUMENT;
        }

        char* path = NULL;
        int result = DFS(root, &path, 0, table);
        free(path);

        return result;
}
