#pragma once

#include "../src/code.h"
#include "../src/frequency.h"
#include "../src/tree.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define TEST_DATA_DIR "test/test_data"

static inline const char* test_data_path(const char* name) {
        static char path[512];
        (void)snprintf(path, sizeof(path), "%s/%s", TEST_DATA_DIR, name);

        return path;
}

static inline unsigned int find_frequency(const FrequencyTable* frequency_table, unsigned char symbol) {
        for (size_t i = 0; i < counter_FrequencyTable(frequency_table); ++i) {
                if (symbol_SymbolFrequency(symbol_frequency_FrequencyTable(frequency_table, i)) == symbol)
                        return frequency_SymbolFrequency(symbol_frequency_FrequencyTable(frequency_table, i));
        }

        return 0;
}

static inline const char* find_code(const CodeTable* code_table, unsigned char symbol) {
        for (size_t i = 0; i < counter_CodeTable(code_table); ++i) {
                if (symbol_SymbolCode(symbol_code_CodeTable(code_table, i)) == symbol)
                        return code_SymbolCode(symbol_code_CodeTable(code_table, i));
        }

        return NULL;
}

static inline int is_prefix_free(const CodeTable* code_table) {
        for (size_t i = 0; i < counter_CodeTable(code_table); ++i) {
                for (size_t j = i + 1; j < counter_CodeTable(code_table); ++j) {
                        const char* a = code_SymbolCode(symbol_code_CodeTable(code_table, i));
                        const char* b = code_SymbolCode(symbol_code_CodeTable(code_table, j));
                        size_t a_length = strlen(a);
                        size_t b_length = strlen(b);
                        size_t min_length = a_length < b_length ? a_length : b_length;

                        if (strncmp(a, b, min_length) == 0)
                                return 0;
                }
        }

        return 1;
}
