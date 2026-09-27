#pragma once

#include <stdio.h>

#define BITSTREAM_ERROR 7
#define BITSTREAM_EOF 8
#define BITSTREAM_INVALID_ARGUMENT 9

typedef struct {    //Structure for bit writer
    FILE *file;
    unsigned char buffer;
    unsigned char bit_counter;
} BitWriter;

typedef struct {    //Structure for bit reader
    FILE *file;
    unsigned char buffer;
    unsigned char bit_counter;
} BitReader;

void init_bit_writer(BitWriter *writer, FILE *file);
int write_bit(BitWriter *writer, unsigned char bit);
int flush_bit_writer(BitWriter *writer);
void init_bit_reader(BitReader *reader, FILE *file);
int read_bit(BitReader *reader, unsigned char *bit);
