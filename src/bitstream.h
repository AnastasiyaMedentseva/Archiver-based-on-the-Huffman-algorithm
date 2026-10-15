#pragma once

#include <stdio.h>
#include <stdlib.h>

#define BITSTREAM_ERROR 7
#define BITSTREAM_EOF 8
#define BITSTREAM_INVALID_ARGUMENT 9

typedef struct BitWriter BitWriter;

typedef struct BitReader BitReader;

FILE* file_BitWriter(const BitWriter* writer);
unsigned char buffer_BitWriter(const BitWriter* writer);
unsigned char bit_counter_BitWriter(const BitWriter* writer);

FILE* file_BitReader(const BitReader* reader);
unsigned char buffer_BitReader(const BitReader* reader);
unsigned char bit_counter_BitReader(const BitReader* reader);

void set_file_BitWriter(BitWriter* writer, FILE* file);
void set_buffer_BitWriter(BitWriter* writer, unsigned char buffer);
void set_bit_counter_BitWriter(BitWriter* writer, unsigned char bit_counter);

void set_file_BitReader(BitReader* reader, FILE* file);
void set_buffer_BitReader(BitReader* reader, unsigned char buffer);
void set_bit_counter_BitReader(BitReader* reader, unsigned char bit_counter);

BitWriter* create_BitWriter(FILE* file);
void destroy_BitWriter(BitWriter* writer);

BitReader* create_BitReader(FILE* file);
void destroy_BitReader(BitReader* reader);

int write_bit(BitWriter* writer, unsigned char bit);
int flush_bit_writer(BitWriter* writer);
int read_bit(BitReader* reader, unsigned char* bit);
