#include <cassert>
#include <chrono>
#include <csignal>
#include <fstream>
#include <sstream>
#include <string>

#include "bfint.hpp"

namespace _MaxInt
{
template <typename T> constexpr size_t _getMax()
{
    return (static_cast<size_t>(1) << (8 * sizeof(T))) - 1;
}

inline static size_t get(CellType c)
{
    switch (c)
    {
    case CellType::INT8:
        return _getMax<uint8_t>();
    case CellType::INT16:
        return _getMax<uint16_t>();
    case CellType::INT32:
        return _getMax<uint32_t>();
    }
    throw -1;
}
} // namespace _MaxInt

BFInterpreter::BFInterpreter(Options const& opt)
    : d_array(opt.tapeLength)
    , d_uniformDist(0, (opt.randMax != 0) ? opt.randMax : _MaxInt::get(opt.cellType))
    , d_cellType(opt.cellType)
    , d_randomEnabled(opt.randomEnabled)
    , d_randMax(opt.randMax)
    , d_randomWarningEnabled(opt.randomWarningEnabled)
    , d_gamingMode(opt.gamingMode)
    , d_testFile(opt.testFile)
{
    // init code
    std::ifstream file(opt.bfFile);
    if (!file.is_open())
        throw std::string("File not found: ") + opt.bfFile;

    std::stringstream buffer;
    buffer << file.rdbuf();
    d_code = buffer.str();

    // init rng
    auto t0 = std::chrono::system_clock::now().time_since_epoch();
    auto ms = duration_cast<std::chrono::milliseconds>(t0).count();
    d_rng.seed(ms);
}

void BFInterpreter::reset()
{
    std::fill(d_array.begin(), d_array.end(), 0);
    d_arrayPointer = 0;
    d_codePointer  = 0;
    d_loopStack    = std::stack<int>{};
}

int BFInterpreter::run()
{
    if (d_testFile.empty())
        return run(std::cin, std::cout);

    auto const report = [](std::string const& testName,
                           std::string const& caseName,
                           std::string const& output,
                           std::string const& expect)
    {
        std::cout << '<' << testName << "::" << caseName << "> ";
        if (output == expect)
        {
            std::cout << "PASS\n";
            return 0;
        }

        std::cout << "FAIL\n"
                  << "\tExpected: \"";
        for (char c : expect)
            std::cout << (std::isprint(c) ? c : '.');
        std::cout << "\"\n\tGot:      \"";
        for (char c : output)
            std::cout << (std::isprint(c) ? c : '.');
        std::cout << "\"\n";
        return 1;
    };

    auto const split = [](std::string const& str, char const s)
    {
        std::vector<std::string> result;
        std::string              current;
        for (char c : str)
        {
            if (c == s)
            {
                result.push_back(current);
                current = "";
            }
            else
                current += c;
        }
        result.push_back(current);
        return result;
    };

    auto const loadStringStream =
        [](std::stringstream& ss, std::string const& filename)
    {
        std::ifstream file(filename);
        if (!file)
        {
            std::cerr << "ERROR: coult not open file " << filename << '\n';
            return false;
        }
        ss << file.rdbuf();
        return true;
    };

    std::ifstream file(d_testFile);
    if (!file)
    {
        std::cerr << "ERROR: coult not open test-file " << d_testFile << '\n';
        return -1;
    }

    std::vector<std::string> lines;
    std::string              line;
    while (std::getline(file, line))
        lines.push_back(line);

    int errCount = 0;
    for (std::string const& base : lines)
    {
        std::vector<std::string> parts = split(base, '-');
        assert(parts.size() == 3);

        std::string const testName = parts[1];
        std::string const caseName = parts[2];

        std::stringstream inputString;
        if (!loadStringStream(inputString, base + ".input"))
            return -1;

        std::stringstream expectString;
        if (!loadStringStream(expectString, base + ".expect"))
            return -1;

        std::ostringstream bfOutput;
        run(inputString, bfOutput);
        errCount += report(testName, caseName, bfOutput.str(), expectString.str());
    }

    return errCount;
}

