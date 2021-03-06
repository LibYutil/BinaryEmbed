
#include <memory>
#include <iostream>

#include "binembed/some_lib.hxx"

int main(int argc, char ** argv) {

    constexpr auto length = sizeof(some_lib_data);

    std::unique_ptr<char[]> m;
    m.reset(new char[length + 1]);
    m.get()[length] = 0;

    for (int i = 0; i < length; ++i) {
        m.get()[i] = some_lib_data[i];
    }

    std::cout << m.get() << std::endl;

}
