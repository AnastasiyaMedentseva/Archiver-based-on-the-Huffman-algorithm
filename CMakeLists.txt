cmake_minimum_required(VERSION 3.16)

project(Archiver LANGUAGES C)

option(BUILD_TESTS "Build tests" ON)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_VALGRIND "Run tests under Valgrind" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
option(ENABLE_CLANG_FORMAT "Check clang-format" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

if(ENABLE_ASAN)
    add_compile_options(
        -fsanitize=address
        -fno-omit-frame-pointer
    )
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

if(ENABLE_CLANG_FORMAT)
    find_program(CLANG_FORMAT clang-format REQUIRED)

    file(GLOB_RECURSE FORMAT_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/test/*.c
        ${CMAKE_SOURCE_DIR}/test/*.h
    )

    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT}
            --dry-run
            --Werror
            ${FORMAT_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy REQUIRED)
endif()

add_library(archiver_core STATIC
    src/bitstream/bitstream.c
    src/code_table/code.c
    src/compressor/compressor.c
    src/decompressor/decompressor.c
    src/frequency_table/frequency.c
    src/tree/tree.c
    src/cli/cli.c
)

target_include_directories(archiver_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(archiver
    src/main.c
)

target_link_libraries(archiver PRIVATE archiver_core)

if(ENABLE_CLANG_TIDY)
    set_target_properties(archiver_core archiver
        PROPERTIES
            C_CLANG_TIDY "${CLANG_TIDY}"
    )
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