int BFInterpreter::run(std::istream& in, std::ostream& out)
{
    reset();

    while (true)
    {
        char token = d_code[d_codePointer];
        switch (token)
        {
        case LEFT:
            pointerDec();
            break;
        case RIGHT:
            pointerInc();
            break;
        case PLUS:
            plus();
            break;
        case MINUS:
            minus();
            break;
        case PRINT:
        {
            if (d_gamingMode)
                printCurses();
            else
                print(out);
            break;
        }
        case READ:
        {
            if (d_gamingMode)
                readCurses();
            else
                read(in);
            break;
        }
        case START_LOOP:
            startLoop();
            break;
        case END_LOOP:
            endLoop();
            break;
        case RAND:
        {
            static bool warned = false;
            if (d_randomEnabled)
                random();
            else if (d_randomWarningEnabled && !warned)
            {
                static std::string const warning =
                    "\n"
                    "=========================== !!!!!! "
                    "==============================\n"
                    "Warning: BF-code contains '?'-commands, which may be\n"
                    "interpreted as the random-operation, an extension to the\n"
                    "canonical BF instructionset. This extension can be "
                    "enabled\n"
                    "with the --random option.\n"
                    "This warning can be disabled with the --no-random-warning "
                    "option.\n"
                    "=========================== !!!!!! "
                    "==============================\n";

                if (!d_gamingMode)
                    std::cerr << warning;
                else
                {
                    assert(false);
                }
                warned = true;
            }
            break;
        }
        default:
            break;
        }

        if (++d_codePointer >= d_code.size())
            break;
    }

    return 0;
}

int BFInterpreter::consume(Ops op)
{
    assert(d_code[d_codePointer] == op && "codepointer should be pointing at op now");

    int n = 1;
    while (d_code[d_codePointer + n] == op)
        ++n;

    d_codePointer += (n - 1);
    return n;
}

void BFInterpreter::plus()
{
    int const n = consume(PLUS);
    switch (d_cellType)
    {
    case CellType::INT8:
        d_array[d_arrayPointer] = static_cast<uint8_t>(d_array[d_arrayPointer] + n);
        break;
    case CellType::INT16:
        d_array[d_arrayPointer] =
            static_cast<uint16_t>(d_array[d_arrayPointer] + n);
        break;
    case CellType::INT32:
        d_array[d_arrayPointer] =
            static_cast<uint32_t>(d_array[d_arrayPointer] + n);
        break;
    }
}

void BFInterpreter::minus()
{
    int const n = consume(MINUS);
    switch (d_cellType)
    {
    case CellType::INT8:
        d_array[d_arrayPointer] = static_cast<uint8_t>(d_array[d_arrayPointer] - n);
        break;
    case CellType::INT16:
        d_array[d_arrayPointer] =
            static_cast<uint16_t>(d_array[d_arrayPointer] - n);
        break;
    case CellType::INT32:
        d_array[d_arrayPointer] =
            static_cast<uint32_t>(d_array[d_arrayPointer] - n);
        break;
    }
}

void BFInterpreter::pointerInc()
{
    int const n = consume(RIGHT);
    d_arrayPointer += n;

    while (d_arrayPointer >= d_array.size())
        d_array.resize(2 * d_array.size());
}

void BFInterpreter::pointerDec()
{
    if (d_arrayPointer == 0)
        throw std::string("Error: trying to decrement pointer beyond beginning.");

    int const n = consume(LEFT);
    d_arrayPointer -= n;
}

void BFInterpreter::startLoop()
{
    if (d_array[d_arrayPointer] != 0)
    {
        d_loopStack.push(d_codePointer);
    }
    else
    {
        int bracketCount = 1;
        while (bracketCount != 0 && d_codePointer < d_code.size())
        {
            ++d_codePointer;
            if (d_code[d_codePointer] == START_LOOP)
                ++bracketCount;
            else if (d_code[d_codePointer] == END_LOOP)
                --bracketCount;
        }
    }
}

void BFInterpreter::endLoop()
{
    if (d_array[d_arrayPointer] != 0)
    {
        d_codePointer = d_loopStack.top();
    }
    else
    {
        d_loopStack.pop();
    }
}

void BFInterpreter::print(std::ostream& out)
{
    out << (char)d_array[d_arrayPointer] << std::flush;
}

void BFInterpreter::printCurses()
{
    assert(false && "printCurses() called but not compiled with USE_CURSES");
}

void BFInterpreter::handleAnsi(std::string& ansiStr, bool const force)
{
    assert(false && "handleAnsi called without USE_CURSES defined");
}

void BFInterpreter::read(std::istream& in)
{
    char c;
    in.get(c);
    d_array[d_arrayPointer] = c;
}

void BFInterpreter::readCurses()
{
    assert(false && "readCurses() called but not compiled with USE_CURSES");
}

void BFInterpreter::random()
{
    auto val                = d_uniformDist(d_rng);
    d_array[d_arrayPointer] = val;
}

void BFInterpreter::printState()
{
    for (auto x : d_array)
        std::cout << (int)x << ' ';
    std::cout << '\n';
}

void BFInterpreter::finish(int sig)
{
    assert(false && "finish() called but not compiled with USE_CURSES");
}
