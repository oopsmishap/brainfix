// Generated by Flexc++ V2.11.02 on Fri, 17 Jan 2025 17:33:03 +0000

#ifndef Lexer_H_INCLUDED_
#define Lexer_H_INCLUDED_

#include <stack>

// $insert baseclass_h
#include "../autogen/lexerbase.h"

#include "../autogen/compilerbase.h"

// $insert classHead
// $insert classHead
class Lexer : public LexerBase
{
    int                         d_nestedCommentLevel{0};
    std::stack<StartCondition_> d_startConditionStack;

  public:
    Lexer(std::string const& infile, std::string const& outfile);
    using LexerBase::pushStream;

    // $insert lexFunctionDecl
    int lex();

  private:
    int lex_();
    int executeAction_(size_t ruleNr);

    void print();
    void preCode(); // re-implement this function for code that must
    // be exec'ed before the patternmatching starts

    void postCode(PostEnum_ type);
    // re-implement this function for code that must
    // be exec'ed after the rules's actions.

    void pushStartCondition(StartCondition_ next);
    void popStartCondition();

    static char escape(char c);
    static char escapeTestContent(std::string const& matched);
};

// $insert inlineLexFunction
inline int Lexer::lex()
{
    return lex_();
}

inline void Lexer::preCode()
{
    // optionally replace by your own code
}

inline void Lexer::postCode([[maybe_unused]] PostEnum_ type)
{
    // optionally replace by your own code
}

inline void Lexer::print()
{
    print_();
}

#endif // Lexer_H_INCLUDED_
