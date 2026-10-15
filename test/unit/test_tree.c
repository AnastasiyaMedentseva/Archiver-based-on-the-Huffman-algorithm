#include "../test_utils.h"
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

static size_t count_leaves(const Node* node) {
        if (!node)
                return 0;

        if (!left_Node(node) && !right_Node(node))
                return 1;

        return count_leaves(left_Node(node)) + count_leaves(right_Node(node));
}

static void test_simple_tree(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("simple.txt"), frequency_table), 0);

        Node* root = build_tree(frequency_table);

        assert_non_null(root);
        assert_int_equal(frequency_Node(root), 6);
        assert_int_equal(count_leaves(root), 3);
        assert_non_null(left_Node(root));
        assert_non_null(right_Node(root));

        free_tree(root);
        destroy_FrequencyTable(frequency_table);
}

static void test_single_tree(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("single.txt"), frequency_table), 0);

        Node* root = build_tree(frequency_table);

        assert_non_null(root);
        assert_int_equal(symbol_Node(root), (unsigned char)'a');
        assert_int_equal(frequency_Node(root), 10);
        assert_null(left_Node(root));
        assert_null(right_Node(root));

        free_tree(root);
        destroy_FrequencyTable(frequency_table);
}

static void test_all_symbols_tree(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("all_symbols.txt"), frequency_table), 0);

        Node* root = build_tree(frequency_table);

        assert_non_null(root);
        assert_int_equal(frequency_Node(root), 94);
        assert_int_equal(count_leaves(root), 94);

        free_tree(root);
        destroy_FrequencyTable(frequency_table);
}

static void test_empty_tree(void** state) {
        (void)state;
        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_int_equal(analyze_file(test_data_path("empty.bin"), frequency_table), 0);
        assert_null(build_tree(frequency_table));

        destroy_FrequencyTable(frequency_table);
}

static void test_invalid_tree_arguments(void** state) {
        (void)state;
        assert_null(build_tree(NULL));

        FrequencyTable* frequency_table = create_FrequencyTable();
        assert_non_null(frequency_table);

        assert_null(build_tree(frequency_table));

        destroy_FrequencyTable(frequency_table);
}

int main(void) {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_simple_tree),
            cmocka_unit_test(test_single_tree),
            cmocka_unit_test(test_all_symbols_tree),
            cmocka_unit_test(test_empty_tree),
            cmocka_unit_test(test_invalid_tree_arguments),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
