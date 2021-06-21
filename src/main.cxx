
#include <fstream>

#include <iostream>
#define n_print(expr) std::cout << expr << std::endl

#include "CxxCli/CxxCli.hpp"

int main(int argc, char ** argv) {

    const char * identifier = nullptr;
    const char * target = nullptr;
    const char * src_name = nullptr;
    const char * header_name = nullptr;
    const char * data_type = nullptr;
    signed char use_c_linkage = 0;

    {
        using namespace CxxCli;

        auto cmd = Command(
            Loop(
                Sequence(
                    Optional(Sequence(Const("--identifier"), Var() >> [&] (const char * v) { identifier = v; })),
                    Optional(Sequence(Const("--target"), Var() >> [&] (const char * v) { target = v; })),
                    Optional(Sequence(Const("--out-src-path"), Var() >> [&] (const char * v) { src_name = v; })),
                    Optional(Sequence(Const("--out-header-path"), Var() >> [&] (const char * v) { header_name = v; })),
                    Optional(Sequence(Const("--data-type"), Var() >> [&] (const char * v) { data_type = v; })),
                    Optional(Const("--use-c-linkage") >> [&] { use_c_linkage = 1; })
                )
            )
        );

        auto r = cmd.parse(argc - 1, argv + 1);
        if (!r) {
            n_print("Failed to parse args");
            r.printUsage(std::cout, argv[0]);
            return -1;
        }
    }

    if (identifier == nullptr) { n_print("identifier not set"); return 1; }
    if (target == nullptr) { n_print("target not set"); return 1; }
    if (src_name == nullptr) { n_print("source name not set"); return 1; }
    if (header_name == nullptr) { n_print("header name not set"); return 1; }
    if (data_type == nullptr) { data_type = "unsigned char"; }

    {
        auto l_write_c_linkage_start = [&] (std::ofstream & s) {
            if (use_c_linkage == 1) {
                s << "#if __cplusplus" << "\n";
                s << "extern \"C\" {" << "\n";
                s << "#endif" << "\n";
            }
        };
        auto l_write_c_linkage_end = [&] (std::ofstream & s) {
            if (use_c_linkage == 1) {
                s << "#if __cplusplus" << "\n";
                s << "}" << "\n";
                s << "#endif" << "\n";
            }
        };
        long long length = 0;

        {
            std::basic_ifstream<unsigned char> input(target, std::ios::binary);
            if (!input) { n_print("failed to open target file"); return -1; }

            std::ofstream src(src_name, std::ios::trunc);
            if (!src) { n_print("failed to open source file"); return -1; }

            src << "\n";

            l_write_c_linkage_start(src);
            
            { // write data
                src << "static const " << data_type <<  " " << identifier << "_data0[] = {";
                {
                    {
                        unsigned char c;
                        input.get(c);
                        if (!input.good()) { goto lbl_skip_read; }
                        src << (unsigned int)c;
                        length++;
                    }
                    while (true) {
                        unsigned char c;
                        input.get(c);
                        if (!input.good()) { break; }
                        src << ',' << (unsigned int)c;
                        length++;
                    }
                lbl_skip_read:;
                }
                src << "};" << "\n";

                src << "\n";
                src << "extern const " << data_type << " * const " << identifier << "_data = " << identifier << "_data0;" << "\n";

                src << "\n";
                src << "extern const long long " << identifier << "_length = " << length << ";" << "\n";
            }
            
            l_write_c_linkage_end(src);
            src << "\n";

            if (!src) { n_print("failed to write source file"); return -1; }
        }

        {
            std::ofstream header(header_name, std::ios::trunc);
            if (!header) { n_print("failed to open header file"); return -1; }

            header << "\n";
            header << "#ifndef HEADER_BINARY_EMBED__" << identifier << "__EMBEDDING" << "\n";
            header << "#define HEADER_BINARY_EMBED__" << identifier << "__EMBEDDING 1" << "\n";
            l_write_c_linkage_start(header);
            header << "extern const " << data_type <<  " * const " << identifier << "_data;" << "\n";
            header << "extern const long long " << identifier << "_length;" << "\n";
            l_write_c_linkage_end(header);
            header << "#endif" << "\n";

            if (!header) { n_print("failed to write header file"); return -1; }
        }

    }
    
    return 0;
}