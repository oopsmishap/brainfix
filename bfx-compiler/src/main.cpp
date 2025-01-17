#include "compiler.hpp"
#include <argparse/argparse.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

struct Options
{
    Compiler::CellType       cellType{Compiler::CellType::INT8};
    std::vector<std::string> includePaths;
    std::string              bfxFile;
    std::string              testFile;
    std::string              profileFile;
    std::ostream*            outStream{&std::cout};
    bool                     constEvalAllowed{true};
    bool                     randomEnabled{false};
    bool                     bcrEnabled{true};
    bool                     includeWarningEnabled{true};
    bool                     assertWarningEnabled{true};
    int                      maxUnrollIterations{20};
};

Compiler::Options parseArguments(int argc, char** argv)
{
    argparse::ArgumentParser program("compiler");

    program.add_argument("target").help("Input target file (.bfx)").required();

    program.add_argument("-t", "--type")
        .help("Specify the number of bytes per BF-cell, where [Type] is one of "
              "int8, int16, and int32 (int8 by default).")
        .default_value(std::string("int8"))
        .action(
            [](const std::string& value)
            {
                static const std::map<std::string, Compiler::CellType> cellTypeMap = {
                    {"int8", Compiler::CellType::INT8},
                    {"int16", Compiler::CellType::INT16},
                    {"int32", Compiler::CellType::INT32}
                };
                auto it = cellTypeMap.find(value);
                if (it == cellTypeMap.end())
                {
                    throw std::invalid_argument("Invalid cell type: " + value);
                }
                return it->second;
            }
        );

    program.add_argument("-I", "--include-path")
        .help("Specify additional include-path(s). This option may appear "
              "multiple times to specify multiple folders.")
        .append()
        .default_value(std::vector<std::string>{});

    program.add_argument("-o", "--output")
        .help("Specify the output stream/file (default stdout).")
        .default_value(std::string("stdout"));

    program.add_argument("-O0")
        .help("Do NOT do any constant expression evaluation.")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-O1")
        .help("Do constant expression evaluation (default).")
        .default_value(true)
        .implicit_value(true);

    program.add_argument("--max-unroll-iterations")
        .help("Specify the maximum number of loop-iterations that will be "
              "unrolled. Defaults to 20.")
        .default_value(20)
        .scan<'i', int>();

    program.add_argument("--test")
        .help("Produce test-files and write a list of generated files to "
              "[file], to be used by bfint for unit-testing.")
        .default_value(std::string(""));

    program.add_argument("--random")
        .help("Enable random number generation (generates the ?-symbol). Your "
              "interpreter must support this extension!")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--profile")
        .help("Write the memory profile to a file. In this file, the number of "
              "visits to each of the cells is listed.")
        .default_value(std::string(""));

    program.add_argument("--no-bcr")
        .help(
            "Disable break/continue/return statements for more compact output."
        )
        .default_value(false)
        .implicit_value(false);

    program.add_argument("--no-multiple-inclusion-warning")
        .help("Do not warn when a file is included more than once, or when "
              "files with duplicate names are included.")
        .default_value(true)
        .implicit_value(false);

    program.add_argument("--no-assert-warning")
        .help("Do not warn when static_assert is used in non-constant context.")
        .default_value(true)
        .implicit_value(false);

    program.parse_args(argc, argv);

    Compiler::Options opt;
    opt.cellType         = program.get<Compiler::CellType>("-t");
    opt.includePaths     = program.get<std::vector<std::string>>("-I");
    opt.bfxFile          = program.get<std::string>("target");
    opt.testFile         = program.get<std::string>("--test");
    opt.profileFile      = program.get<std::string>("--profile");
    opt.constEvalAllowed = !program.get<bool>("-O0");
    opt.randomEnabled    = program.get<bool>("--random");
    opt.bcrEnabled       = !program.get<bool>("--no-bcr");
    opt.includeWarningEnabled =
        program.get<bool>("--no-multiple-inclusion-warning");
    opt.assertWarningEnabled = program.get<bool>("--no-assert-warning");
    opt.maxUnrollIterations  = program.get<int>("--max-unroll-iterations");

    std::string output = program.get<std::string>("-o");
    if (output == "stdout")
    {
        opt.outStream = &std::cout;
    }
    else
    {
        static std::ofstream file(output);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open output file: " + output);
        }
        opt.outStream = &file;
    }

    return opt;
}

int main(int argc, char** argv)
{
    try
    {
        Compiler::Options opt = parseArguments(argc, argv);
        Compiler          c(opt);
        int               err = c.compile();
        if (err)
        {
            return err;
        }
        c.write();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
