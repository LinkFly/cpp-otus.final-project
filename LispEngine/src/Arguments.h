#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::function;
using std::string;
using std::vector;

string helpHeader = string{}
+"Welcome to `LispEngine` program!\n"
+ "Usage:"
+ "./bayan --load <full_path_to_lisp_file> --eval <expr_string>\n"
+ "\nShort options: load - l, eval - e";

#ifdef _MSC_VER
#define _GLIBCXX_USE_NOEXCEPT
#endif
struct MyException : public std::exception {
    char const* errMes;
    MyException(char const* mes) : errMes{ mes } {}
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT override {
        return errMes;
    }
};

struct Arguments {
    std::unique_ptr<po::options_description> desc;
    std::string file;
    std::string sexprStr;

    Arguments() {
        // make_unique since c++14
        //desc = std::make_unique<po::options_description>(helpHeader.c_str());
        desc.reset(new po::options_description(helpHeader.c_str()));
    }
    void parse(int argc, char** argv) {
        desc->add_options()
            ("help,h", "Show help")
            ("load,l", po::value<std::string>(&file), "Select block size (must be > 0)")
            ("eval,e", po::value<std::string>(&sexprStr), "Select hashType: crc32, md5, sha1")
            ;

        // For unlimited files after --block-size, --hash-type (without `--file` before file)
        /*po::positional_options_description posOptions;
        posOptions.add("file", -1);*/

        po::variables_map vm;
        try {
            auto options = po::command_line_parser(argc, argv).options(*desc);// .positional(posOptions);
            po::parsed_options parsed = options.run();
            po::store(parsed, vm);
            if (vm.count("help")) {
                showDesc();
                exit(0);
            }
            po::notify(vm);
            check_arguments(vm);
        }
        catch (std::exception & ex) {
            std::cerr << "ERROR: " << ex.what() << "\n\n";
            showDesc();
            exit(-1);
        }

    }

    void showDesc() {
        std::cout << *desc << std::endl;
    }

    void check_arguments(const po::variables_map& vm) //throw(std::exception)
    {
        /*if (blockSize == 0) {
            throw MyException("Failed: --block-size must be > 0");
        }
        if (vm.count("dir") && vm.count("file")) {
            throw MyException("Failed: denied both --dir and --file options");
        }
        if (vm.count("file") && vm["file"].as<vector<string>>().size() <= 1) {
            throw MyException("Failed: --file options count must be > 1 (or --dir options must be have)");
        }*/
    }
};
