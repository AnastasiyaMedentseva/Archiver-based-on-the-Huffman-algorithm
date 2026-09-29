find_package(cmocka REQUIRED)

if(ENABLE_VALGRIND)
    find_program(VALGRIND valgrind REQUIRED)
endif()

add_executable(test_frequency
    unit/test_frequency.c
)

add_executable(test_tree
    unit/test_tree.c
)

add_executable(test_code
    unit/test_code.c
)

add_executable(test_bitstream
    unit/test_bitstream.c
)

add_executable(test_archiver_integration
    integration/test_archiver_integration.c
)

set(TESTS
    test_frequency
    test_tree
    test_code
    test_bitstream
    test_archiver_integration
)

foreach(TEST ${TESTS})
    target_link_libraries(${TEST}
        PRIVATE
        archiver_core
        cmocka::cmocka
)

if(ENABLE_VALGRIND)
    add_test(
        NAME ${TEST}
        COMMAND
            ${VALGRIND}
            --leak-check=full
            --show-leak-kinds=all
            --error-exitcode=1
            $<TARGET_FILE:${TEST}>
    )
else()
    add_test(NAME ${TEST} COMMAND ${TEST})
endif()

    set_tests_properties(${TEST}
        PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endforeach()
