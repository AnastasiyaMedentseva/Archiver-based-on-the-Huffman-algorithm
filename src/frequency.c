#include "frequency.h"
#include <stddef.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct SymbolFrequency {    // Structure for frequency count
        unsigned char symbol;
        unsigned int frequency;
};

struct FrequencyTable {    // Structure for frequency table
        SymbolFrequency* symbols;
        size_t counter;
        size_t capacity;
};

unsigned char
symbol_SymbolFrequency(const SymbolFrequency* symbols) {    // Access to symbol field in SymbolFrequency structure
        return symbols->symbol;
}

unsigned int
frequency_SymbolFrequency(const SymbolFrequency* symbols) {    // Access to frequency field in SymbolFrequency structure
        return symbols->frequency;
}

SymbolFrequency*
symbols_FrequencyTable(const FrequencyTable* table) {    // Access to symbols field in FrequencyTable structure
        return table->symbols;
}

size_t counter_FrequencyTable(const FrequencyTable* table) {    // Access to counter field in FrequencyTable structure
        return table->counter;
}

size_t capacity_FrequencyTable(const FrequencyTable* table) {    // Access to capacity field in FrequencyTable structure
        return table->capacity;
}

SymbolFrequency* symbol_frequency_FrequencyTable(const FrequencyTable* table,
                                                 size_t index) {    // Access to frequency table element by index
        if (!table || index >= table->counter)
                return NULL;

        return &table->symbols[index];
}

void set_symbol_SymbolFrequency(SymbolFrequency* symbols,
                                unsigned char symbol) {    // Setter for symbol field in SymbolFrequency structure
        if (symbols)
                symbols->symbol = symbol;
}

void set_frequency_SymbolFrequency(
    SymbolFrequency* symbols, unsigned int frequency) {    // Setter for frequency field in SymbolFrequency structure
        if (symbols)
                symbols->frequency = frequency;
}

void set_symbols_FrequencyTable(
    FrequencyTable* table, SymbolFrequency* symbols) {    // Setter for  frequency field in FrequencyTable structure
        if (table)
                table->symbols = symbols;
}

void set_counter_FrequencyTable(FrequencyTable* table,
                                size_t counter) {    // Setter for counter field in FrequencyTable structure
        if (table)
                table->counter = counter;
}

void set_capacity_FrequencyTable(FrequencyTable* table,
                                 size_t capacity) {    // Setter for capacity field in FrequencyTable structure
        if (table)
                table->capacity = capacity;
}

void set_symbol_frequency_FrequencyTable(FrequencyTable* table, size_t index, unsigned char symbol,
                                         unsigned int frequency) {    // Setter for frequency table element by index
        if (!table || index >= table->counter)
                return;

        table->symbols[index].symbol = symbol;
        table->symbols[index].frequency = frequency;
}

void init_frequency_table(FrequencyTable* table) {    // Initialize frequency table
        if (!table)
                return;

        table->symbols = NULL;
        table->counter = 0;
        table->capacity = 0;
}

void free_frequency_table(FrequencyTable* table) {    // Free memory
        if (table == NULL)
                return;

        free(table->symbols);

        table->symbols = NULL;
        table->counter = 0;
        table->capacity = 0;
}

FrequencyTable* create_FrequencyTable(void) {    // Create frequency table
        FrequencyTable* table = malloc(sizeof(FrequencyTable));

        if (!table)
                return NULL;

        init_frequency_table(table);

        return table;
}

void destroy_FrequencyTable(FrequencyTable* table) {    // Destroy frequency table
        if (!table)
                return;

        free_frequency_table(table);
        free(table);
}

int reserve_FrequencyTable(FrequencyTable* table, size_t capacity) {    // Reserve frequency table
        if (!table)
                return FREQUENCY_INVALID_ARGUMENT;

        if (capacity <= table->capacity)
                return 0;

        SymbolFrequency* symbols = realloc(table->symbols, capacity * sizeof(SymbolFrequency));

        if (!symbols)
                return FREQUENCY_MEMORY_ERROR;

        table->symbols = symbols;
        table->capacity = capacity;

        return 0;
}

static int add_symbol(FrequencyTable* table, unsigned char symbol) {    // Add symbol to frequency table dynamically
        if (table->counter == table->capacity) {
                size_t new_capacity;

                if (table->capacity == 0)
                        new_capacity = 1;
                else
                        new_capacity = table->capacity * 2;

                SymbolFrequency* new_symbols = realloc(table->symbols, new_capacity * sizeof(SymbolFrequency));

                if (new_symbols == NULL)
                        return FREQUENCY_MEMORY_ERROR;

                table->symbols = new_symbols;
                table->capacity = new_capacity;
        }

        table->symbols[table->counter].symbol = symbol;
        table->symbols[table->counter].frequency = 1;
        table->counter++;

        return 0;
}

static int find_symbol(FrequencyTable* table, unsigned char symbol) {    // Linear search for symbol
        for (size_t i = 0; i < table->counter; i++) {
                if (table->symbols[i].symbol == symbol)
                        return (int)i;
        }

        return FREQUENCY_SYMBOL_NOT_FOUND;
}

int analyze_file(const char* file_name,
                 FrequencyTable* table) {    // Read file in binary format and fill frequency table
        if (file_name == NULL || table == NULL)
                return FREQUENCY_INVALID_ARGUMENT;

        FILE* file = fopen(file_name, "rb");

        if (file == NULL)
                return FREQUENCY_FILE_ERROR;

        int buffer;    // Per byte

        while ((buffer = fgetc(file)) != EOF) {
                int symbol_index = find_symbol(table, (unsigned char)buffer);

                if (symbol_index >= 0) {
                        if (table->symbols[symbol_index].frequency == UINT_MAX) {
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
                fclose(file);

                return FREQUENCY_FILE_ERROR;
        }

        fclose(file);

        return 0;
}
