cmake_minimum_required(VERSION 3.9...4.1)
project(kenny C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

enable_testing()

option(CodeCoverage "CodeCoverage" OFF)



set(CMAKE_C_STANDARD 99)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

add_definitions(-Wall)


if (CodeCoverage MATCHES ON)
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CFLAGS -Wall -g -O0 --coverage)

    include(CodeCoverage)

    APPEND_COVERAGE_COMPILER_FLAGS()

    SETUP_TARGET_FOR_COVERAGE_GCOVR_XML(
            NAME coverage
            EXECUTABLE ctest -V -j ${PROCESSOR_COUNT}
            DEPENDENCIES kenny
    )

    SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
            NAME coverage-html
            EXECUTABLE ctest -V -j ${PROCESSOR_COUNT}
            DEPENDENCIES kenny
    )
endif()


configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

add_executable(kenny main.c)


install(TARGETS kenny DESTINATION bin)
install(FILES kenny.1 DESTINATION share/man/man1)



add_test(NAME version COMMAND ./kenny --version)
set_tests_properties(version PROPERTIES PASS_REGULAR_EXPRESSION "kenny [0-9]+.[0-9]+.[0-9]+")

add_test(NAME help COMMAND ./kenny --help)
set_tests_properties(help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

add_test(NAME extra-argument COMMAND ./kenny asdf foo)
set_tests_properties(extra-argument PROPERTIES PASS_REGULAR_EXPRESSION "--help' for more information")

add_test(NAME unknown-option COMMAND ./kenny --asdf)
set_tests_properties(unknown-option PROPERTIES PASS_REGULAR_EXPRESSION "--help' for more information")

add_test(NAME file-not-found COMMAND ./kenny asdf)
set_tests_properties(file-not-found PROPERTIES PASS_REGULAR_EXPRESSION "asdf: No such file or directory")

add_test(NAME encode COMMAND /bin/sh -c "echo -n abcdefghijklmnopqrstuvwxyz | ./kenny")
set_tests_properties(encode PROPERTIES PASS_REGULAR_EXPRESSION "mmmmmpmmfmpmmppmpfmfmmfpmffpmmpmppmfppmpppppfpfmpfppfffmmfmpfmffpmfppfpfffmffp")

add_test(NAME decode COMMAND /bin/sh -c "echo -n mmmmmpmmfmpmmppmpfmfmmfpmffpmmpmppmfppmpppppfpfmpfppfffmmfmpfmffpmfppfpfffmffp | ./kenny -d")
set_tests_properties(decode PROPERTIES PASS_REGULAR_EXPRESSION "abcdefghijklmnopqrstuvwxyz")

add_test(NAME encode_special COMMAND /bin/sh -c "echo -n Hallo Welt! | ./kenny")
set_tests_properties(encode_special PROPERTIES PASS_REGULAR_EXPRESSION "Mfpmmmpmfpmfppf Fppmpppmffmp!")

add_test(NAME decode_special COMMAND /bin/sh -c "echo -n 'Mfpmmmpmfpmfppf Fppmpppmffmp! Bye' | ./kenny -d")
set_tests_properties(decode_special PROPERTIES PASS_REGULAR_EXPRESSION "Hallo Welt! Bye")

add_test(NAME file-input COMMAND /bin/sh -c "echo -n abc > foobar.txt ; ./kenny foobar.txt ; rm foobar.txt")
set_tests_properties(file-input PROPERTIES PASS_REGULAR_EXPRESSION "mmmmmpmmf")
