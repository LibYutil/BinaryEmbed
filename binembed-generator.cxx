
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#define n_print(expr) std::cout << expr << std::endl

#include "CxxCli/CxxCli.hpp"

namespace {
    template <class x>
    struct malloc_delete {
        constexpr malloc_delete() noexcept = default;
        void operator()(x * o) const noexcept { o->~x(); std::free(o); }
    };
}

static int main0(int argc, char ** argv) {

    static constexpr long long default_buffer_size = 256;

    const char * identifier = nullptr;
    const char * target = nullptr;
    const char * src_name = nullptr;
    const char * header_name = nullptr;
    const char * data_type = nullptr;
    bool use_c_linkage = false;
    bool append_0 = false;
    long long buffer_size = default_buffer_size;

    using namespace CxxCli;

    auto cmd = Command(
        Loop(
            Sequence(
                Sequence(Optional(Sequence(Const("--identifier"), Var("identifier") >> &identifier))) & UsageAsList & Doc("use identifier"),
                Sequence(Optional(Sequence(Const("--target"), Var("target") >> &target))) & UsageAsList & Doc("target file to embed"),
                Sequence(Optional(Sequence(Const("--out-src-path"), Var("source") >> &src_name))) & UsageAsList & Doc("source path to output to"),
                Sequence(Optional(Sequence(Const("--out-header-path"), Var("header") >> &header_name))) & UsageAsList & Doc("header path to output to"),
                Sequence(Optional(Sequence(Const("--data-type"), Var("data-type") >> &data_type))) & UsageAsList & Doc("c++ data type to store data as [default = unsigned char]"),
                Sequence(Optional(Const("--use-c-linkage") >> [&] { use_c_linkage = true; })) & UsageAsList & Doc("declare embedded accessors with 'extern \"C\"' linkage"),
                Sequence(Optional(Const("--append-null") >> [&] { append_0 = true; })) & UsageAsList & Doc("append null character at the end of the embedded data"),
                Sequence(Optional(Sequence(Const("--copy-buffer"), Var("copy-buffer") >> &buffer_size))) & UsageAsList & Doc("size of the copy buffer to use [default, min = 256]")
            ) & UsageAsList
        )
    );

    {
        auto r = cmd.parse(argc - 1, argv + 1);
        if (!r) {
            n_print("Failed to parse args");
            r.printUsage(std::cout, argv[0]);
            return -1;
        }
    }

    if (identifier == nullptr) { n_print("identifier not set"); cmd.printUsage(std::cerr, argv[0]); return -1; }
    if (target == nullptr) { n_print("target not set"); cmd.printUsage(std::cerr, argv[0]); return -1; }
    if (src_name == nullptr) { n_print("source name not set"); cmd.printUsage(std::cerr, argv[0]); return -1; }
    if (header_name == nullptr) { n_print("header name not set"); cmd.printUsage(std::cerr, argv[0]); return -1; }
    if (data_type == nullptr) { data_type = "unsigned char"; }
    if (buffer_size < default_buffer_size) { n_print("copy-buffer too small: " << buffer_size); return -1; }

    {
        auto l_write_c_linkage_start = [&] (std::ofstream & s) {
            if (use_c_linkage) {
                s << "#if __cplusplus" << "\n";
                s << "extern \"C\" {" << "\n";
                s << "#endif" << "\n";
            }
        };
        auto l_write_c_linkage_end = [&] (std::ofstream & s) {
            if (use_c_linkage) {
                s << "#if __cplusplus" << "\n";
                s << "}" << "\n";
                s << "#endif" << "\n";
            }
        };
        unsigned long long length = 0;
         
        {
            std::ifstream input(target, std::ios::binary);
            if (!input) { n_print("failed to open target file: " << target); return -1; }
            input.exceptions(std::ios_base::badbit);

            std::ofstream src(src_name, std::ios::trunc);
            if (!src) { n_print("failed to open source file" << src_name); return -1; }
            src.exceptions(std::ios_base::badbit | std::ios_base::failbit);

            src << "\n";

            l_write_c_linkage_start(src);
            
            { // write data
                src << "static const " << data_type <<  " " << identifier << "_data0[] = {";
                {
                    unsigned char stackBuffer[default_buffer_size];
                    std::unique_ptr<unsigned char, malloc_delete<unsigned char>> heapBuffer;
                    unsigned char * buffer;
                    if (buffer_size <= default_buffer_size) {
                        buffer = stackBuffer;
                    } else {
                        buffer = (unsigned char *)std::malloc(buffer_size);
                        if (buffer == nullptr) { throw std::bad_alloc(); }
                        heapBuffer.reset(buffer);
                    }

                    auto readData = [&] () -> std::streamsize {
                        std::streamsize bufPos = 0;
                        while (bufPos < buffer_size && !input.eof()) {
                            input.read((char *)buffer, buffer_size - bufPos);
                            if (!input.eof() && input.fail()) { throw std::runtime_error("failed to read target file"); }
                            std::streamsize count = input.gcount();
                            if (count == 0) { break; }
                            bufPos += count;
                            length += count;
                        }
                        return bufPos; // data count available
                    };

                    {
                        auto i = readData();
                        if (i != 0) {
                            src << (unsigned short)buffer[0];
                            for (std::streamsize j = 1; j < i; ++j) {
                                src << ',' << (unsigned short)buffer[j];
                            }
                        }
                    }
                    for (auto i = readData(); i != 0; i = readData()) {
                        for (std::streamsize j = 0; j < i; ++j) {
                            src << ',' << (unsigned short)buffer[j];
                        }
                    }
                }

                if (append_0) {
                    if (length > 0) { src << ','; }
                    src << (unsigned short)0;
                    ++length;
                }

                src << "};" << "\n";

                src << "\n";
                src << "extern const " << data_type << " * const " << identifier << "_data = " << identifier << "_data0;" << "\n";

                src << "\n";
                src << "extern const unsigned long long " << identifier << "_length = " << length << ";" << "\n";
            }
            
            l_write_c_linkage_end(src);
            src << "\n";

            if (!src) { n_print("failed to write source file"); return -1; }
        }

        {
            std::ofstream header(header_name, std::ios::trunc);
            if (!header) { n_print("failed to open header file: " << header_name); return -1; }

            header << "\n";
            header << "#ifndef HEADER_BINARY_EMBED__" << identifier << "__EMBEDDING" << "\n";
            header << "#define HEADER_BINARY_EMBED__" << identifier << "__EMBEDDING 1" << "\n";
            l_write_c_linkage_start(header);
            header << "extern const " << data_type <<  " * const " << identifier << "_data;" << "\n";
            header << "extern const unsigned long long " << identifier << "_length;" << "\n";
            l_write_c_linkage_end(header);
            header << "#endif" << "\n";

            if (!header) { n_print("failed to write header file"); return -1; }
        }

    }
    
    return 0;
}

int main(int argc, char ** argv) {
    try {
        return main0(argc, argv);
    } catch (std::exception & ex) {
        std::cerr << "uncaught exception" << std::endl;
        std::cerr << "what(): " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "uncaught unknwon exception" << std::endl;
    }
    return -2;
}
