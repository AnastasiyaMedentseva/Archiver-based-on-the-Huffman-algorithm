#pragma once

#include "bitstream.h"
#include "frequency.h"
#include "tree.h"

#define DECOMPRESSOR_FILE_ERROR 17
#define DECOMPRESSOR_MEMORY_ERROR 18
#define DECOMPRESSOR_INVALID_ARGUMENT 19
#define DECOMPRESSOR_ARCHIVE_ERROR 20
#define DECOMPRESSOR_BITSTREAM_ERROR 21

int decompress(const char* input_filename, const char* output_filename);
