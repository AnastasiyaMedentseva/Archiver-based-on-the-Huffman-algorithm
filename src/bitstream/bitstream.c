#include "bitstream.h"
#include <stdio.h>

void init_bit_writer(BitWriter* writer, FILE* file) {    // Initialize bit writer
        writer->file = file;
        writer->buffer = 0;
        writer->bit_counter = 0;
}

int write_bit(BitWriter* writer, unsigned char bit) {    // Write data per byte into output file
        if (!writer || !writer->file) {
                printf("Error: invalid argument in write_bit\n");

                return BITSTREAM_INVALID_ARGUMENT;
        }

        writer->buffer <<= 1;

        if (bit != 0)
                writer->buffer |= 1;

        writer->bit_counter++;

        if (writer->bit_counter == 8) {
                if (fputc(writer->buffer, writer->file) == EOF) {
                        printf("Error: could not write bitstream\n");

                        return BITSTREAM_ERROR;
                }

                writer->buffer = 0;
                writer->bit_counter = 0;
        }

        return 0;
}

int flush_bit_writer(BitWriter* writer) {    // Write remaining bits if less than 8
        if (!writer || !writer->file) {
                printf("Error: invalid argument in flush_bit_writer\n");

                return BITSTREAM_INVALID_ARGUMENT;
        }

        if (writer->bit_counter == 0)
                return 0;

        writer->buffer <<= 8 - writer->bit_counter;

        if (fputc(writer->buffer, writer->file) == EOF) {
                printf("Error: could not flush bitstream\n");

                return BITSTREAM_ERROR;
        }

        writer->buffer = 0;
        writer->bit_counter = 0;

        return 0;
}

void init_bit_reader(BitReader* reader, FILE* file) {    // Initialize bit reader
        reader->file = file;
        reader->buffer = 0;
        reader->bit_counter = 0;
}

int read_bit(BitReader* reader, unsigned char* bit) {    // Read data per byte from input file
        if (!reader || !reader->file || !bit) {
                printf("Error: invalid argument in read_bit\n");

                return BITSTREAM_INVALID_ARGUMENT;
        }

        if (reader->bit_counter == 0) {
                int byte = fgetc(reader->file);

                if (byte == EOF) {
                        if (feof(reader->file))
                                return BITSTREAM_EOF;

                        printf("Error: could not read bitstream\n");

                        return BITSTREAM_ERROR;
                }

                reader->buffer = (unsigned char)byte;
                reader->bit_counter = 8;
        }

        *bit = (reader->buffer >> 7) & 1;
        reader->buffer <<= 1;
        reader->bit_counter--;

        return 0;
}
