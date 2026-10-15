#include "bitstream.h"
#include <stdio.h>
#include <stdlib.h>

struct BitWriter {    // Structure for bit writer
        FILE* file;
        unsigned char buffer;
        unsigned char bit_counter;
};

struct BitReader {    // Structure for bit reader
        FILE* file;
        unsigned char buffer;
        unsigned char bit_counter;
};

FILE* file_BitWriter(const BitWriter* writer) {    // Access to file field in BitWriter structure
        return writer->file;
}

unsigned char buffer_BitWriter(const BitWriter* writer) {    // Access to buffer field in BitWriter structure
        return writer->buffer;
}

unsigned char bit_counter_BitWriter(const BitWriter* writer) {    // Access to bit_counter field in BitWriter structure
        return writer->bit_counter;
}

FILE* file_BitReader(const BitReader* reader) {    // Access to file field in BitReader structure
        return reader->file;
}

unsigned char buffer_BitReader(const BitReader* reader) {    // Access to buffer field in BitReader structure
        return reader->buffer;
}

unsigned char bit_counter_BitReader(const BitReader* reader) {    // Access to bit_cointer field in BitReader structure
        return reader->bit_counter;
}

void set_file_BitWriter(BitWriter* writer, FILE* file) {    // Setter for file field in BitWriter structure
        if (writer)
                writer->file = file;
}

void set_buffer_BitWriter(BitWriter* writer,
                          unsigned char buffer) {    // Setter for buffer field in BittWriter structure
        if (writer)
                writer->buffer = buffer;
}

void set_bit_counter_BitWriter(BitWriter* writer,
                               unsigned char bit_counter) {    // Setter for bit_counter field in BitWriter structure
        if (writer)
                writer->bit_counter = bit_counter;
}

void set_file_BitReader(BitReader* reader, FILE* file) {    // Setter for file field in BitReader structure
        if (reader)
                reader->file = file;
}

void set_buffer_BitReader(BitReader* reader,
                          unsigned char buffer) {    // Setter for buffer field in BitReader structure
        if (reader)
                reader->buffer = buffer;
}

void set_bit_counter(BitReader* reader,
                     unsigned char bit_counter) {    // Setter for bit_counter field in BitReader structure
        if (reader)
                reader->bit_counter = bit_counter;
}

static void init_bit_writer(BitWriter* writer, FILE* file) {    // Initialize bit writer
        writer->file = file;
        writer->buffer = 0;
        writer->bit_counter = 0;
}

static void init_bit_reader(BitReader* reader, FILE* file) {    // Initialize bit reader
        reader->file = file;
        reader->buffer = 0;
        reader->bit_counter = 0;
}

BitWriter* create_BitWriter(FILE* file) {    // Create bit writer
        if (!file)
                return NULL;

        BitWriter* writer = malloc(sizeof(BitWriter));

        if (!writer)
                return NULL;

        init_bit_writer(writer, file);

        return writer;
}

BitReader* create_BitReader(FILE* file) {    // Create bit reader
        if (!file)
                return NULL;

        BitReader* reader = malloc(sizeof(BitReader));

        if (!reader)
                return NULL;

        init_bit_reader(reader, file);

        return reader;
}

void destroy_BitWriter(BitWriter* writer) {    // Destroy bit writer
        free(writer);
}

void destroy_BitReader(BitReader* reader) {    // Destroy bit reader
        free(reader);
}

int write_bit(BitWriter* writer, unsigned char bit) {    // Write data per byte into output file
        if (!writer || !writer->file)
                return BITSTREAM_INVALID_ARGUMENT;

        writer->buffer <<= 1;

        if (bit != 0)
                writer->buffer |= 1;

        writer->bit_counter++;

        if (writer->bit_counter == 8) {
                if (fputc(writer->buffer, writer->file) == EOF)
                        return BITSTREAM_ERROR;

                writer->buffer = 0;
                writer->bit_counter = 0;
        }

        return 0;
}

int flush_bit_writer(BitWriter* writer) {    // Write remaining bits if less than 8
        if (!writer || !writer->file)
                return BITSTREAM_INVALID_ARGUMENT;

        if (writer->bit_counter == 0)
                return 0;

        writer->buffer <<= 8 - writer->bit_counter;

        if (fputc(writer->buffer, writer->file) == EOF)
                return BITSTREAM_ERROR;

        writer->buffer = 0;
        writer->bit_counter = 0;

        return 0;
}

int read_bit(BitReader* reader, unsigned char* bit) {    // Read data per byte from input file
        if (!reader || !reader->file || !bit)
                return BITSTREAM_INVALID_ARGUMENT;

        if (reader->bit_counter == 0) {
                int byte = fgetc(reader->file);

                if (byte == EOF) {
                        if (feof(reader->file))
                                return BITSTREAM_EOF;

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
