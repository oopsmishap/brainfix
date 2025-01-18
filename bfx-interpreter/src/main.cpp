#include "bfint.hpp"
#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>

Options parseCmdLine(int argc, char** argv)
{
    Options opt;

    argparse::ArgumentParser parser("bfint");

    parser.add_argument("target").help("The input Brainf*ck (.bf) file.").required();

    parser.add_argument("-t", "--type")
        .help("Specify the number of bytes per BF-cell: int8, int16, or int32 "
              "(default: int8).")
        .default_value(std::string("int8"))
        .action(
            [](const std::string& value)
            {
                static const std::map<std::string, CellType> getType{
                    {"int8", CellType::INT8},
                    {"int16", CellType::INT16},
                    {"int32", CellType::INT32}
                };

                auto it = getType.find(value);
                if (it == getType.end())
                {
                    throw std::runtime_error("Invalid type: " + value);
                }
                return it->second;
            }
        );

    parser.add_argument("-n")
        .help("Specify the number of cells (default: 30,000).")
        .scan<'i', int>()
        .default_value(30000);

    parser.add_argument("--test")
        .help("Run the tests specified by the file.")
        .default_value(std::string(""));

    parser.add_argument("--random")
        .help("Enable Random Brainf*ck extension (support ?-symbol).")
        .default_value(false)
        .implicit_value(true);

    parser.add_argument("--rand-max")
        .help("Specify maximum value returned by RNG (default: cell-type max).")
        .scan<'i', int>()
        .default_value(0);

    parser.add_argument("--no-random-warning")
        .help("Disable warning when ? occurs without --random.")
        .default_value(true)
        .implicit_value(false);

    try
    {
        parser.parse_args(argc, argv);

        opt.bfFile               = parser.get<std::string>("target");
        opt.cellType             = parser.get<CellType>("-t");
        opt.tapeLength           = parser.get<int>("-n");
        opt.testFile             = parser.get<std::string>("--test");
        opt.randomEnabled        = parser.get<bool>("--random");
        opt.randMax              = parser.get<int>("--rand-max");
        opt.randomWarningEnabled = parser.get<bool>("--no-random-warning");

        if (opt.tapeLength <= 0)
        {
            throw std::runtime_error("Tape length must be a positive integer.");
        }

        if (opt.randMax < 0)
        {
            throw std::runtime_error("rand-max must be a positive integer.");
        }
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << "\n";
        std::cerr << parser;
        opt.err = 1;
    }

    return opt;
}

int main(int argc, char** argv)
{
    try
    {
        Options opt = parseCmdLine(argc, argv);
        if (opt.err == 1)
        {
            return 1;
        }

        if (opt.randMax > 0 && !opt.randomEnabled)
        {
            std::cerr
                << "Warning: a value for rand-max was specified but the "
                   "random extension was not enabled. Use --random to enable "
                   "this feature.\n";
        }

        BFInterpreter bfint(opt);
        return bfint.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
