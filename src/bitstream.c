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
        if (writer)                                         // Check for non-empty input
                writer->file = file;
}

void set_buffer_BitWriter(BitWriter* writer,
                          unsigned char buffer) {    // Setter for buffer field in BittWriter structure
        if (writer)                                  // Check for non-empty input
                writer->buffer = buffer;
}

void set_bit_counter_BitWriter(BitWriter* writer,
                               unsigned char bit_counter) {    // Setter for bit_counter field in BitWriter structure
        if (writer)                                            // Check for non-empty input
                writer->bit_counter = bit_counter;
}

void set_file_BitReader(BitReader* reader, FILE* file) {    // Setter for file field in BitReader structure
        if (reader)                                         // Check for non-empty input
                reader->file = file;
}

void set_buffer_BitReader(BitReader* reader,
                          unsigned char buffer) {    // Setter for buffer field in BitReader structure
        if (reader)                                  // Check for non-empty input
                reader->buffer = buffer;
}

void set_bit_counter(BitReader* reader,
                     unsigned char bit_counter) {    // Setter for bit_counter field in BitReader structure
        if (reader)                                  // Check for non-empty input
                reader->bit_counter = bit_counter;
}

static void init_bit_writer(BitWriter* writer, FILE* file) {    // Initialize bit writer
        writer->file = file;                                    // Set values for initialized bit writer
        writer->buffer = 0;
        writer->bit_counter = 0;
}

static void init_bit_reader(BitReader* reader, FILE* file) {    // Initialize bit reader
        reader->file = file;                                    // Set values for initialized bit reader
        reader->buffer = 0;
        reader->bit_counter = 0;
}

BitWriter* create_BitWriter(FILE* file) {    // Create bit writer
        if (!file)                           // Check for empty input
                return NULL;

        BitWriter* writer = malloc(sizeof(BitWriter));    // Allocate memory for bit writer

        if (!writer)    // Check for malloc
                return NULL;

        init_bit_writer(writer, file);    // Initialize bit writer

        return writer;
}

BitReader* create_BitReader(FILE* file) {    // Create bit reader
        if (!file)                           // Check for empty input
                return NULL;

        BitReader* reader = malloc(sizeof(BitReader));    // Allocate memory for bit reader

        if (!reader)    // Check for empty input
                return NULL;

        init_bit_reader(reader, file);    // Initialize bit reader

        return reader;
}

void destroy_BitWriter(BitWriter* writer) {    // Destroy bit writer
        free(writer);                          // Free memory for bit writer
}

void destroy_BitReader(BitReader* reader) {    // Destroy bit reader
        free(reader);                          // Free memory for bit reader
}

int write_bit(BitWriter* writer, unsigned char bit) {    // Write data per byte into output file
        if (!writer || !writer->file)                    // Check for empty input
                return BITSTREAM_INVALID_ARGUMENT;

        writer->buffer <<= 1;    // Free Least Significant Bit (младший бит), value 0
        // Bits are stored according to entry order, Most Significant Bit (старший бит) is High-ordered bit (старший
        // разряд) of byte
        if (bit != 0)    // 1 - 1, 0 - 0
                writer->buffer |= 1;

        writer->bit_counter++;    // Increase bit counter

        if (writer->bit_counter == 8) {                            // Number of bits is 8 equalling to byte
                if (fputc(writer->buffer, writer->file) == EOF)    // Write stored data in buffer into output file
                        return BITSTREAM_ERROR;

                writer->buffer = 0;    // Set values as for empty buffer
                writer->bit_counter = 0;
        }

        return 0;
}

int flush_bit_writer(BitWriter* writer) {    // Write remaining bits if less than 8
        if (!writer || !writer->file)        // Check for empty input
                return BITSTREAM_INVALID_ARGUMENT;

        if (writer->bit_counter == 0)    // No bits in buffer
                return 0;

        writer->buffer <<= (8 - writer->bit_counter);    // Move Least Significant Bits in buffer to the left for (8 -
                                                         // bit_counter) bits (Fill with "0" to complete byte)

        if (fputc(writer->buffer, writer->file) == EOF)    // Write stored data in buffer into output file
                return BITSTREAM_ERROR;

        writer->buffer = 0;    // Set values as for empty buffer
        writer->bit_counter = 0;

        return 0;
}

int read_bit(BitReader* reader, unsigned char* bit) {    // Read data per byte from input file
        if (!reader || !reader->file || !bit)            // Check for empty input
                return BITSTREAM_INVALID_ARGUMENT;

        if (reader->bit_counter == 0) {            // Buffer is empty
                int byte = fgetc(reader->file);    // Read data per byte from file

                if (byte == EOF) {                 // Check if reader read EOF
                        if (feof(reader->file))    // Check if reader tried to read after real EOF
                                return BITSTREAM_EOF;

                        return BITSTREAM_ERROR;
                }

                reader->buffer = (unsigned char)byte;    // Set read data from byte variable into buffer variable
                reader->bit_counter = 8;                 // Set bit_counter value to 8
        }

        *bit = (reader->buffer >> 7) & 1;    // Get Most Significant Bit from buffer
        reader->buffer <<= 1;                // Discard read bit, move Least Significant Bits to the left for 1 bit
        reader->bit_counter--;               // Decrease bit counter

        return 0;
}
