# ==============================================================================
# Example Applications
# ==============================================================================
# This CMakeLists.txt builds example applications that demonstrate how to use
# the cobs library.
#
# Users can:
# - Add new example applications by creating new executables
# - Modify existing examples to demonstrate different features
# - Choose whether to install examples (disabled by default)

# ==============================================================================
# Example: POSIX CLI Application
# ==============================================================================
# Build only on POSIX-like systems (excluding Zephyr).
if(UNIX AND NOT CMAKE_SYSTEM_NAME STREQUAL "Zephyr")
    # Create the CLI example executable
    add_executable(CobsExample
        example.c
    )

    # Link with the cobs library
    # Using the alias target ensures proper namespacing
    target_link_libraries(CobsExample
        PRIVATE
            Cobs
    )

    # Set C99 standard for the example
    target_compile_features(CobsExample
        PRIVATE
            c_std_99
    )

    # Add compiler warnings for the example (same as library)
    target_compile_options(CobsExample
        PRIVATE
            # GCC and Clang warnings
            $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
                -Wall
                -Wextra
                -Wpedantic
            >
            # MSVC warnings
            $<$<C_COMPILER_ID:MSVC>:
                /W4
            >
    )
endif()

# ==============================================================================
# Example: Embedded-Safe Application
# ==============================================================================
add_executable(CobsEmbeddedExample
    example_embedded.c
)

target_link_libraries(CobsEmbeddedExample
    PRIVATE
        Cobs::Cobs
)

target_compile_features(CobsEmbeddedExample
    PRIVATE
        c_std_99
)

target_compile_options(CobsEmbeddedExample
    PRIVATE
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
            -Wall
            -Wextra
            -Wpedantic
        >
        $<$<C_COMPILER_ID:MSVC>:
            /W4
        >
)

# ==============================================================================
# Adding New Examples (Template)
# ==============================================================================
# To add a new example application, copy and modify this template:
#
# add_executable(example_cobs_myexample
#     myexample.c
# )
#
# target_link_libraries(example_cobs_myexample
#     PRIVATE
#         Cobs::Cobs
# )
#
# target_compile_features(example_cobs_myexample
#     PRIVATE
#         c_std_99
# )

# ==============================================================================
# Installation (Optional)
# ==============================================================================
# By default, examples are not installed to keep the installation clean.
# Uncomment the section below if you want to install example executables.

# install(
#     TARGETS CobsExample CobsEmbeddedExample
#     RUNTIME
#         DESTINATION ${CMAKE_INSTALL_BINDIR}
#         COMPONENT Examples
# )

# To install examples along with the library:
# cmake --install . --component Examples
