#pragma once

#include <stddef.h>

#define FREQUENCY_SYMBOL_NOT_FOUND -1
#define FREQUENCY_MEMORY_ERROR 1
#define FREQUENCY_FILE_ERROR 2
#define FREQUENCY_INVALID_ARGUMENT 3
#define FREQUENCY_OVERFLOW 4

typedef struct SymbolFrequency SymbolFrequency;

typedef struct FrequencyTable FrequencyTable;

unsigned char symbol_SymbolFrequency(const SymbolFrequency* symbols);
unsigned int frequency_SymbolFrequency(const SymbolFrequency* symbols);

SymbolFrequency* symbols_FrequencyTable(const FrequencyTable* table);
size_t counter_FrequencyTable(const FrequencyTable* table);
size_t capacity_FrequencyTable(const FrequencyTable* table);
SymbolFrequency* symbol_frequency_FrequencyTable(const FrequencyTable* table, size_t index);

void set_symbol_SymbolFrequency(SymbolFrequency* symbols, unsigned char symbol);
void set_frequency_SymbolFrequency(SymbolFrequency* symbols, unsigned int frequency);

void set_symbols_FrequencyTable(FrequencyTable* table, SymbolFrequency* symbols);
void set_counter_FrequencyTable(FrequencyTable* table, size_t counter);
void set_capacity_FrequencyTable(FrequencyTable* table, size_t capacity);
void set_symbol_frequency_FrequencyTable(FrequencyTable* table, size_t index, unsigned char symbol,
                                         unsigned int frequency);

FrequencyTable* create_FrequencyTable(void);
void destroy_FrequencyTable(FrequencyTable* table);
int reserve_FrequencyTable(FrequencyTable* table, size_t capacity);

int analyze_file(const char* file_name, FrequencyTable* table);
