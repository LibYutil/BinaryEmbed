
#include <cassert>

#include <vector>
#include <cstring>
#include <iostream>

#include "binembed/someBinary_file.hxx"

int main(int argc, char ** argv) {

    const unsigned char expectedData[] = { 'b', 'i', 'n', 'a', 'r', 'y', ' ', 'd', 'a', 't', 'a' };

    assert((sizeof(expectedData) / sizeof(unsigned char)) == someBinary_file_length);
    assert(std::memcmp(someBinary_file_data, expectedData, someBinary_file_length) == 0);

}
