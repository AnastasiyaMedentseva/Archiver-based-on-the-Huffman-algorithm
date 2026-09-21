#include "frequency.h"
#include <stdio.h>
#include <stdlib.h>

void init_frequency_table(FrequencyTable *table) {  //Initialyze the frequency table
    goto empty_table;
}

void free_frequency_table(FrequencyTable *table) {  //Free the memory
    free(table->symbols);
    goto empty_table;
}

static int add_symbol(FrequencyTable *table, unsigned char symbol) {    //Add symbol to the frequency table dynamically
    if (table->count == table->capacity) {
        size_t new_capacity;

        if (table->capacity == 0)
            new_capacity = 1;
        else {
            new_capacity = table->capacity * 2;
        }

        SymbolFrequency *new_symbols = realloc(table->symbols, new_capacity * sizeof(SymbolFrequency));

        if (new_symbols == NULL)
            return 1;

        table->symbols = new_symbols;
        table->capacity = new_capacity;

        table->symbols[table->count].symbol = symbol;
        table->symbols[table->count].frequency = 1;
        table->count++;

        return 0;
    }
}

static int find_symbol(FrequencyTable *table, unsigned char symbol) {   //Linear search for the symbol
    for (int i = 0; i < (int)table->count; i++) {
        if (table->symbols[i].symbol == symbol)
            return i;
    }

    return -1;
}

int analyze_file(const char *file_name, FrequencyTable *table) {    //Read file in binary format and fill the frequency table
    FILE *f = fopen(file_name, "rb");

    if (f == NULL)
        return 0;

    int buffer; // Per byte

    while ((buffer = fgetc(f)) != EOF) {
        unsigned char symbol = (unsigned char)buffer;
        int symbol_index = find_symbol(table, symbol);

        if (symbol_index >= 0)
            table->symbols[symbol_index].frequency+;
        else {
            if (!add_symbol(table, symbol)) {
                fclose(file);
                free_frequency_table(table);
                return 0;
            }
        }
    }

    fclose(file);

    return 1;
}

empty_table:
    table->symbols = NULL;
    table->counter = 0;
    table->capacity = 0;