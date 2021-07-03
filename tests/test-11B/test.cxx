
#include <cassert>

#include <vector>
#include <cstring>
#include <iostream>

#include "binembed/someBinary_file.hxx"

int main(int argc, char ** argv) {

    constexpr const char expectedData[] = "binary data";

    assert(sizeof(expectedData) == someBinary_file_length);
    assert(std::strcmp((const char *)someBinary_file_data, expectedData) == 0);

}
