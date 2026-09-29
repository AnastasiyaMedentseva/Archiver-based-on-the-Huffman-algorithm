#pragma once

#include <stddef.h>

#define FREQUENCY_SYMBOL_NOT_FOUND -1    // Error codes
#define FREQUENCY_MEMORY_ERROR 1
#define FREQUENCY_FILE_ERROR 2
#define FREQUENCY_INVALID_ARGUMENT 3
#define FREQUENCY_OVERFLOW 4

typedef struct {    // Structure for frequency count
        unsigned char symbol;
        unsigned int frequency;
} SymbolFrequency;

typedef struct {    // Structure for frequency table
        SymbolFrequency* symbols;
        size_t counter;    // Unique symbols counter
        size_t capacity;
} FrequencyTable;

void init_frequency_table(FrequencyTable* table);
void free_frequency_table(FrequencyTable* table);
int analyze_file(const char* file_name, FrequencyTable* table);
