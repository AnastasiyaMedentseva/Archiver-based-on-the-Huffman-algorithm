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
        if (!table || index >= table->counter)    // Check for empty input and UB (access to element outside of array)
                return NULL;

        return &table->symbols[index];
}

void set_symbol_SymbolFrequency(SymbolFrequency* symbols,
                                unsigned char symbol) {    // Setter for symbol field in SymbolFrequency structure
        if (symbols)                                       // Check for non-empty input
                symbols->symbol = symbol;
}

void set_frequency_SymbolFrequency(
    SymbolFrequency* symbols, unsigned int frequency) {    // Setter for frequency field in SymbolFrequency structure
        if (symbols)                                       // Check for non-empty input
                symbols->frequency = frequency;
}

void set_symbols_FrequencyTable(
    FrequencyTable* table, SymbolFrequency* symbols) {    // Setter for  frequency field in FrequencyTable structure
        if (table)                                        // Check for non-empty input
                table->symbols = symbols;
}

void set_counter_FrequencyTable(FrequencyTable* table,
                                size_t counter) {    // Setter for counter field in FrequencyTable structure
        if (table)                                   // Check for non-empty input
                table->counter = counter;
}

void set_capacity_FrequencyTable(FrequencyTable* table,
                                 size_t capacity) {    // Setter for capacity field in FrequencyTable structure
        if (table)                                     // Check for non-empty input
                table->capacity = capacity;
}

void set_symbol_frequency_FrequencyTable(FrequencyTable* table, size_t index, unsigned char symbol,
                                         unsigned int frequency) {    // Setter for frequency table element by index
        if (!table || index >= table->counter)    // Check for empty input and UB (access to element outside of array)
                return;

        table->symbols[index].symbol = symbol;    // Set values for frequency table
        table->symbols[index].frequency = frequency;
}

void init_frequency_table(FrequencyTable* table) {    // Initialize frequency table
        if (!table)                                   // Check for empty input
                return;

        table->symbols = NULL;    // Set values for frequency table initialization
        table->counter = 0;
        table->capacity = 0;
}

void free_frequency_table(FrequencyTable* table) {    // Free memory
        if (!table)                                   // Check for empty input
                return;

        free(table->symbols);    // Free memory for elements in frequency table

        table->symbols = NULL;    // Set values as for newly initialized frquency table
        table->counter = 0;
        table->capacity = 0;
}

FrequencyTable* create_FrequencyTable(void) {                      // Create frequency table
        FrequencyTable* table = malloc(sizeof(FrequencyTable));    // Allocate memory for frequency table

        if (!table)    // Check for empty input
                return NULL;

        init_frequency_table(table);    // Initialize frequency table

        return table;
}

void destroy_FrequencyTable(FrequencyTable* table) {    // Destroy frequency table
        if (!table)                                     // Check for empty input
                return;

        free_frequency_table(
            table);     // Free memory for frequency table and set values as for newly initialized frequency table
        free(table);    // Free memory allocated for frequency table
}

int reserve_FrequencyTable(FrequencyTable* table, size_t capacity) {    // Reserve frequency table
        if (!table)                                                     // Check for empty input
                return FREQUENCY_INVALID_ARGUMENT;

        if (capacity <= table->capacity)    // Check if capacity is enough
                return 0;

        SymbolFrequency* symbols = realloc(
            table->symbols, capacity * sizeof(SymbolFrequency));    // Reallocate memory according to new capacity

        if (!symbols)    // Check for realloc
                return FREQUENCY_MEMORY_ERROR;

        table->symbols = symbols;    // Set new values in frequency table
        table->capacity = capacity;

        return 0;
}

static int add_symbol(FrequencyTable* table, unsigned char symbol) {    // Add symbol to frequency table dynamically
        if (table->counter == table->capacity) {                        // Increase frequency table capacity dynamically
                size_t new_capacity;                                    // Variable for new capacity

                if (table->capacity == 0)    // New frequency table
                        new_capacity = 1;
                else
                        new_capacity = table->capacity * 2;    // Increase double

                SymbolFrequency* new_symbols =
                    realloc(table->symbols,
                            new_capacity * sizeof(SymbolFrequency));    // Reallocate memory according to new capacity

                if (!new_symbols)    // Check for realloc
                        return FREQUENCY_MEMORY_ERROR;

                table->symbols = new_symbols;    // Set new values in frequency table
                table->capacity = new_capacity;
        }

        table->symbols[table->counter].symbol = symbol;    // Set symbol in frequency table
        table->symbols[table->counter].frequency = 1;      // Set frequency of symbol in frequency table
        table->counter++;                                  // Increase counter of elements in frequency table

        return 0;
}

static int find_symbol(FrequencyTable* table, unsigned char symbol) {    // Linear search for symbol
        for (size_t i = 0; i < table->counter; i++) {      // In range of number of elements in frequency table
                if (table->symbols[i].symbol == symbol)    // Searched symbol
                        return (int)i;                     // Return index of symbol element in symbols array
        }

        return FREQUENCY_SYMBOL_NOT_FOUND;
}

int analyze_file(const char* file_name,
                 FrequencyTable* table) {    // Read file in binary format and fill frequency table
        if (!file_name || !table)            // Check for empty input
                return FREQUENCY_INVALID_ARGUMENT;

        FILE* file = fopen(file_name, "rb");    // Open input file for reading in binary format

        if (!file)    // Check for fopen
                return FREQUENCY_FILE_ERROR;

        int buffer;    // Per byte

        while ((buffer = fgetc(file)) != EOF) {                                  // Get bits from input file until EOF
                int symbol_index = find_symbol(table, (unsigned char)buffer);    // Symbol index of symbol in buffer

                if (symbol_index >= 0) {    // Check if symbol in buffer is in frequency table
                        if (table->symbols[symbol_index].frequency == UINT_MAX) {    // Check for overflow
                                fclose(file);

                                return FREQUENCY_OVERFLOW;
                        }

                        table->symbols[symbol_index].frequency++;    // Increase symbol frequency
                } else {
                        if (add_symbol(table, (unsigned char)buffer) != 0) {    // Check for add_symbol function
                                fclose(file);                                   // Close input file and free memory

                                return add_symbol(table, (unsigned char)buffer);    // Add symbol in frequency table
                        }
                }
        }

        if (ferror(file)) {    // Check for input file stream error
                fclose(file);

                return FREQUENCY_FILE_ERROR;
        }

        fclose(file);    // Close file after successful reading in binary format

        return 0;
}
