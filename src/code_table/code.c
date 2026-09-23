#include "code.h"
#include <stdlib.h>
#include <string.h>

void init_code_table(CodeTable *table) {    //Initialize code table
    table->symbol_codes = NULL;
    table->counter = 0;
    table->capacity = 0;
}

void free_code_table(CodeTable *table) {    //Free code table iteratively
    for (size_t i = 0; i < table->counter; i++)
        free(table->symbol_codes[i].code);

    free(table->symbol_codes);

    table->symbol_codes = NULL;
    table->counter = 0;
    table->capacity = 0;
}

static int add_code(CodeTable *table, unsigned char symbol, const char *code) { //Add the symbol code to the code table
    if (table->counter == table->capacity) {
        size_t new_capacity;

        if (table->capacity == 0)
            new_capacity = 1;
        else
            new_capacity = table->capacity * 2;

        SymbolCodes *new_symbol_codes = realloc(table->symbol_codes, new_capacity * sizeof(SymbolCodes));

        if (!new_symbol_codes)
            return 1;

        table->symbol_codes = new_symbol_codes;
        table->capacity = new_capacity;
    }

    size_t code_length = strlen(code);
    char *new_code = malloc(code_length + 1);

    if (!new_code)
        return 1;

    strcpy(new_code, code);

    table->symbol_codes[table->counter].symbol = symbol;
    table->symbol_codes[table->counter].code = new_code;
    table->counter++;

    return 0;
}

static int DFS(const Node *node, char **path, size_t depth, CodeTable *table) { //Build the symbol code using the Huffman algorithm (DFS)
    if (!node)
        return 0;

    char *new_path = realloc(*path, depth + 1);

    if (!new_path)
        return 1;

    *path = new_path;

    if (node->left == NULL && node->right == NULL) {
        (*path)[depth] = '\0';

        return add_code(table, node->symbol, *path);
    }

    new_path = realloc(path, depth + 2);

    if (!new_path)
        return 1;

    *path = new_path;
    (*path)[depth] = 0;

    if (DFS(node->left, path, depth + 1, table) != 0)
        return 1;

    return 0;
}

int build_code_table(const Node *root, CodeTable *table) {  //Build the code table
    if (!root || !table)
        return 1;

    char *path = NULL;

    int result = DFS(root, &path, 0, table);

    free(path);

    return result;
}
