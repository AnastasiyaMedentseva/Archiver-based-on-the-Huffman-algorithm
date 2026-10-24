#include "compressor.h"
#include <stddef.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static int write_metadata(
    FILE* output, const FrequencyTable* frequencies,
    unsigned int original_size) {    // Write metadata (frequency table and original size) for following decompression
        size_t frequencies_counter =
            counter_FrequencyTable(frequencies);    // Variable for number of elements in frequency table
        unsigned int symbol_counter =
            (unsigned int)frequencies_counter;    // Variable for symbol counter of frequency table

        if (fwrite(&symbol_counter, sizeof(symbol_counter), 1, output) !=
            1)    // Write number of symbols in frequency table into output file, check for fwrite
                return COMPRESSOR_WRITE_ERROR;

        for (size_t i = 0; i < frequencies_counter; i++) {    // In range of number of symbols in frequency table
                unsigned char symbol = symbol_SymbolFrequency(
                    symbol_frequency_FrequencyTable(frequencies, i));    // Get each symbol to write it into output file
                unsigned int frequency = frequency_SymbolFrequency(symbol_frequency_FrequencyTable(
                    frequencies, i));    // Get frequency of each symbol to write it into output file

                if (fwrite(&symbol, sizeof(symbol), 1, output) != 1)    // Write symbol, check for fwrite
                        return COMPRESSOR_WRITE_ERROR;

                if (fwrite(&frequency, sizeof(frequency), 1, output) !=
                    1)    // Write frequency of symbol, check for fwrite
                        return COMPRESSOR_WRITE_ERROR;
        }

        if (fwrite(&original_size, sizeof(original_size), 1, output) !=
            1)    // Write original size of input file, check for fwrite
                return COMPRESSOR_WRITE_ERROR;

        return 0;
}

static const char* find_code(const CodeTable* symbol_codes, unsigned char symbol) {    // Find code for symbol
        size_t symbol_codes_counter =
            counter_CodeTable(symbol_codes);    // Variable for number of elements in code table

        for (size_t i = 0; i < symbol_codes_counter; i++) {    // In range of number of elements in code table
                const SymbolCode* symbol_code =
                    symbol_code_CodeTable(symbol_codes, i);    // Get element from array of symbols with codes

                if (symbol_SymbolCode(symbol_code) == symbol)    // Symbol from code table is searched symbol
                        return code_SymbolCode(symbol_code);     // Return code of searched symbol
        }

        return NULL;
}

static int write_compressed_data(FILE* input, FILE* output, const CodeTable* codes) {    // Read input file and code it
        BitWriter* writer = create_BitWriter(output);                                    // Create bit writer

        if (!writer)    // Check for create_BitWriter function
                return COMPRESSOR_MEMORY_ERROR;

        int character;    // Variable for symbol

        while ((character = fgetc(input)) != EOF) {                               // Get data per bytes from input file
                const char* code = find_code(codes, (unsigned char)character);    // Get code of symbol

                if (!code) {                          // Code of symbol not found
                        destroy_BitWriter(writer);    // Free memory for bit writer

                        return COMPRESSOR_CODE_NOT_FOUND;
                }

                for (size_t i = 0; code[i] != '\0'; i++) {               // In range of code length
                        if (write_bit(writer, code[i] == '1') != 0) {    // Write bits, check for write_bit function
                                destroy_BitWriter(writer);               // Free memory for bit writer

                                return COMPRESSOR_BITSTREAM_ERROR;
                        }
                }
        }

        if (ferror(input)) {                  // Check for input file stream error
                destroy_BitWriter(writer);    // Free memory for bit writer

                return COMPRESSOR_FILE_ERROR;
        }

        if (flush_bit_writer(writer) != 0) {    // Check for flush_bit_writer function
                destroy_BitWriter(writer);      // Free memory for bit writer

                return COMPRESSOR_BITSTREAM_ERROR;
        }

        destroy_BitWriter(writer);    // Free memory for bit writer

        return 0;
}

