// Generated by Bisonc++ V6.03.00 on Sun, 06 Feb 2022 11:15:45 +0100

// Include this file in the sources of the class Compiler.

// $insert class.h
#include "compiler.hpp"
#include <cmath>
#include <fstream>
#include <set>

inline void Compiler::print()
{
}

inline void Compiler::exceptionHandler(std::exception const& exc)
{
    throw; // re-implement to handle exceptions thrown by actions
}

// Add here includes that are only required for the compilation
// of Compiler's sources.

// UN-comment the next using-declaration if you want to use
// int Compiler's sources symbols from the namespace std without
// specifying std::

// using namespace std;

#define STRINGIFY(x)                    STRINGIFY2(x)
#define STRINGIFY2(x)                   #x
#define BFX_DEFAULT_INCLUDE_PATH_STRING STRINGIFY(BFX_DEFAULT_INCLUDE_PATH)