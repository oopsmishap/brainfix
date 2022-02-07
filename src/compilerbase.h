// Generated by Bisonc++ V6.03.00 on Mon, 07 Feb 2022 15:43:37 +0100

// hdr/includes
#ifndef CompilerBase_h_included
#define CompilerBase_h_included

#include <exception>
#include <vector>
#include <iostream>
// $insert polyincludes
#include <memory>
// $insert preincludes
#include "bfxfunction.h"

// hdr/baseclass

namespace // anonymous
{
    struct PI_;
}


// $insert polymorphic
enum class Tag_
{
    INSTRUCTION_PAIR,
    BFX_FUNCTION,
    INSTRUCTION_LIST,
    PARAMETER,
    INSTRUCTION,
    DECLARATION,
    STRING,
    CHAR,
    CASE_LIST,
    PARAMETER_LIST,
    DECLARATION_LIST,
    INT,
};

namespace Meta_
{

extern size_t const *t_nErrors;

extern size_t const *s_nErrors_;

template <Tag_ tag>
struct TypeOf;

template <typename Tp_>
struct TagOf;

// $insert polymorphicSpecializations
enum { sizeofTag_ = 12 };

extern char const *idOfTag_[];
template <>
struct TagOf<std::pair<Instruction, Instruction>>
{
    static Tag_ const tag = Tag_::INSTRUCTION_PAIR;
};

template <>
struct TagOf<BFXFunction>
{
    static Tag_ const tag = Tag_::BFX_FUNCTION;
};

template <>
struct TagOf<std::vector<Instruction>>
{
    static Tag_ const tag = Tag_::INSTRUCTION_LIST;
};

template <>
struct TagOf<BFXFunction::Parameter>
{
    static Tag_ const tag = Tag_::PARAMETER;
};

template <>
struct TagOf<Instruction>
{
    static Tag_ const tag = Tag_::INSTRUCTION;
};

template <>
struct TagOf<std::pair<std::string, int>>
{
    static Tag_ const tag = Tag_::DECLARATION;
};

template <>
struct TagOf<std::string>
{
    static Tag_ const tag = Tag_::STRING;
};

template <>
struct TagOf<char>
{
    static Tag_ const tag = Tag_::CHAR;
};

template <>
struct TagOf<std::vector<std::pair<Instruction, Instruction>>>
{
    static Tag_ const tag = Tag_::CASE_LIST;
};

template <>
struct TagOf<std::vector<BFXFunction::Parameter>>
{
    static Tag_ const tag = Tag_::PARAMETER_LIST;
};

template <>
struct TagOf<std::vector<std::pair<std::string, int>>>
{
    static Tag_ const tag = Tag_::DECLARATION_LIST;
};

template <>
struct TagOf<int>
{
    static Tag_ const tag = Tag_::INT;
};

template <>
struct TypeOf<Tag_::INSTRUCTION_PAIR>
{
    typedef std::pair<Instruction, Instruction> type;
};

template <>
struct TypeOf<Tag_::BFX_FUNCTION>
{
    typedef BFXFunction type;
};

template <>
struct TypeOf<Tag_::INSTRUCTION_LIST>
{
    typedef std::vector<Instruction> type;
};

template <>
struct TypeOf<Tag_::PARAMETER>
{
    typedef BFXFunction::Parameter type;
};

template <>
struct TypeOf<Tag_::INSTRUCTION>
{
    typedef Instruction type;
};

template <>
struct TypeOf<Tag_::DECLARATION>
{
    typedef std::pair<std::string, int> type;
};

template <>
struct TypeOf<Tag_::STRING>
{
    typedef std::string type;
};

template <>
struct TypeOf<Tag_::CHAR>
{
    typedef char type;
};

template <>
struct TypeOf<Tag_::CASE_LIST>
{
    typedef std::vector<std::pair<Instruction, Instruction>> type;
};

template <>
struct TypeOf<Tag_::PARAMETER_LIST>
{
    typedef std::vector<BFXFunction::Parameter> type;
};

template <>
struct TypeOf<Tag_::DECLARATION_LIST>
{
    typedef std::vector<std::pair<std::string, int>> type;
};

template <>
struct TypeOf<Tag_::INT>
{
    typedef int type;
};


    // Individual semantic value classes are derived from Base, offering a
    // member returning the value's Tag_, a member cloning the object of its
    // derived Semantic<Tag_> and a member returning a pointerr to its
    // derived Semantic<Tag_> data. See also Bisonc++'s distribution file
    // README.polymorphic-techical
class Base
{
    protected:
        Tag_ d_baseTag;        // d_baseTag is assigned by Semantic.

    public:
        Base() = default;
        Base(Base const &other) = delete;

        virtual ~Base();

        Tag_ tag() const;
        Base *clone() const;
        void *data() const;        

    private:
        virtual Base *vClone() const = 0;
        virtual void *vData() const = 0;
};

inline Base *Base::clone() const
{
    return vClone();
}

inline void *Base::data() const
{
    return vData();
}

inline Tag_ Base::tag() const
{
    return d_baseTag;
}