int compress(const char* input_filename, const char* output_filename) {    // Compress file using all modules
        if (!input_filename || !output_filename)                           // Check for empty input
                return COMPRESSOR_INVALID_ARGUMENT;

        if (strcmp(input_filename, output_filename) == 0)    // Compare input file with output file, check strcmp
                return COMPRESSOR_INVALID_ARGUMENT;

        FILE* input = fopen(input_filename, "rb");    // Open input file for reading in binary format

        if (!input)    // Check for fopen
                return COMPRESSOR_FILE_ERROR;

        FrequencyTable* frequencies = create_FrequencyTable();    // Create frequency table

        if (!frequencies) {       // Check for create_FrequencyTable function
                fclose(input);    // Close input file

                return COMPRESSOR_MEMORY_ERROR;
        }

        int result = analyze_file(input_filename, frequencies);    // Analyze input file and fill frequency table

        if (result != 0) {                              // Analysis failed
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                if (result == FREQUENCY_MEMORY_ERROR)    // Map frequency module errors to compressor errors
                        return COMPRESSOR_MEMORY_ERROR;

                if (result == FREQUENCY_OVERFLOW)
                        return COMPRESSOR_OVERFLOW;

                return COMPRESSOR_FILE_ERROR;
        }

        unsigned int original_size = 0;    // Total size of original file, accumulated from frequencies

        for (size_t i = 0; i < counter_FrequencyTable(frequencies);
             i++) {    // In range of number of elements in frequency table
                unsigned int frequency = frequency_SymbolFrequency(
                    symbol_frequency_FrequencyTable(frequencies, i));    // Get frequency of each symbol

                if (UINT_MAX - original_size < frequency) {     // Check for overflow
                        destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                        fclose(input);                          // Close input file

                        return COMPRESSOR_OVERFLOW;
                }

                original_size += frequency;    // Increase size by summing frequencies of symbols
        }

        FILE* output = fopen(output_filename, "wb");    // Open output file for writing in binary format

        if (!output) {                                  // Check for fopen
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return COMPRESSOR_FILE_ERROR;
        }

        result =
            write_metadata(output, frequencies, original_size);    // Write frequency table and original size to output

        if (result != 0) {                              // Metadata write failed
                fclose(output);                         // Close output file
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return result;
        }

        if (counter_FrequencyTable(frequencies) == 0) {    // Empty input file, nothing to encode
                fclose(output);                            // Close output file
                destroy_FrequencyTable(frequencies);       // Free memory for frequency table
                fclose(input);                             // Close input file

                return 0;    // Empty file compressed successfully (metadata only)
        }

        Node* root = build_tree(frequencies);    // Build Huffman tree from frequencies

        if (!root) {                                    // Check for build_tree function
                fclose(output);                         // Close output file
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return COMPRESSOR_MEMORY_ERROR;
        }

        CodeTable* codes = create_CodeTable();    // Create code table

        if (!codes) {                                   // Check for create_CodeTable function
                free_tree(root);                        // Free memory for Huffman tree
                fclose(output);                         // Close output file
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return COMPRESSOR_MEMORY_ERROR;
        }

        result = build_code_table(root, codes);    // Fill code table by traversing Huffman tree

        if (result != 0) {                              // Code table build failed
                destroy_CodeTable(codes);               // Free memory for code table
                free_tree(root);                        // Free memory for Huffman tree
                fclose(output);                         // Close output file
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                if (result == CODE_MEMORY_ERROR)    // Map code table module errors to compressor errors
                        return COMPRESSOR_MEMORY_ERROR;

                return COMPRESSOR_INVALID_ARGUMENT;
        }

        if (fseek(input, 0, SEEK_SET) != 0) {           // Rewind input file to beginning for second pass
                destroy_CodeTable(codes);               // Free memory for code table
                free_tree(root);                        // Free memory for Huffman tree
                fclose(output);                         // Close output file
                destroy_FrequencyTable(frequencies);    // Free memory for frequency table
                fclose(input);                          // Close input file

                return COMPRESSOR_FILE_ERROR;
        }

        result =
            write_compressed_data(input, output, codes);    // Read input file again and write encoded bits into output

        if (fclose(output) != 0 && result == 0)    // Close output file, check for fclose
                result = COMPRESSOR_FILE_ERROR;

        destroy_CodeTable(codes);               // Free memory for code table
        free_tree(root);                        // Free memory for Huffman tree
        destroy_FrequencyTable(frequencies);    // Free memory for frequency table
        fclose(input);                          // Close input file

        return result;
}
