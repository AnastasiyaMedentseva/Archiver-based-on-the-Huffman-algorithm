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
        if (!table || index >= table->counter)    // Check for empty input and UB (access to element outside of array)
                return NULL;

        return &table->symbol_codes[index];
}

void set_symbol_SymbolCode(SymbolCode* symbol_code,
                           unsigned char symbol) {    // Setter for symbol field in SymbolCode structure
        if (symbol_code)                              // Check for non-empty input
                symbol_code->symbol = symbol;
}

void set_code_SymbolCode(SymbolCode* symbol_code, char* code) {    // Setter for code field in SymbolCode structure
        if (symbol_code)                                           // Check for non-empty input
                symbol_code->code = code;
}

void init_code_table(CodeTable* table) {    // Initialize code table
        table->symbol_codes = NULL;         // Set values for newly initialized code table
        table->counter = 0;
        table->capacity = 0;
}

void free_code_table(CodeTable* table) {    // Free code table iteratively
        if (!table)                         // Check for empty input
                return;

        for (size_t i = 0; i < table->counter; i++)    // In range of number of elements in code table
                free(table->symbol_codes[i].code);     // Free memory for every element in code table

        free(table->symbol_codes);    // Free memory for code table array of symbols with codes

        table->symbol_codes = NULL;    // Set values as for empty code table
        table->counter = 0;
        table->capacity = 0;
}

CodeTable* create_CodeTable(void) {                      // Create code table
        CodeTable* table = malloc(sizeof(CodeTable));    // Allocate memory for code table

        if (!table)    // Check for empty input
                return NULL;

        init_code_table(table);    // Initialize code table

        return table;
}

void destroy_CodeTable(CodeTable* table) {    // Destriy code table
        if (!table)                           // Check for empty input
                return;

        free_code_table(table);    // Free memory for every element of code table array of symbols with codes
        free(table);               // Free memory for code table
}

static int add_code(CodeTable* table, unsigned char symbol,
                    const char* code) {    // Add symbol code to code table dynamically
        if (!table || !code)               // Check for empty input
                return CODE_INVALID_ARGUMENT;

        if (table->counter == table->capacity) {    // Increase code table capacity dynamically
                size_t new_capacity;                // Variable for new capacity

                if (table->capacity == 0)    // New code table
                        new_capacity = 1;    // Default value for new code table
                else
                        new_capacity = table->capacity * 2;    // Increase double

                SymbolCode* new_symbol_codes = realloc(
                    table->symbol_codes,
                    new_capacity * sizeof(SymbolCode));    // Reallocation of new code table array of symbols with codes

                if (!new_symbol_codes)    // Check for realloc
                        return CODE_MEMORY_ERROR;

                table->symbol_codes = new_symbol_codes;    // Set new values
                table->capacity = new_capacity;
        }

        size_t code_length = strlen(code);           // Get length of code of symbol
        char* new_code = malloc(code_length + 1);    // Allocate memory for code of symbol, +1 for '\0'

        if (!new_code)    // Check for malloc
                return CODE_MEMORY_ERROR;

        memcpy(new_code, code, code_length + 1);    // Copy code of symbol into code table

        table->symbol_codes[table->counter].symbol = symbol;    // Set symbol into code table
        table->symbol_codes[table->counter].code = new_code;    // Set code of symbol into code table
        table->counter++;                                       // Increase number of elements in code table

        return 0;
}

static int DFS(const Node* node, char** path, size_t depth,
               CodeTable* table) {    // Build symbol code using the Huffman algorithm (DFS)
        if (!node)                    // Check for empty input
                return 0;

        char* new_path = realloc(*path, depth + 2);    // Guarantee enough memory for code (including "\0")

        if (!new_path)    // Check for realloc
                return CODE_MEMORY_ERROR;

        *path = new_path;    // Set new path

        const Node* node_left = left_Node(node);    // Set values for node
        const Node* node_right = right_Node(node);
        unsigned char node_symbol = symbol_Node(node);

        if (!node_left && !node_right) {    // Leaf node has symbol
                if (depth == 0) {           // Only one symbol - code is "0"
                        (*path)[0] = '0';
                        (*path)[1] = '\0';
                } else
                        (*path)[depth] = '\0';    // Last symbol, write "\0"

                return add_code(table, node_symbol, *path);    // Add code of symbol into code table
        }

        (*path)[depth] = '0';    // Go left and write "0"

        if (DFS(node_left, path, depth + 1, table) != 0)    // Check for DFS function
                return CODE_MEMORY_ERROR;

        (*path)[depth] = '1';    // Go right and write "1"

        if (DFS(node_right, path, depth + 1, table) != 0)    // Check for DFS function
                return CODE_MEMORY_ERROR;

        return 0;
}

int build_code_table(const Node* root, CodeTable* table) {    // Build code table
        if (!root || !table)                                  // Check for empty input
                return CODE_INVALID_ARGUMENT;

        char* path = NULL;                          // Initialize buffer path
        int result = DFS(root, &path, 0, table);    // Build code table using DFS
        free(path);                                 // Free memory for path

        return result;
}
