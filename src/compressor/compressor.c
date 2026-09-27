#include "compressor.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

static int write_metadata(FILE *output, const FrequencyTable *frequencies, unsigned int original_size) {    //Write metadata (frequency table and original size) for following decompression
    unsigned int symbol_counter = (unsigned int)frequencies->counter;

    if (fwrite(&symbol_counter, sizeof(symbol_counter), 1, output) != 1) {
        printf("Error: could not write symbol count in compressor\n");

        return COMPRESSOR_WRITE_ERROR;
    }

    for (size_t i = 0; i < frequencies->counter; i++) {
        unsigned char symbol = frequencies->symbols[i].symbol;
        unsigned int frequency = frequencies->symbols[i].frequency;

        if (fwrite(&symbol, sizeof(symbol), 1, output) != 1) {
            printf("Error: could not write symbol in compressor\n");

            return COMPRESSOR_WRITE_ERROR;
        }

        if (fwrite(&frequency, sizeof(frequency), 1, output) != 1) {
            printf("Error: could not write frequency in compressor\n");

            return COMPRESSOR_WRITE_ERROR;
        }
    }

    if (fwrite(&original_size, sizeof(original_size), 1, output) != 1) {
        printf("Error: could not write original size in compressor\n");

        return COMPRESSOR_WRITE_ERROR;
    }

    return 0;
}

static const char *find_code(const CodeTable *symbol_codes, unsigned char symbol) { //Find code for symbol
    for (size_t i = 0; i < symbol_codes->counter; i++) {
        if (symbol_codes->symbol_codes[i].symbol == symbol)
            return symbol_codes->symbol_codes[i].code;
    }

    return NULL;
}

static int write_compressed_data(FILE *input, FILE *output, const CodeTable *codes) {   //Read input file and code it
    BitWriter writer;
    init_bit_writer(&writer, output);
    int character;

    while ((character = fgetc(input)) != EOF) {
        const char *code = find_code(codes, (unsigned char)character);

        if (!code) {
            printf("Error: could not find code for symbol in compressor\n");

            return COMPRESSOR_CODE_NOT_FOUND;
        }

        for (size_t i = 0; code[i] != '\0'; i++) {
            int result = write_bit(&writer, code[i] == '1');

            if (result != 0) {
                printf("Error: could not write compressed data\n");

                return COMPRESSOR_BITSTREAM_ERROR;
            }
        }
    }

    if (ferror(input)) {
        printf("Error: could not read input file in compressor\n");

        return COMPRESSOR_FILE_ERROR;
    }

    if (flush_bit_writer(&writer) != 0) {
        printf("Error: could not flush bitstream in compressor\n");

        return COMPRESSOR_BITSTREAM_ERROR;
    }

    return 0;
}

int compress(const char *input_filename, const char *output_filename) { //Compress file using all modules
    if (!input_filename || !output_filename) {
        printf("Error: invalid argument in compress\n");

        return COMPRESSOR_INVALID_ARGUMENT;
    }

    if (strcmp(input_filename, output_filename) == 0) {
        printf("Error: input and output files must be different\n");

        return COMPRESSOR_INVALID_ARGUMENT;
    }

    FILE *input = fopen(input_filename, "rb");

    if (!input) {
        printf("Error: could not open input file in compressor\n");

        return COMPRESSOR_FILE_ERROR;
    }

    FrequencyTable frequencies;
    init_frequency_table(&frequencies);
    int result = analyze_file(input_filename, &frequencies);

    if (result != 0) {
        free_frequency_table(&frequencies);
        fclose(input);

        if (result == FREQUENCY_MEMORY_ERROR)
            return COMPRESSOR_MEMORY_ERROR;

        if (result == FREQUENCY_OVERFLOW)
            return COMPRESSOR_OVERFLOW;

        return COMPRESSOR_FILE_ERROR;
    }

    unsigned int original_size = 0;

    for (size_t i = 0; i < frequencies.counter; i++) {
        unsigned int frequency = frequencies.symbols[i].frequency;

        if (UINT_MAX - original_size < frequency) {
            printf("Error: original size overflow in compressor\n");
            free_frequency_table(&frequencies);
            fclose(input);

            return COMPRESSOR_OVERFLOW;
        }

        original_size += frequency;
    }

    FILE *output = fopen(output_filename, "wb");

    if (!output) {
        printf("Error: failed to open output file\n");
        free_frequency_table(&frequencies);
        fclose(input);

        return COMPRESSOR_FILE_ERROR;
    }

    result = write_metadata(output, &frequencies, original_size);

    if (result != 0) {
        fclose(output);
        free_frequency_table(&frequencies);
        fclose(input);

        return result;
    }

    if (frequencies.counter == 0) {
        fclose(output);
        free_frequency_table(&frequencies);
        fclose(input);

        return 0;
    }

    Node *root = build_tree(&frequencies);

    if (!root) {
        printf("Error: could not build Huffman tree\n");
        fclose(output);
        free_frequency_table(&frequencies);
        fclose(input);

        return COMPRESSOR_MEMORY_ERROR;
    }

    CodeTable codes;
    init_code_table(&codes);
    result = build_code_table(root, &codes);

    if (result != 0) {
        free_code_table(&codes);
        free_tree(root);
        fclose(output);
        free_frequency_table(&frequencies);
        fclose(input);

        if (result == CODE_MEMORY_ERROR)
            return COMPRESSOR_MEMORY_ERROR;

        return COMPRESSOR_INVALID_ARGUMENT;
    }

    if (fseek(input, 0, SEEK_SET) != 0) {
        printf("Error: could not rewind input file in compressor\n");
        free_code_table(&codes);
        free_tree(root);
        fclose(output);
        free_frequency_table(&frequencies);
        fclose(input);

        return COMPRESSOR_FILE_ERROR;
    }

    result = write_compressed_data(input, output, &codes);

    if (fclose(output) != 0 && result == 0) {
        printf("Error: could not close output file\n");
        result = COMPRESSOR_FILE_ERROR;
    }

    free_code_table(&codes);
    free_tree(root);
    free_frequency_table(&frequencies);
    fclose(input);

    return result;
}
