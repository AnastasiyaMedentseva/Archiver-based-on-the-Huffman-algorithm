#include "decompressor.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_metadata(FILE* input, FrequencyTable* frequencies, unsigned int* original_size) {
        // Read metadata (frequencies, original size) from compressed input file
        unsigned int symbol_counter;

        if (fread(&symbol_counter, sizeof(symbol_counter), 1, input) != 1)
                return DECOMPRESSOR_ARCHIVE_ERROR;

        if (reserve_FrequencyTable(frequencies, symbol_counter) != 0)
                return DECOMPRESSOR_MEMORY_ERROR;

        set_counter_FrequencyTable(frequencies, symbol_counter);

        for (size_t i = 0; i < symbol_counter; i++) {
                unsigned char symbol;
                unsigned int frequency;

                if (fread(&symbol, sizeof(symbol), 1, input) != 1)
                        return DECOMPRESSOR_ARCHIVE_ERROR;

                if (fread(&frequency, sizeof(frequency), 1, input) != 1)
                        return DECOMPRESSOR_ARCHIVE_ERROR;

                if (frequency == 0)
                        return DECOMPRESSOR_ARCHIVE_ERROR;

                set_symbol_frequency_FrequencyTable(frequencies, i, symbol, frequency);
        }

        if (fread(original_size, sizeof(*original_size), 1, input) != 1)
                return DECOMPRESSOR_ARCHIVE_ERROR;

        return 0;
}

static int write_decompressed_data(FILE* input, FILE* output, const Node* root,
                                   unsigned int original_size) {    // Decode file
        if (original_size == 0)
                return 0;

        if (!root)
                return DECOMPRESSOR_ARCHIVE_ERROR;

        BitReader* reader = create_BitReader(input);

        if (!reader)
                return DECOMPRESSOR_MEMORY_ERROR;

        if (!left_Node(root) && !right_Node(root)) {
                for (unsigned int i = 0; i < original_size; i++) {
                        unsigned char bit;

                        if (read_bit(reader, &bit) != 0) {
                                destroy_BitReader(reader);

                                return DECOMPRESSOR_BITSTREAM_ERROR;
                        }

                        if (bit != 0) {
                                destroy_BitReader(reader);

                                return DECOMPRESSOR_ARCHIVE_ERROR;
                        }

                        if (fputc(symbol_Node(root), output) == EOF) {
                                destroy_BitReader(reader);

                                return DECOMPRESSOR_FILE_ERROR;
                        }
                }

                destroy_BitReader(reader);

                return 0;
        }

        const Node* current = root;
        unsigned int written = 0;

        while (written < original_size) {
                unsigned char bit;

                if (read_bit(reader, &bit) != 0) {
                        destroy_BitReader(reader);

                        return DECOMPRESSOR_BITSTREAM_ERROR;
                }

                if (bit == 0)
                        current = left_Node(current);
                else
                        current = right_Node(current);

                if (!current) {
                        destroy_BitReader(reader);

                        return DECOMPRESSOR_ARCHIVE_ERROR;
                }

                if (!left_Node(current) && !right_Node(current)) {
                        if (fputc(symbol_Node(current), output) == EOF) {
                                destroy_BitReader(reader);

                                return DECOMPRESSOR_FILE_ERROR;
                        }

                        written++;
                        current = root;
                }
        }

        destroy_BitReader(reader);

        return 0;
}

int decompress(const char* input_filename, const char* output_filename) {    // Decompress using all modules
        if (!input_filename || !output_filename)
                return DECOMPRESSOR_INVALID_ARGUMENT;

        if (strcmp(input_filename, output_filename) == 0)
                return DECOMPRESSOR_INVALID_ARGUMENT;

        FILE* input = fopen(input_filename, "rb");

        if (!input)
                return DECOMPRESSOR_FILE_ERROR;

        FrequencyTable* frequencies = create_FrequencyTable();

        if (!frequencies) {
                fclose(input);

                return DECOMPRESSOR_MEMORY_ERROR;
        }

        unsigned int original_size = 0;
        int result = read_metadata(input, frequencies, &original_size);

        if (result != 0) {
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return result;
        }

        FILE* output = fopen(output_filename, "wb");

        if (!output) {
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return DECOMPRESSOR_FILE_ERROR;
        }

        if (original_size == 0) {
                if (counter_FrequencyTable(frequencies) != 0) {
                        fclose(output);
                        destroy_FrequencyTable(frequencies);
                        fclose(input);

                        return DECOMPRESSOR_ARCHIVE_ERROR;
                }

                if (fclose(output) != 0) {
                        destroy_FrequencyTable(frequencies);
                        fclose(input);

                        return DECOMPRESSOR_FILE_ERROR;
                }

                destroy_FrequencyTable(frequencies);
                fclose(input);

                return 0;
        }

        if (counter_FrequencyTable(frequencies) == 0) {
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return DECOMPRESSOR_ARCHIVE_ERROR;
        }

        Node* root = build_tree(frequencies);

        if (!root) {
                fclose(output);
                destroy_FrequencyTable(frequencies);
                fclose(input);

                return DECOMPRESSOR_MEMORY_ERROR;
        }

        result = write_decompressed_data(input, output, root, original_size);

        if (fclose(output) != 0 && result == 0)
                result = DECOMPRESSOR_FILE_ERROR;

        free_tree(root);
        destroy_FrequencyTable(frequencies);
        fclose(input);

        return result;
}
