#include "frequency.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void init_frequency_table(FrequencyTable *table) {  //Initialize the frequency table
    if (!table)
        return;

    table->symbols = NULL;
    table->counter = 0;
    table->capacity = 0;
}

void free_frequency_table(FrequencyTable *table) {  //Free the memory
    if (table == NULL)
        return;

    free(table->symbols);

    table->symbols = NULL;
    table->counter = 0;
    table->capacity = 0;
}

static int add_symbol(FrequencyTable *table, unsigned char symbol) {    //Add the symbol to the frequency table dynamically
    if (table->counter == table->capacity) {
        size_t new_capacity;

        if (table->capacity == 0)
            new_capacity = 1;
        else
            new_capacity = table->capacity * 2;

        SymbolFrequency *new_symbols = realloc(table->symbols, new_capacity * sizeof(SymbolFrequency));

        if (new_symbols == NULL) {
            printf("Error: could not reallocate memory for frequency table\n");

            return FREQUENCY_MEMORY_ERROR;
        }

        table->symbols = new_symbols;
        table->capacity = new_capacity;
    }

    table->symbols[table->counter].symbol = symbol;
    table->symbols[table->counter].frequency = 1;
    table->counter++;

    return 0;
}

static int find_symbol(FrequencyTable *table, unsigned char symbol) {   //Linear search for the symbol
    for (size_t i = 0; i < table->counter; i++) {
        if (table->symbols[i].symbol == symbol)
            return (int)i;
    }

    return FREQUENCY_SYMBOL_NOT_FOUND;
}

int analyze_file(const char *file_name, FrequencyTable *table) {    //Read the file in binary format and fill the frequency table
    if (file_name == NULL || table == NULL) {
        printf("Error: invalid argument in analyze_file\n");

        return FREQUENCY_INVALID_ARGUMENT;
    }

    FILE *file = fopen(file_name, "rb");

    if (file == NULL) {
        printf("Error: could not open file\n");

        return FREQUENCY_FILE_ERROR;
    }

    int buffer; // Per byte

    while ((buffer = fgetc(file)) != EOF) {
        int symbol_index = find_symbol(table, (unsigned char)buffer);

        if (symbol_index >= 0) {
            if (table->symbols[symbol_index].frequency == UINT_MAX) {
                printf("Error: symbol frequency overflow\n");
                fclose(file);

                return FREQUENCY_OVERFLOW;
            }

            table->symbols[symbol_index].frequency++;
        } else {
            int result = add_symbol(table, (unsigned char)buffer);

            if (result != 0) {
                fclose(file);

                return result;
            }
        }
    }

    if (ferror(file)) {
        printf("Error: could not read file\n");
        fclose(file);

        return FREQUENCY_FILE_ERROR;
    }

    fclose(file);

    return 0;
}