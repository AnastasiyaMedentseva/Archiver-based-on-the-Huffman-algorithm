#include "test_utils.h"
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

static size_t count_leaves(const Node* node) {
        if (!node)
                return 0;

        if (!node->left && !node->right)
                return 1;

        return count_leaves(node->left) + count_leaves(node->right);
}

static void test_simple_tree(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("simple.txt"), &frequency_table), 0);

        Node* root = build_tree(&frequency_table);

        assert_non_null(root);
        assert_int_equal(root->frequency, 6);
        assert_int_equal(count_leaves(root), 3);
        assert_non_null(root->left);
        assert_non_null(root->right);

        free_tree(root);
        free_frequency_table(&frequency_table);
}

static void test_single_tree(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("single.txt"), &frequency_table), 0);

        Node* root = build_tree(&frequency_table);

        assert_non_null(root);
        assert_int_equal(root->symbol, (unsigned char)'a');
        assert_int_equal(root->frequency, 10);
        assert_null(root->left);
        assert_null(root->right);

        free_tree(root);
        free_frequency_table(&frequency_table);
}

static void test_all_symbols_tree(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("all_symbols.txt"), &frequency_table), 0);

        Node* root = build_tree(&frequency_table);

        assert_non_null(root);
        assert_int_equal(root->frequency, 94);
        assert_int_equal(count_leaves(root), 94);

        free_tree(root);
        free_frequency_table(&frequency_table);
}

static void test_empty_tree(void** state) {
        (void)state;
        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_int_equal(analyze_file(test_data_path("empty.bin"), &frequency_table), 0);
        assert_null(build_tree(&frequency_table));

        free_frequency_table(&frequency_table);
}

static void test_invalid_tree_arguments(void** state) {
        (void)state;
        assert_null(build_tree(NULL));

        FrequencyTable frequency_table;
        init_frequency_table(&frequency_table);

        assert_null(build_tree(&frequency_table));

        free_frequency_table(&frequency_table);
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
