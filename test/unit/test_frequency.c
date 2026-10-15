#include "../test_utils.h"
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <cmocka.h>

static void test_simple_file(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("simple.txt"), frequency_table), 0);
        assert_int_equal(counter_FrequencyTable(frequency_table), 3);
        assert_int_equal(find_frequency(frequency_table, 'a'), 1);
        assert_int_equal(find_frequency(frequency_table, 'b'), 2);
        assert_int_equal(find_frequency(frequency_table, 'c'), 3);

        destroy_FrequencyTable(frequency_table);
}

static void test_single_symbol_file(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("single.txt"), frequency_table), 0);
        assert_int_equal(counter_FrequencyTable(frequency_table), 1);
        assert_int_equal(find_frequency(frequency_table, 'a'), 10);

        destroy_FrequencyTable(frequency_table);
}

static void test_text_file(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("text.txt"), frequency_table), 0);
        assert_int_equal(counter_FrequencyTable(frequency_table), 23);
        assert_int_equal(find_frequency(frequency_table, 'H'), 1);
        assert_int_equal(find_frequency(frequency_table, 'a'), 9);
        assert_int_equal(find_frequency(frequency_table, 'e'), 18);
        assert_int_equal(find_frequency(frequency_table, 'y'), 2);
        assert_int_equal(find_frequency(frequency_table, ' '), 25);

        destroy_FrequencyTable(frequency_table);
}

static void test_all_symbols_file(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("all_symbols.txt"), frequency_table), 0);
        assert_int_equal(counter_FrequencyTable(frequency_table), 94);

        for (size_t i = 0; i < counter_FrequencyTable(frequency_table); ++i)
                assert_int_equal(frequency_SymbolFrequency(symbol_frequency_FrequencyTable(frequency_table, i)), 1);

        destroy_FrequencyTable(frequency_table);
}

static void test_empty_file(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("empty.bin"), frequency_table), 0);
        assert_int_equal(counter_FrequencyTable(frequency_table), 0);

        destroy_FrequencyTable(frequency_table);
}

static void test_invalid_arguments(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(NULL, frequency_table), FREQUENCY_INVALID_ARGUMENT);
        assert_int_equal(analyze_file(test_data_path("text.txt"), NULL), FREQUENCY_INVALID_ARGUMENT);
        assert_int_equal(analyze_file("test/test_data/no-such-file.bin", frequency_table), FREQUENCY_FILE_ERROR);

        destroy_FrequencyTable(frequency_table);
}

int main(void) {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_simple_file), cmocka_unit_test(test_single_symbol_file),
            cmocka_unit_test(test_text_file),   cmocka_unit_test(test_all_symbols_file),
            cmocka_unit_test(test_empty_file),  cmocka_unit_test(test_invalid_arguments),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
