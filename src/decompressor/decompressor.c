#include "decompressor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
read_metadata(FILE* input, FrequencyTable* frequencies,
              unsigned int* original_size) {    // Read metadata (frequencies, original size) from compressef input file
        unsigned int symbol_counter;

        if (fread(&symbol_counter, sizeof(symbol_counter), 1, input) != 1) {
                printf("Error: could not read symbol count in decompressor\n");

                return DECOMPRESSOR_ARCHIVE_ERROR;
        }

        if (symbol_counter == 0) {
                frequencies->symbols = NULL;
                frequencies->counter = 0;
                frequencies->capacity = 0;
        } else {
                frequencies->symbols = malloc(symbol_counter * sizeof(SymbolFrequency));

                if (!frequencies->symbols) {
                        printf("Error: could not allocate frequency table in decompressor\n");

                        return DECOMPRESSOR_MEMORY_ERROR;
                }

                frequencies->counter = symbol_counter;
                frequencies->capacity = symbol_counter;

                for (size_t i = 0; i < frequencies->counter; i++) {
                        if (fread(&frequencies->symbols[i].symbol, sizeof(frequencies->symbols[i].symbol), 1, input) !=
                            1) {
                                printf("Error: could not read symbol in decompressor\n");

                                return DECOMPRESSOR_ARCHIVE_ERROR;
                        }

                        if (fread(&frequencies->symbols[i].frequency, sizeof(frequencies->symbols[i].frequency), 1,
                                  input) != 1) {
                                printf("Error: could not read frequency in decompressor\n");

                                return DECOMPRESSOR_ARCHIVE_ERROR;
                        }

                        if (frequencies->symbols[i].frequency == 0) {
                                printf("Error: invalid symbol frequency in decompressor\n");

                                return DECOMPRESSOR_ARCHIVE_ERROR;
                        }
                }
        }

        if (fread(original_size, sizeof(*original_size), 1, input) != 1) {
                printf("Error: could not read original size in decompressor\n");

                return DECOMPRESSOR_ARCHIVE_ERROR;
        }

        return 0;
}

static int write_decompressed_data(FILE* input, FILE* output, const Node* root,
                                   unsigned int original_size) {    // Decode file
        if (original_size == 0)
                return 0;

        if (!root)
                return DECOMPRESSOR_ARCHIVE_ERROR;

        BitReader reader;
        init_bit_reader(&reader, input);

        if (!root->left && !root->right) {
                for (unsigned int i = 0; i < original_size; i++) {
                        unsigned char bit;
                        int result = read_bit(&reader, &bit);

                        if (result != 0) {
                                printf("Error: oculd not read compressed data in decompressor\n");

                                return DECOMPRESSOR_BITSTREAM_ERROR;
                        }

                        if (bit != 0) {
                                printf("Error: invalid compressed data in decompressor\n");

                                return DECOMPRESSOR_ARCHIVE_ERROR;
                        }

                        if (fputc(root->symbol, output) == EOF) {
                                printf("Error: could not write output file in decompressor\n");

                                return DECOMPRESSOR_FILE_ERROR;
                        }
                }

                return 0;
        }

        const Node* current = root;
        unsigned int written = 0;

        while (written < original_size) {
                unsigned char bit;
                int result = read_bit(&reader, &bit);

                if (result == BITSTREAM_EOF) {
                        printf("Error: unexpected end of compressed data in decompressor\n");

                        return DECOMPRESSOR_BITSTREAM_ERROR;
                }

                if (result != 0) {
                        printf("Error: could not read compressed data in decompressor\n");

                        return DECOMPRESSOR_BITSTREAM_ERROR;
                }

                if (bit == 0)
                        current = current->left;
                else
                        current = current->right;

                if (!current) {
                        printf("Error: invalid Huffman tree traversal in decompressor\n");

                        return DECOMPRESSOR_ARCHIVE_ERROR;
                }

                if (!current->left && !current->right) {
                        if (fputc(current->symbol, output) == EOF) {
                                printf("Error: could not write output file in decompressor\n");

                                return DECOMPRESSOR_FILE_ERROR;
                        }

                        written++;
                        current = root;
                }
        }

        return 0;
}

int decompress(const char* input_filename, const char* output_filename) {    // Decompress using all modules
        if (!input_filename || !output_filename) {
                printf("Error: invalid argument in decompress\n");

                return DECOMPRESSOR_INVALID_ARGUMENT;
        }

        if (strcmp(input_filename, output_filename) == 0) {
                printf("Error: input and output files must be different\n");

                return DECOMPRESSOR_INVALID_ARGUMENT;
        }

        FILE* input = fopen(input_filename, "rb");

        if (!input) {
                printf("Error: failed to open input file in decompressor\n");

                return DECOMPRESSOR_FILE_ERROR;
        }

        FrequencyTable frequencies;
        init_frequency_table(&frequencies);
        unsigned int original_size = 0;
        int result = read_metadata(input, &frequencies, &original_size);

        if (result != 0) {
                free_frequency_table(&frequencies);
                fclose(input);

                return result;
        }

        FILE* output = fopen(output_filename, "wb");

        if (!output) {
                printf("Error: could not open output file in decompressor\n");
                free_frequency_table(&frequencies);
                fclose(input);

                return DECOMPRESSOR_FILE_ERROR;
        }

        if (original_size == 0) {
                if (frequencies.counter != 0) {
                        printf("Error: invalid archive in decompressor\n");
                        fclose(output);
                        free_frequency_table(&frequencies);
                        fclose(input);

                        return DECOMPRESSOR_ARCHIVE_ERROR;
                }

                if (fclose(output) != 0) {
                        printf("Error: could not close output file in decompressor\n");
                        free_frequency_table(&frequencies);
                        fclose(input);

                        return DECOMPRESSOR_FILE_ERROR;
                }

                free_frequency_table(&frequencies);
                fclose(input);

                return 0;
        }

        if (frequencies.counter == 0) {
                printf("Error: invalid archive in decompressor\n");
                fclose(output);
                free_frequency_table(&frequencies);
                fclose(input);

                return DECOMPRESSOR_ARCHIVE_ERROR;
        }

        Node* root = build_tree(&frequencies);

        if (!root) {
                printf("Error: could not build Huffman tree in decompressor\n");
                fclose(output);
                free_frequency_table(&frequencies);
                fclose(input);

                return DECOMPRESSOR_MEMORY_ERROR;
        }

        result = write_decompressed_data(input, output, root, original_size);

        if (fclose(output) != 0 && result == 0) {
                printf("Error: could not close output file in decompressor\n");

                result = DECOMPRESSOR_FILE_ERROR;
        }

        free_tree(root);
        free_frequency_table(&frequencies);
        fclose(input);

        return result;
}
