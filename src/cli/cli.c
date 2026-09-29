#include "cli.h"
#include "../compressor/compressor.h"
#include "../decompressor/decompressor.h"
#include <stdio.h>
#include <string.h>

static void print_usage(const char* program_name) {    // Show elements in usage (auxiliary function)
        printf("Usage:\n"
               "  %s compress <input> <output>\n"
               "  %s decompress <input> <output>\n",
               program_name, program_name);
}

int run_cli(int argc, char** argv) {    // Execute command
        if (argc != 4) {
                printf("Error: invalid number of arguments\n");
                print_usage(argv[0]);

                return CLI_INVALID_ARGUMENT;
        }

        if (strcmp(argv[1], "compress") == 0)
                return compress(argv[2], argv[3]);

        if (strcmp(argv[1], "decompress") == 0)
                return decompress(argv[2], argv[3]);

        printf("Error: unknown command '%s'\n", argv[1]);
        print_usage(argv[0]);

        return CLI_UNKNOWN_COMMAND;
}
