#pragma once

#include "../frequency_table/frequency.h"
#include "../tree/tree.h"
#include "../code_table/code.h"
#include "../bitstream/bitstream.h"

#define COMPRESSOR_FILE_ERROR 10
#define COMPRESSOR_MEMORY_ERROR 11
#define COMPRESSOR_INVALID_ARGUMENT 12
#define COMPRESSOR_WRITE_ERROR 13
#define COMPRESSOR_BITSTREAM_ERROR 14
#define COMPRESSOR_CODE_NOT_FOUND 15
#define COMPRESSOR_OVERFLOW 16

int compress(const char *input_filename, const char *output_filename);
