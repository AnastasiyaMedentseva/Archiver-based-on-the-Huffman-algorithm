#include "test_utils.h"
#include <cmocka.h>
#include <string.h>

static void test_simple_file(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("simple.txt"), &frequency_table), 0);
        assert_int_equal(frequency_table.counter, 3);
        assert_int_equal(find_frequency(&frequency_table, 'a')->frequency, 1);
        assert_int_equal(find_frequency(&frequency_table, 'b')->frequency, 2);
        assert_int_equal(find_frequency(&frequency_table, 'c')->frequency, 3);

        free_frequency_table(&frequency_table);
}

static void test_single_symbol_file(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("single.txt"), &frequency_table), 0);
        assert_int_equal(frequency_table.counter, 1);
        assert_int_equal(find_frequency(&frequency_table, 'a')->frequency, 10);

        free_frequency_table(&frequency_table);
}

static void test_text_file(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("text.txt"), &frequency_table), 0);
        assert_int_equal(frequency_table.counter, 23);
        assert_int_equal(find_frequency(&frequency_table, 'H')->frequency, 1);
        assert_int_equal(find_frequency(&frequency_table, 'a')->frequency, 9);
        assert_int_equal(find_frequency(&frequency_table, 'e')->frequency, 18);
        assert_int_equal(find_frequency(&frequency_table, 'y')->frequency, 2);
        assert_int_equal(find_frequency(&frequency_table, ' ')->frequency, 25);

        free_frequency_table(&frequency_table);
}

static void test_all_symbols_file(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("all_symbols.txt"), &frequency_table), 0);
        assert_int_equal(frequency_table.counter, 94);

        for (size_t i = 0; i < frequency_table.counter; ++i)
                assert_int_equal(frequency_table.symbols[i].frequency, 1);

        free_frequency_table(&frequency_table);
}

static void test_empty_file(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("empty.bin"), &frequency_table), 0);
        assert_int_equal(frequency_table.counter, 0);

        free_frequency_table(&frequency_table);
}

static void test_invalid_arguments(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(NULL, &frequency_table), FREQUENCY_INVALID_ARGUMENT);
        assert_int_equal(analyze_file(test_data_path("text.txt"), NULL), FREQUENCY_INVALID_ARGUMENT);
        assert_int_equal(analyze_file("test/test_data/no-such-file.bin", &frequency_table), FREQUENCY_FILE_ERROR);

        free_frequency_table(&frequency_table);
}

int main(void) {
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_simple_file),
                cmocka_unit_test(test_single_symbol_file),
                cmocka_unit_test(test_text_file),
                cmocka_unit_test(test_all_symbols_file),
                cmocka_unit_test(test_empty_file),
                cmocka_unit_test(test_invalid_arguments),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