    // The class Semantic stores a semantic value of the type matching tg_
template <Tag_ tg_>
class Semantic: public Base
{
    typename TypeOf<tg_>::type d_data;
    
    public:
        Semantic();
        Semantic(Semantic<tg_> const &other);   // req'd for cloning

            // This constructor member template forwards its arguments to
            // d_data, allowing it to be initialized using whatever
            // constructor is available for DataType
        template <typename ...Params>
        Semantic(Params &&...params);

    private:
        Base *vClone() const override;
        void *vData() const override;
};

template <Tag_ tg_>
Semantic<tg_>::Semantic()
{
    d_baseTag = tg_;                // Base's data member:
}

template <Tag_ tg_>
Semantic<tg_>::Semantic(Semantic<tg_> const &other)
:
    d_data(other.d_data)
{
    d_baseTag = other.d_baseTag;
}

template <Tag_ tg_>
template <typename ...Params>
Semantic<tg_>::Semantic(Params &&...params)
:
    d_data(std::forward<Params>(params) ...)
{
    d_baseTag = tg_;
}


template <Tag_ tg_>
Base *Semantic<tg_>::vClone() const
{
    return new Semantic<tg_>{*this};
}

template <Tag_ tg_>
void *Semantic<tg_>::vData() const 
{
    return const_cast<typename TypeOf<tg_>::type *>(&d_data);
}


    // The class SType wraps a pointer to Base.  It becomes the polymorphic
    // STYPE_ type. It also defines get members, allowing constructions like
    // $$.get<INT> to be used.  
class SType: private std::unique_ptr<Base>
{
    typedef std::unique_ptr<Base> BasePtr;

    public:
        SType() = default;
        SType(SType const &other);
        SType(SType &&tmp);

        ~SType() = default;

            // Specific overloads are needed for SType = SType assignments
        SType &operator=(SType const &rhs);
        SType &operator=(SType &rhs);           // required so it is used
                                                // instead of the template op=
        SType &operator=(SType &&tmp);

// $insert polymorphicOpAssignDecl
        SType &operator=(std::pair<Instruction, Instruction> const &value);
        SType &operator=(std::pair<Instruction, Instruction> &&tmp);

        SType &operator=(BFXFunction const &value);
        SType &operator=(BFXFunction &&tmp);

        SType &operator=(std::vector<Instruction> const &value);
        SType &operator=(std::vector<Instruction> &&tmp);

        SType &operator=(BFXFunction::Parameter const &value);
        SType &operator=(BFXFunction::Parameter &&tmp);

        SType &operator=(Instruction const &value);
        SType &operator=(Instruction &&tmp);

        SType &operator=(std::pair<std::string, int> const &value);
        SType &operator=(std::pair<std::string, int> &&tmp);

        SType &operator=(std::string const &value);
        SType &operator=(std::string &&tmp);

        SType &operator=(char const &value);
        SType &operator=(char &&tmp);

        SType &operator=(std::vector<std::pair<Instruction, Instruction>> const &value);
        SType &operator=(std::vector<std::pair<Instruction, Instruction>> &&tmp);

        SType &operator=(std::vector<BFXFunction::Parameter> const &value);
        SType &operator=(std::vector<BFXFunction::Parameter> &&tmp);

        SType &operator=(std::vector<std::pair<std::string, int>> const &value);
        SType &operator=(std::vector<std::pair<std::string, int>> &&tmp);

        SType &operator=(int const &value);
        SType &operator=(int &&tmp);

        template <Tag_ tagParam, typename ...Args>
        void assign(Args &&...args);
    
            // By default the get()-members check whether the specified <tag>
            // matches the tag returned by SType::tag (d_data's tag). If they
            // don't match a run-time fatal error results.
        template <Tag_ tag>
        typename TypeOf<tag>::type &get();

        template <Tag_ tag>
        typename TypeOf<tag>::type const &get() const;

