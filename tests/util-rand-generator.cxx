
#include <vector>
#include <random>
#include <fstream>
#include <iostream>

#include "CxxCli/CxxCli.hpp"

static int main0(int argc, char ** argv) {
    constexpr unsigned long long static_buffer_size = 256;

    using namespace CxxCli;

    unsigned long long bytes = 0;
    const char * output_file = nullptr;
    unsigned long long buffer_size = static_buffer_size;

    auto cmd = Command(
        Sequence(
            Optional(Sequence(Const("--buffer-size"), Var("buffer-size") >> &buffer_size)),
            Var("byte-count") >> &bytes,
            Var("output-file") >> &output_file
        )
    );

    auto r = cmd.parse(argc - 1, argv + 1);
    if (!r) {
        r.printUsage(std::cout, argv[0]);
        return -1;
    }

    {
        using rt = std::mt19937_64::result_type;
        static_assert(static_buffer_size % sizeof(rt) == 0, "static_buffer_size should be a divisible by std::mt19937_64::result_type");

        buffer_size += (buffer_size % sizeof(rt));

        rt stack_buffer[static_buffer_size / sizeof(rt)];
        std::vector<rt> heap_buffer;
        rt * buffer;
        if (buffer_size <= static_buffer_size) {
            buffer = stack_buffer;
        } else {
            heap_buffer.resize(buffer_size / sizeof(rt));
            buffer = &heap_buffer[0];
        }

        using ull = unsigned long long;
        std::mt19937_64 engine;

        auto fillBuffer = [&] () {
            for (ull i = 0, l = (buffer_size / sizeof(rt)); i < l; ++i) {
                buffer[i] = engine();
            }
        };

        {
            std::ofstream output(output_file, std::ios_base::binary);
            if (!output) { std::cout << "failed to open output file: " << output_file << std::endl; return 1; }
            output.exceptions(std::ios_base::badbit | std::ios_base::failbit);

            for (ull written = 0; written < bytes; written += buffer_size) {
                fillBuffer();
                auto bytesToWrite = bytes - written;
                output.write((const char *)buffer, bytesToWrite > buffer_size ? buffer_size : bytesToWrite);
            }
        }

    }

    return 0;
}

int main(int argc, char ** argv) {
    try {
        return main0(argc, argv);
    } catch (std::exception & ex) {
        std::cout << "uncaught exception" << std::endl;
        std::cout << ex.what() << std::endl;
    } catch (...) {
        std::cout << "uncaught unknown exception" << std::endl;
    }
    return -1;
}
