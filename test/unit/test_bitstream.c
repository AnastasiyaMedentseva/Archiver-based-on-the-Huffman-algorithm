#include "../../src/bitstream/bitstream.h"
#include "test_utils.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <cmocka.h>

static void test_partial_byte_flush(void** state) {
        (void)state;
        FILE* file = tmpfile();

        assert_non_null(file);

        BitWriter writer;
        init_bit_writer(&writer, file);

        assert_int_equal(write_bit(&writer, 1), 0);
        assert_int_equal(write_bit(&writer, 0), 0);
        assert_int_equal(write_bit(&writer, 1), 0);
        assert_int_equal(flush_bit_writer(&writer), 0);

        rewind(file);

        assert_int_equal(fgetc(file), 0xA0);
        assert_int_equal(fgetc(file), EOF);

        fclose(file);
}

static void test_invalid_arguments(void** state) {
        (void)state;
        FILE* file = tmpfile();

        assert_non_null(file);

        BitWriter writer;
        BitReader reader;
        unsigned char bit = 0;
        init_bit_writer(&writer, file);
        init_bit_reader(&reader, file);

        assert_int_equal(write_bit(NULL, 0), BITSTREAM_INVALID_ARGUMENT);
        assert_int_equal(flush_bit_writer(NULL), BITSTREAM_INVALID_ARGUMENT);
        assert_int_equal(read_bit(NULL, &bit), BITSTREAM_INVALID_ARGUMENT);
        assert_int_equal(read_bit(&reader, NULL), BITSTREAM_INVALID_ARGUMENT);

        fclose(file);
}

int main(void) {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_partial_byte_flush),
            cmocka_unit_test(test_invalid_arguments),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
