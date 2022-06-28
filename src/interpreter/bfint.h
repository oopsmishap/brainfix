#ifndef BFINT_H
#define BFINT_H

#include <vector>
#include <stack>
#include <random>
#include <iostream>

enum class CellType
    {
     INT8,
     INT16,
     INT32
    };

struct Options
{
    int          err{0};
    CellType     cellType{CellType::INT8};
    int          tapeLength{30000};
    std::string  bfFile;
    std::ostream *outStream{&std::cout};
    bool         randomEnabled{false};
    bool         randomWarningEnabled{true};
    bool         gamingMode{false};
};

class BFInterpreter
{
    std::vector<int> d_array;
    std::string d_code;
    size_t d_arrayPointer{0};
    size_t d_codePointer{0};
    std::stack<int> d_loopStack;

    using RngType = std::mt19937;
    std::uniform_int_distribution<RngType::result_type> d_uniformDist;
    RngType d_rng;

    // Options
    CellType const d_cellType;
    std::ostream &d_out;
    bool const d_randomEnabled{false};
    bool const d_randomWarningEnabled{true};
    bool const d_gamingMode{false};
    
    enum Ops: char
        {
         PLUS  = '+',
         MINUS = '-',
         LEFT  = '<',
         RIGHT = '>',
         START_LOOP = '[',
         END_LOOP = ']',
         PRINT = '.',
         READ = ',',
         RAND = '?',
        };

public:
    BFInterpreter(Options const &opt);
    void run();

private:
    int consume(Ops op);
    void plus();
    void minus();
    void pointerInc();
    void pointerDec();
    void startLoop();
    void endLoop();
    void print(std::ostream &out);
    void printCurses(std::ostream &out);
    void read();
    void readCurses();
    void random();
    void printState();
    void handleAnsi(std::string &ansiStr, bool const force);
    static void finish(int sig);
};



#endif