        Tag_ tag() const;
        bool valid() const;
};

inline SType::SType(SType const &other)
:
    BasePtr{other ? other->clone() : 0}
{}

inline SType::SType(SType &&tmp)
:
    BasePtr{std::move(tmp)}
{}

inline SType &SType::operator=(SType const &rhs)
{
    reset(rhs->clone());
    return *this;
}

inline SType &SType::operator=(SType &rhs)
{
    reset(rhs->clone());
    return *this;
}

inline SType &SType::operator=(SType &&tmp)
{
    BasePtr::operator=(std::move(tmp));
    return *this;
}

// $insert polymorphicOpAssignImpl
inline SType &SType::operator=(std::pair<Instruction, Instruction> const &value)
{
    assign< Tag_::INSTRUCTION_PAIR >(value);
    return *this;
}
inline SType &SType::operator=(std::pair<Instruction, Instruction> &&tmp)
{
    assign< Tag_::INSTRUCTION_PAIR >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(BFXFunction const &value)
{
    assign< Tag_::BFX_FUNCTION >(value);
    return *this;
}
inline SType &SType::operator=(BFXFunction &&tmp)
{
    assign< Tag_::BFX_FUNCTION >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(std::vector<Instruction> const &value)
{
    assign< Tag_::INSTRUCTION_LIST >(value);
    return *this;
}
inline SType &SType::operator=(std::vector<Instruction> &&tmp)
{
    assign< Tag_::INSTRUCTION_LIST >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(BFXFunction::Parameter const &value)
{
    assign< Tag_::PARAMETER >(value);
    return *this;
}
inline SType &SType::operator=(BFXFunction::Parameter &&tmp)
{
    assign< Tag_::PARAMETER >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(Instruction const &value)
{
    assign< Tag_::INSTRUCTION >(value);
    return *this;
}
inline SType &SType::operator=(Instruction &&tmp)
{
    assign< Tag_::INSTRUCTION >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(std::pair<std::string, int> const &value)
{
    assign< Tag_::DECLARATION >(value);
    return *this;
}
inline SType &SType::operator=(std::pair<std::string, int> &&tmp)
{
    assign< Tag_::DECLARATION >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(std::string const &value)
{
    assign< Tag_::STRING >(value);
    return *this;
}
inline SType &SType::operator=(std::string &&tmp)
{
    assign< Tag_::STRING >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(char const &value)
{
    assign< Tag_::CHAR >(value);
    return *this;
}
inline SType &SType::operator=(char &&tmp)
{
    assign< Tag_::CHAR >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(std::vector<std::pair<Instruction, Instruction>> const &value)
{
    assign< Tag_::CASE_LIST >(value);
    return *this;
}
inline SType &SType::operator=(std::vector<std::pair<Instruction, Instruction>> &&tmp)
{
    assign< Tag_::CASE_LIST >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(std::vector<BFXFunction::Parameter> const &value)
{
    assign< Tag_::PARAMETER_LIST >(value);
    return *this;
}
inline SType &SType::operator=(std::vector<BFXFunction::Parameter> &&tmp)
{
    assign< Tag_::PARAMETER_LIST >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(std::vector<std::pair<std::string, int>> const &value)
{
    assign< Tag_::DECLARATION_LIST >(value);
    return *this;
}
inline SType &SType::operator=(std::vector<std::pair<std::string, int>> &&tmp)
{
    assign< Tag_::DECLARATION_LIST >(std::move(tmp));
    return *this;
}
inline SType &SType::operator=(int const &value)
{
    assign< Tag_::INT >(value);
    return *this;
}
inline SType &SType::operator=(int &&tmp)
{
    assign< Tag_::INT >(std::move(tmp));
    return *this;
}

template <Tag_ tagParam, typename ...Args>
void SType::assign(Args &&...args)
{
    reset(new Semantic<tagParam>(std::forward<Args>(args) ...));
}

template <Tag_ tg>
typename TypeOf<tg>::type &SType::get()
{
// $insert warnTagMismatches

    if (tag() != tg)
    {
        if (*t_nErrors != 0)
            const_cast<SType *>(this)->assign<tg>();
        else
        {
            std::cerr << "[Fatal] calling `.get<Tag_::" << 
                idOfTag_[static_cast<int>(tg)] << 
                ">()', but Tag " <<
                idOfTag_[static_cast<int>(tag())] << " is encountered. Try "
                "option --debug and call setDebug(Parser::ACTIONCASES)\n";
            throw 1;        // ABORTs
        }
    }

    return *static_cast<typename TypeOf<tg>::type *>( (*this)->data() );
}

template <Tag_ tg>
typename TypeOf<tg>::type const &SType::get() const
{
// $insert warnTagMismatches

    if (tag() != tg)
    {
        if (*t_nErrors != 0)
            const_cast<SType *>(this)->assign<tg>();
        else
        {
            std::cerr << "[Fatal] calling `.get<Tag_::" << 
                idOfTag_[static_cast<int>(tg)] << 
                ">()', but Tag " <<
                idOfTag_[static_cast<int>(tag())] << " is encountered. Try "
                "option --debug and call setDebug(Parser::ACTIONCASES)\n";
            throw 1;        // ABORTs
        }
    }

    return *static_cast<typename TypeOf<tg>::type *>( (*this)->data() );
}

inline Tag_ SType::tag() const
{
    return valid() ? (*this)->tag() : static_cast<Tag_>(sizeofTag_);
}

inline bool SType::valid() const
{
    return BasePtr::get() != 0;
}

}  // namespace Meta_

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
        LET = 257,
        FUNCTION,
        GLOBAL,
        INLINE_BF,
        MOVE_PTR,
        INCLUDE,
        SIZEOF,
        FOR,
        IF,
        WHILE,
        CONST,
        SWITCH,
        CASE,
        DEFAULT,
        then,
        ELSE,
        ADD,
        SUB,
        MUL,
        MOD,
        DIV,
        DIVMOD,
        MODDIV,
        OR,
        AND,
        EQ,
        NE,
        LE,
        GE,
        INC,
        DEC,
        unaryMinus,
        _var,
        _arr,
        IDENT,
        STR,
        NUM,
        CHR,
    };

// $insert STYPE
    typedef Meta_::SType STYPE_;


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



