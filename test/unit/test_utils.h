#pragma once

#include "../../src/code_table/code.h"
#include "../../src/frequency_table/frequency.h"
#include "../../src/tree/tree.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define TEST_DATA_DIR "test/test_data"

static inline const char* test_data_path(const char* name) {
        static char path[512];
        (void)snprintf(path, sizeof(path), "%s/%s", TEST_DATA_DIR, name);

        return path;
}

static inline const SymbolFrequency* find_frequency(const FrequencyTable* frequency_table, unsigned char symbol) {
        for (size_t i = 0; i < frequency_table->counter; ++i) {
                if (frequency_table->symbols[i].symbol == symbol)
                        return &frequency_table->symbols[i];
        }

        return NULL;
}

static inline const SymbolCode* find_code(const CodeTable* code_table, unsigned char symbol) {
        for (size_t i = 0; i < code_table->counter; ++i) {
                if (code_table->symbol_codes[i].symbol == symbol)
                        return &code_table->symbol_codes[i];
        }

        return NULL;
}

static inline int is_prefix_free(const CodeTable* code_table) {
        for (size_t i = 0; i < code_table->counter; ++i) {
                for (size_t j = i + 1; j < code_table->counter; ++j) {
                        const char* a = code_table->symbol_codes[i].code;
                        const char* b = code_table->symbol_codes[j].code;
                        size_t a_length = strlen(a);
                        size_t b_length = strlen(b);
                        size_t min_length = a_length < b_length ? a_length : b_length;

                        if (strncmp(a, b, min_length) == 0)
                                return 0;
                }
        }

        return 1;
}
