# Tests need to be added as executables first
add_executable(testlib bzip2.cpp gzip.cpp lzma.cpp zlib.cpp test_main.cpp)

# I'm using C++17 in the test
target_compile_features(testlib PRIVATE cxx_std_20)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(testlib PRIVATE decoco Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibtest COMMAND testlib) # Command can be a target
