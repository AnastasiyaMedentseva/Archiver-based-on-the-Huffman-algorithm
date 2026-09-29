#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/compressor/compressor.h"
#include "../../src/decompressor/decompressor.h"
#include "../unit/test_utils.h"

#define TEST_ARCHIVE_FILE "test/test_data/.test_archive.bin"
#define TEST_OUTPUT_FILE "test/test_data/.test_output.bin"

static int files_equal(const char* first_filename, const char* second_filename) {
        FILE* first = fopen(first_filename, "rb");
        FILE* second = fopen(second_filename, "rb");

        if (!first || !second) {
                if (first)
                        fclose(first);

                if (second)
                        fclose(second);

                return 0;
        }

        int equal = 1;
        unsigned char first_buffer[4096];
        unsigned char second_buffer[4096];

        while (1) {
                size_t first_read = fread(first_buffer, 1, sizeof(first_buffer), first);
                size_t second_read = fread(second_buffer, 1, sizeof(second_buffer), second);

                if (first_read != second_read || memcmp(first_buffer, second_buffer, first_read) != 0) {
                        equal = 0;

                        break;
                }

                if (first_read == 0)
                        break;
        }

        if (ferror(first) || ferror(second))
                equal = 0;

        fclose(first);
        fclose(second);

        return equal;
}

static void remove_test_files(void) {
        remove(TEST_ARCHIVE_FILE);
        remove(TEST_OUTPUT_FILE);
}

static void test_compress_and_decompress(const char* filename) {
        const char* input = test_data_path(filename);

        remove_test_files();

        assert_int_equal(compress(input, TEST_ARCHIVE_FILE), 0);
        assert_int_equal(decompress(TEST_ARCHIVE_FILE, TEST_OUTPUT_FILE), 0);
        assert_true(files_equal(input, TEST_OUTPUT_FILE));

        remove_test_files();
}

static void test_text_file_round_trip(void** state) {
        (void)state;
        test_compress_and_decompress("text.txt");
}

static void test_simple_symbols_round_trip(void** state) {
        (void)state;
        test_compress_and_decompress("simple.txt");
}

static void test_single_symbol_round_trip(void** state) {
        (void)state;
        test_compress_and_decompress("single.txt");
}

static void test_russian_text_round_trip(void** state) {
        (void)state;
        test_compress_and_decompress("russian.txt");
}

static void test_empty_file_round_trip(void** state) {
        (void)state;
        test_compress_and_decompress("empty.bin");
}

static void test_invalid_arguments(void** state) {
        (void)state;

        assert_int_equal(compress(NULL, TEST_ARCHIVE_FILE), COMPRESSOR_INVALID_ARGUMENT);
        assert_int_equal(compress(TEST_DATA_DIR "/text.txt", NULL), COMPRESSOR_INVALID_ARGUMENT);
        assert_int_equal(compress(TEST_DATA_DIR "/text.txt", TEST_DATA_DIR "/text.txt"), COMPRESSOR_INVALID_ARGUMENT);
        assert_int_equal(decompress(NULL, TEST_OUTPUT_FILE), DECOMPRESSOR_INVALID_ARGUMENT);
        assert_int_equal(decompress(TEST_ARCHIVE_FILE, NULL), DECOMPRESSOR_INVALID_ARGUMENT);
        assert_int_equal(decompress(TEST_ARCHIVE_FILE, TEST_ARCHIVE_FILE), DECOMPRESSOR_INVALID_ARGUMENT);

        remove_test_files();
}

static int group_setup(void** state) {
        (void)state;
        remove_test_files();

        return 0;
}

static int group_teardown(void** state) {
        (void)state;
        remove_test_files();

        return 0;
}

int main(void) {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_text_file_round_trip),     cmocka_unit_test(test_simple_symbols_round_trip),
            cmocka_unit_test(test_single_symbol_round_trip), cmocka_unit_test(test_russian_text_round_trip),
            cmocka_unit_test(test_empty_file_round_trip),    cmocka_unit_test(test_invalid_arguments),
        };

        return cmocka_run_group_tests(tests, group_setup, group_teardown);
}
