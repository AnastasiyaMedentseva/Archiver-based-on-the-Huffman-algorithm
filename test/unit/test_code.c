#include "test_utils.h"
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <cmocka.h>

static void build_from_file(const char* name, FrequencyTable* frequency_table, Node** root, CodeTable* code_table) {
        init_frequency_table(frequency_table);
        init_code_table(code_table);

        assert_int_equal(analyze_file(test_data_path(name), frequency_table), 0);

        *root = build_tree(frequency_table);

        assert_non_null(*root);
        assert_int_equal(build_code_table(*root, code_table), 0);
}

static void test_simple_code(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        Node* root;
        CodeTable code_table;
        build_from_file("simple.txt", &frequency_table, &root, &code_table);

        assert_int_equal(code_table.counter, 3);
        assert_string_equal(find_code(&code_table, 'a')->code, "00");
        assert_string_equal(find_code(&code_table, 'b')->code, "01");
        assert_string_equal(find_code(&code_table, 'c')->code, "1");
        assert_true(is_prefix_free(&code_table));

        free_code_table(&code_table);
        free_tree(root);
        free_frequency_table(&frequency_table);
}

static void test_single_symbol_code(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        Node* root;
        CodeTable code_table;
        build_from_file("single.txt", &frequency_table, &root, &code_table);

        assert_int_equal(code_table.counter, 1);
        assert_string_equal(find_code(&code_table, 'a')->code, "0");

        free_code_table(&code_table);
        free_tree(root);
        free_frequency_table(&frequency_table);
}

static void test_all_symbols_code(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        Node* root;
        CodeTable code_table;
        build_from_file("all_symbols.txt", &frequency_table, &root, &code_table);

        assert_int_equal(code_table.counter, 94);
        assert_true(is_prefix_free(&code_table));

        for (size_t i = 0; i < code_table.counter; ++i)
                assert_true(code_table.symbol_codes[i].code[0] == '0' || code_table.symbol_codes[i].code[0] == '1');

        free_code_table(&code_table);
        free_tree(root);
        free_frequency_table(&frequency_table);
}

static void test_invalid_code_arguments(void** state) {
        (void)state;
        CodeTable code_table;
        init_code_table(&code_table);

        assert_int_equal(build_code_table(NULL, &code_table), CODE_INVALID_ARGUMENT);
        assert_int_equal(build_code_table(NULL, NULL), CODE_INVALID_ARGUMENT);

        free_code_table(&code_table);
}

int main(void) {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_simple_code),
            cmocka_unit_test(test_single_symbol_code),
            cmocka_unit_test(test_all_symbols_code),
            cmocka_unit_test(test_invalid_code_arguments),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
