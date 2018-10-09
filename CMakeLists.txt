cmake_minimum_required(VERSION 3.9)
project(kenny C)

set(CMAKE_C_STANDARD 99)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

add_definitions(-Wall)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

add_executable(kenny main.c)

install(TARGETS kenny DESTINATION bin)


enable_testing()

add_test(NAME version COMMAND ./kenny --version)
set_tests_properties(version PROPERTIES PASS_REGULAR_EXPRESSION "kenny [0-9]+.[0-9]+.[0-9]+")

add_test(NAME help COMMAND ./kenny --help)
set_tests_properties(help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

add_test(NAME encode COMMAND /bin/sh -c "echo -n abcdefghijklmnopqrstuvwxyz | ./kenny")
set_tests_properties(encode PROPERTIES PASS_REGULAR_EXPRESSION "mmmmmpmmfmpmmppmpfmfmmfpmffpmmpmppmfppmpppppfpfmpfppfffmmfmpfmffpmfppfpfffmffp")

add_test(NAME decode COMMAND /bin/sh -c "echo -n mmmmmpmmfmpmmppmpfmfmmfpmffpmmpmppmfppmpppppfpfmpfppfffmmfmpfmffpmfppfpfffmffp | ./kenny -d")
set_tests_properties(decode PROPERTIES PASS_REGULAR_EXPRESSION "abcdefghijklmnopqrstuvwxyz")

add_test(NAME encode_special COMMAND /bin/sh -c "echo -n Hallo Welt! | ./kenny")
set_tests_properties(encode_special PROPERTIES PASS_REGULAR_EXPRESSION "Mfpmmmpmfpmfppf Fppmpppmffmp!")

add_test(NAME decode_special COMMAND /bin/sh -c "echo -n Mfpmmmpmfpmfppf Fppmpppmffmp! | ./kenny -d")
set_tests_properties(decode_special PROPERTIES PASS_REGULAR_EXPRESSION "Hallo Welt!")
