// Generated by Bisonc++ V6.03.00 on Mon, 07 Feb 2022 12:52:41 +0100

// hdr/includes
#ifndef CompilerBase_h_included
#define CompilerBase_h_included

#include <exception>
#include <vector>
#include <iostream>

// hdr/baseclass

namespace // anonymous
{
    struct PI_;
}



class CompilerBase
{
    public:
        enum DebugMode_
        {
            OFF           = 0,
            ON            = 1 << 0,
            ACTIONCASES   = 1 << 1
        };

// $insert tokens

    // Symbolic tokens:
    enum Tokens_
    {
        IDENT = 257,
        NUM,
    };

// $insert STYPE
typedef std::string STYPE_;


    private:
                        // state  semval
        typedef std::pair<size_t, STYPE_> StatePair;
                       // token   semval
        typedef std::pair<int,    STYPE_> TokenPair;

        int d_stackIdx = -1;
        std::vector<StatePair> d_stateStack;
        StatePair  *d_vsp = 0;       // points to the topmost value stack
        size_t      d_state = 0;

        TokenPair   d_next;
        int         d_token;

        bool        d_terminalToken = false;
        bool        d_recovery = false;


    protected:
        enum Return_
        {
            PARSE_ACCEPT_ = 0,   // values used as parse()'s return values
            PARSE_ABORT_  = 1
        };
        enum ErrorRecovery_
        {
            UNEXPECTED_TOKEN_,
        };

        bool        d_actionCases_ = false;    // set by options/directives
        bool        d_debug_ = true;
        size_t      d_requiredTokens_;
        size_t      d_nErrors_;                // initialized by clearin()
        size_t      d_acceptedTokens_;
        STYPE_     d_val_;


        CompilerBase();

        void ABORT() const;
        void ACCEPT() const;
        void ERROR() const;

        STYPE_ &vs_(int idx);             // value stack element idx 
        int  lookup_() const;
        int  savedToken_() const;
        int  token_() const;
        size_t stackSize_() const;
        size_t state_() const;
        size_t top_() const;
        void clearin_();
        void errorVerbose_();
        void lex_(int token);
        void popToken_();
        void pop_(size_t count = 1);
        void pushToken_(int token);
        void push_(size_t nextState);
        void redoToken_();
        bool recovery_() const;
        void reduce_(int rule);
        void shift_(int state);
        void startRecovery_();

    public:
        void setDebug(bool mode);
        void setDebug(DebugMode_ mode);
}; 

// hdr/abort
inline void CompilerBase::ABORT() const
{
    throw PARSE_ABORT_;
}

// hdr/accept
inline void CompilerBase::ACCEPT() const
{
    throw PARSE_ACCEPT_;
}


// hdr/error
inline void CompilerBase::ERROR() const
{
    throw UNEXPECTED_TOKEN_;
}

// hdr/savedtoken
inline int CompilerBase::savedToken_() const
{
    return d_next.first;
}

// hdr/opbitand
inline CompilerBase::DebugMode_ operator&(CompilerBase::DebugMode_ lhs,
                                     CompilerBase::DebugMode_ rhs)
{
    return static_cast<CompilerBase::DebugMode_>(
            static_cast<int>(lhs) & rhs);
}

// hdr/opbitor
inline CompilerBase::DebugMode_ operator|(CompilerBase::DebugMode_ lhs, 
                                     CompilerBase::DebugMode_ rhs)
{
    return static_cast<CompilerBase::DebugMode_>(static_cast<int>(lhs) | rhs);
};

// hdr/recovery
inline bool CompilerBase::recovery_() const
{
    return d_recovery;
}

// hdr/stacksize
inline size_t CompilerBase::stackSize_() const
{
    return d_stackIdx + 1;
}

// hdr/state
inline size_t CompilerBase::state_() const
{
    return d_state;
}

// hdr/token
inline int CompilerBase::token_() const
{
    return d_token;
}

// hdr/vs
inline CompilerBase::STYPE_ &CompilerBase::vs_(int idx) 
{
    return (d_vsp + idx)->second;
}

// hdr/tail
// For convenience, when including ParserBase.h its symbols are available as
// symbols in the class Parser, too.
#define Compiler CompilerBase


#endif


