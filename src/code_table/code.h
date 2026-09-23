#include <stddef.h>
#include "../tree/tree.h"

typedef struct {    //Structure for matching symbol with its code
    unsigned char symbol;
    char* code;
} SymbolCode;

typedef struct {    //Structure for code table
    struct SymbolCode *symbol_codes;
    size_t counter;
    size_t capacity;
} CodeTable;

void init_code_table(CodeTable *table);
void free_code_table(CodeTable *table);
int build_code_table(Node *root, CodeTable *table);