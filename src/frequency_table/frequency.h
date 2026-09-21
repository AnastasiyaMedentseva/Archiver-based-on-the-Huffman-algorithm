#include <stddef.h>

typedef struct {    //Structure for frequency count
    unsigned char symbol;
    unsigned int frequency;
} SymbolFrequency;

typedef struct {    //Structure for frequency table
    SymbolFrequency *symbols;
    size_t counter; //Unique symbols counter
    size_t capacity;
} FrequencyTable;

void init_frequency_table(FrequencyTable *table);
void free_frequency_table(FrequencyTable *table);
int analyze_file(const char *filename, FrequencyTable *table);