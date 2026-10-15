#include "../test_utils.h"
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <cmocka.h>

static void build_from_file(const char* name, FrequencyTable* frequency_table, Node** root, CodeTable* code_table) {
        assert_int_equal(analyze_file(test_data_path(name), frequency_table), 0);

        *root = build_tree(frequency_table);

        assert_non_null(*root);
        assert_int_equal(build_code_table(*root, code_table), 0);
}

static void test_simple_code(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);
        Node* root;
        CodeTable* code_table = create_CodeTable();
        assert_non_null(code_table);
        build_from_file("simple.txt", frequency_table, &root, code_table);

        assert_int_equal(counter_CodeTable(code_table), 3);
        assert_string_equal(find_code(code_table, 'a'), "00");
        assert_string_equal(find_code(code_table, 'b'), "01");
        assert_string_equal(find_code(code_table, 'c'), "1");
        assert_true(is_prefix_free(code_table));

        destroy_CodeTable(code_table);
        free_tree(root);
        destroy_FrequencyTable(frequency_table);
}

static void test_single_symbol_code(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);
        Node* root;
        CodeTable* code_table = create_CodeTable();
        assert_non_null(code_table);
        build_from_file("single.txt", frequency_table, &root, code_table);

        assert_int_equal(counter_CodeTable(code_table), 1);
        assert_string_equal(find_code(code_table, 'a'), "0");

        destroy_CodeTable(code_table);
        free_tree(root);
        destroy_FrequencyTable(frequency_table);
}

static void test_all_symbols_code(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);
        Node* root;
        CodeTable* code_table = create_CodeTable();
        assert_non_null(code_table);
        build_from_file("all_symbols.txt", frequency_table, &root, code_table);

        assert_int_equal(counter_CodeTable(code_table), 94);
        assert_true(is_prefix_free(code_table));

        for (size_t i = 0; i < counter_CodeTable(code_table); ++i)
                assert_true(code_SymbolCode(symbol_code_CodeTable(code_table, i))[0] == '0' ||
                            code_SymbolCode(symbol_code_CodeTable(code_table, i))[0] == '1');

        destroy_CodeTable(code_table);
        free_tree(root);
        destroy_FrequencyTable(frequency_table);
}

static void test_invalid_code_arguments(void** state) {
        (void)state;
        CodeTable* code_table = create_CodeTable();
        assert_non_null(code_table);

        assert_int_equal(build_code_table(NULL, code_table), CODE_INVALID_ARGUMENT);
        assert_int_equal(build_code_table(NULL, NULL), CODE_INVALID_ARGUMENT);

        destroy_CodeTable(code_table);
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
