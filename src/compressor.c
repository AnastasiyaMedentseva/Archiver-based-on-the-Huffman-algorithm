#include "compressor.h"
#include <stddef.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static int write_metadata(
    FILE* output, const FrequencyTable* frequencies,
    unsigned int original_size) {    // Write metadata (frequency table and original size) for following decompression
        size_t frequencies_counter = counter_FrequencyTable(frequencies);
        unsigned int symbol_counter = (unsigned int)frequencies_counter;

        if (fwrite(&symbol_counter, sizeof(symbol_counter), 1, output) != 1)
                return COMPRESSOR_WRITE_ERROR;

        for (size_t i = 0; i < frequencies_counter; i++) {
                unsigned char symbol = symbol_SymbolFrequency(symbol_frequency_FrequencyTable(frequencies, i));
                unsigned int frequency = frequency_SymbolFrequency(symbol_frequency_FrequencyTable(frequencies, i));

                if (fwrite(&symbol, sizeof(symbol), 1, output) != 1)
                        return COMPRESSOR_WRITE_ERROR;

                if (fwrite(&frequency, sizeof(frequency), 1, output) != 1)
                        return COMPRESSOR_WRITE_ERROR;
        }

        if (fwrite(&original_size, sizeof(original_size), 1, output) != 1)
                return COMPRESSOR_WRITE_ERROR;

        return 0;
}

static const char* find_code(const CodeTable* symbol_codes, unsigned char symbol) {    // Find code for symbol
        size_t symbol_codes_counter = counter_CodeTable(symbol_codes);

        for (size_t i = 0; i < symbol_codes_counter; i++) {
                const SymbolCode* symbol_code = symbol_code_CodeTable(symbol_codes, i);

                if (symbol_SymbolCode(symbol_code) == symbol)
                        return code_SymbolCode(symbol_code);
        }

        return NULL;
}

static int write_compressed_data(FILE* input, FILE* output, const CodeTable* codes) {    // Read input file and code it
        BitWriter* writer = create_BitWriter(output);

        if (!writer)
                return COMPRESSOR_MEMORY_ERROR;

        int character;

        while ((character = fgetc(input)) != EOF) {
                const char* code = find_code(codes, (unsigned char)character);

                if (!code) {
                        destroy_BitWriter(writer);

                        return COMPRESSOR_CODE_NOT_FOUND;
                }

                for (size_t i = 0; code[i] != '\0'; i++) {
                        if (write_bit(writer, code[i] == '1') != 0) {
                                destroy_BitWriter(writer);

                                return COMPRESSOR_BITSTREAM_ERROR;
                        }
                }
        }

        if (ferror(input)) {
                destroy_BitWriter(writer);

                return COMPRESSOR_FILE_ERROR;
        }

        if (flush_bit_writer(writer) != 0) {
                destroy_BitWriter(writer);

                return COMPRESSOR_BITSTREAM_ERROR;
        }

        destroy_BitWriter(writer);

        return 0;
}

int compress(const char* input_filename, const char* output_filename) {    // Compress file using all modules
        if (!input_filename || !output_filename)
                return COMPRESSOR_INVALID_ARGUMENT;

        if (strcmp(input_filename, output_filename) == 0)
                return COMPRESSOR_INVALID_ARGUMENT;

        FILE* input = fopen(input_filename, "rb");

        if (!input)
                return COMPRESSOR_FILE_ERROR;

        FrequencyTable* frequencies = create_FrequencyTable();

        if (!frequencies) {
                fclose(input);

                return COMPRESSOR_MEMORY_ERROR;
        }

        int result = analyze_file(input_filename, frequencies);

        if (result != 0) {
                destroy_FrequencyTable(frequencies);
                fclose(input);

                if (result == FREQUENCY_MEMORY_ERROR)
                        return COMPRESSOR_MEMORY_ERROR;

                if (result == FREQUENCY_OVERFLOW)
                        return COMPRESSOR_OVERFLOW;

                return COMPRESSOR_FILE_ERROR;
        }

        unsigned int original_size = 0;

        for (size_t i = 0; i < counter_FrequencyTable(frequencies); i++) {
                unsigned int frequency = frequency_SymbolFrequency(symbol_frequency_FrequencyTable(frequencies, i));

                if (UINT_MAX - original_size < frequency) {
                        destroy_FrequencyTable(frequencies);
                        fclose(input);

                        return COMPRESSOR_OVERFLOW;
                }

                original_size += frequency;
        }

        FILE* output = fopen(output_filename, "wb");

        if (!output) {
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return COMPRESSOR_FILE_ERROR;
        }

        result = write_metadata(output, frequencies, original_size);

        if (result != 0) {
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return result;
        }

        if (counter_FrequencyTable(frequencies) == 0) {
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return 0;
        }

        Node* root = build_tree(frequencies);

        if (!root) {
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return COMPRESSOR_MEMORY_ERROR;
        }

        CodeTable* codes = create_CodeTable();

        if (!codes) {
                free_tree(root);
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return COMPRESSOR_MEMORY_ERROR;
        }

        result = build_code_table(root, codes);

        if (result != 0) {
                destroy_CodeTable(codes);
                free_tree(root);
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                if (result == CODE_MEMORY_ERROR)
                        return COMPRESSOR_MEMORY_ERROR;

                return COMPRESSOR_INVALID_ARGUMENT;
        }

        if (fseek(input, 0, SEEK_SET) != 0) {
                destroy_CodeTable(codes);
                free_tree(root);
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return COMPRESSOR_FILE_ERROR;
        }

        result = write_compressed_data(input, output, codes);

        if (fclose(output) != 0 && result == 0)
                result = COMPRESSOR_FILE_ERROR;

        destroy_CodeTable(codes);
        free_tree(root);
        destroy_FrequencyTable(frequencies);
        fclose(input);

        return result;
}
