#include "decompressor.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_metadata(FILE* input, FrequencyTable* frequencies, unsigned int* original_size) {
        // Read metadata (frequencies, original size) from compressed input file
        unsigned int symbol_counter;    // Variable for number of symbols in frequency table

        if (fread(&symbol_counter, sizeof(symbol_counter), 1, input) != 1)    // Read number of symbols, check for fread
                return DECOMPRESSOR_ARCHIVE_ERROR;

        if (reserve_FrequencyTable(frequencies, symbol_counter) !=
            0)    // Allocate space for symbols, check for reserve_FrequencyTable function
                return DECOMPRESSOR_MEMORY_ERROR;

        set_counter_FrequencyTable(frequencies, symbol_counter);    // Set number of symbols in frequency table

        for (size_t i = 0; i < symbol_counter; i++) {    // In range of number of symbols in frequency table
                unsigned char symbol;                    // Variable for symbol
                unsigned int frequency;                  // Variable for frequency of symbol

                if (fread(&symbol, sizeof(symbol), 1, input) != 1)    // Read symbol, check for fread
                        return DECOMPRESSOR_ARCHIVE_ERROR;

                if (fread(&frequency, sizeof(frequency), 1, input) != 1)    // Read frequency of symbol, check for fread
                        return DECOMPRESSOR_ARCHIVE_ERROR;

                if (frequency == 0)    // Frequency of symbol cannot be zero
                        return DECOMPRESSOR_ARCHIVE_ERROR;

                set_symbol_frequency_FrequencyTable(
                    frequencies, i, symbol, frequency);    // Store symbol and its frequency into frequency table
        }

        if (fread(original_size, sizeof(*original_size), 1, input) !=
            1)    // Read original size of input file, check for fread
                return DECOMPRESSOR_ARCHIVE_ERROR;

        return 0;
}

static int write_decompressed_data(FILE* input, FILE* output, const Node* root,
                                   unsigned int original_size) {    // Decode file
        if (original_size == 0)                                     // Empty file, nothing to decode
                return 0;

        if (!root)    // Check for empty Huffman tree
                return DECOMPRESSOR_ARCHIVE_ERROR;

        BitReader* reader = create_BitReader(input);    // Create bit reader

        if (!reader)    // Check for create_BitReader function
                return DECOMPRESSOR_MEMORY_ERROR;

        if (!left_Node(root) && !right_Node(root)) {                  // Tree has only one node (single symbol in file)
                for (unsigned int i = 0; i < original_size; i++) {    // In range of original size
                        unsigned char bit;                            // Variable for bit

                        if (read_bit(reader, &bit) != 0) {    // Read bit, check for read_bit function
                                destroy_BitReader(reader);    // Free memory for bit reader

                                return DECOMPRESSOR_BITSTREAM_ERROR;
                        }

                        if (bit != 0) {                       // Single-node tree expects only zero bits
                                destroy_BitReader(reader);    // Free memory for bit reader

                                return DECOMPRESSOR_ARCHIVE_ERROR;
                        }

                        if (fputc(symbol_Node(root), output) ==
                            EOF) {                            // Write symbol into output file, check for fputc
                                destroy_BitReader(reader);    // Free memory for bit reader

                                return DECOMPRESSOR_FILE_ERROR;
                        }
                }

                destroy_BitReader(reader);    // Free memory for bit reader

                return 0;
        }

        const Node* current = root;    // Current node for traversing Huffman tree
        unsigned int written = 0;      // Variable for number of written symbols

        while (written < original_size) {    // While not all symbols are written
                unsigned char bit;           // Variable for bit

                if (read_bit(reader, &bit) != 0) {    // Read bit, check for read_bit function
                        destroy_BitReader(reader);    // Free memory for bit reader

                        return DECOMPRESSOR_BITSTREAM_ERROR;
                }

                if (bit == 0)    // Move to left child for bit 0
                        current = left_Node(current);
                else
                        current = right_Node(current);    // Move to right child for bit 1

                if (!current) {                       // Traversal fell off the tree
                        destroy_BitReader(reader);    // Free memory for bit reader

                        return DECOMPRESSOR_ARCHIVE_ERROR;
                }

                if (!left_Node(current) && !right_Node(current)) {    // Reached leaf node, symbol decoded
                        if (fputc(symbol_Node(current), output) ==
                            EOF) {                            // Write symbol into output file, check for fputc
                                destroy_BitReader(reader);    // Free memory for bit reader

                                return DECOMPRESSOR_FILE_ERROR;
                        }

                        written++;         // Increase number of written symbols
                        current = root;    // Restart traversal from tree root
                }
        }

        destroy_BitReader(reader);    // Free memory for bit reader

        return 0;
}

int decompress(const char* input_filename, const char* output_filename) {    // Decompress using all modules
        if (!input_filename || !output_filename)                             // Check for empty input
                return DECOMPRESSOR_INVALID_ARGUMENT;

        if (strcmp(input_filename, output_filename) == 0)    // Compare input file with output file, check strcmp
                return DECOMPRESSOR_INVALID_ARGUMENT;

        FILE* input = fopen(input_filename, "rb");    // Open input file for reading in binary format

        if (!input)    // Check for fopen
                return DECOMPRESSOR_FILE_ERROR;

        FrequencyTable* frequencies = create_FrequencyTable();    // Create frequency table

        if (!frequencies) {       // Check for create_FrequencyTable function
                fclose(input);    // Close input file

                return DECOMPRESSOR_MEMORY_ERROR;
        }

        unsigned int original_size = 0;    // Variable for original size of decompressed file
        int result = read_metadata(input, frequencies, &original_size);    // Read metadata from input file

        if (result != 0) {                              // Metadata read failed
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return result;
        }

        FILE* output = fopen(output_filename, "wb");    // Open output file for writing in binary format

        if (!output) {                                  // Check for fopen
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return DECOMPRESSOR_FILE_ERROR;
        }

        if (original_size == 0) {                                  // Empty original file, no data to decompress
                if (counter_FrequencyTable(frequencies) != 0) {    // Frequency table must be empty for empty file
                        fclose(output);                            // Close output file
                        destroy_FrequencyTable(frequencies);       // Free memory for frequency table
                        fclose(input);                             // Close input file

                        return DECOMPRESSOR_ARCHIVE_ERROR;
                }

                if (fclose(output) != 0) {                      // Close output file, check for fclose
                        destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                        fclose(input);                          // Close input file

                        return DECOMPRESSOR_FILE_ERROR;
                }

                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return 0;    // Empty file decompressed successfully
        }

        if (counter_FrequencyTable(frequencies) == 0) {    // Non-empty file must have non-empty frequency table
                fclose(output);                            // Close output file
                destroy_FrequencyTable(frequencies);       // Free memory for frequency table
                fclose(input);                             // Close input file

                return DECOMPRESSOR_ARCHIVE_ERROR;
        }

        Node* root = build_tree(frequencies);    // Build Huffman tree from frequencies

        if (!root) {                                    // Check for build_tree function
                fclose(output);                         // Close output file
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return DECOMPRESSOR_MEMORY_ERROR;
        }

        result =
            write_decompressed_data(input, output, root, original_size);    // Decode compressed data into output file

        if (fclose(output) != 0 && result == 0)    // Close output file, check for fclose
                result = DECOMPRESSOR_FILE_ERROR;

        free_tree(root);                        // Free memory for Huffman tree
        destroy_FrequencyTable(frequencies);    // Free memory for frequency table
        fclose(input);                          // Close input file

        return result;
}
