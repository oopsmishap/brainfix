%class-name Compiler
%filenames compiler
%parsefun-source compiler_gen.cpp
%scanner scanner.h
%baseclass-preinclude "parser_types.hpp"
%tag-mismatches on

%polymorphic    INT:                    int;
                CHAR:                   char;
                STRING:                 std::string;
                STRING_LIST:            std::vector<std::string>;
                STRING_TUPLE:           std::tuple<std::string, std::string, std::string>;
                INSTRUCTION:            Instruction;
                INSTRUCTION_LIST:       std::vector<Instruction>;
                DECLARATION:            std::pair<std::string, TypeSystem::Type>;
                DECLARATION_LIST:       std::vector<std::pair<std::string, TypeSystem::Type>>;
                PARAMETER:              BFXFunction::Parameter;
                PARAMETER_LIST:         std::vector<BFXFunction::Parameter>;
                INSTRUCTION_PAIR:       std::pair<Instruction, Instruction>;
                CASE_LIST:              std::vector<std::pair<Instruction, Instruction>>;
                BFX_FUNCTION:           BFXFunction;
                TEST_VECTOR:            std::vector<std::tuple<std::string, std::string, std::string>>;

// Keywords
%token LET FUNCTION GLOBAL INCLUDE SCAN PRINT RAND
%token SIZEOF FOR IF WHILE CONST SWITCH CASE DEFAULT STRUCT BREAK CONTINUE RETURN ASSERT
%token START_TEST END_TEST INPUT EXPECT END_TEST_CONTENT

%nonassoc then
%nonassoc ELSE
%nonassoc _var
%nonassoc _arr

// Operators
%right '=' ADD SUB MUL MOD DIV DIVMOD MODDIV POW
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%left '^'
%right '!' unaryMinus
%left INC DEC
%left '['
%left '.'

%token <STRING> IDENT
%token <STRING> STR
%token <INT>    NUM
%token <INT>    HEX
%token <CHAR>   CHR

%type <INSTRUCTION>         expression
%type <INSTRUCTION>         statement
%type <INSTRUCTION>         statement_
%type <INSTRUCTION>         builtin_function
%type <INSTRUCTION>         if_statement
%type <INSTRUCTION>         for_statement
%type <INSTRUCTION>         while_statement
%type <INSTRUCTION>         switch_statement
%type <INSTRUCTION>         break_statement
%type <INSTRUCTION>         continue_statement
%type <INSTRUCTION>         return_statement
%type <INSTRUCTION>         static_assert
%type <CASE_LIST>           case_list
%type <INSTRUCTION_PAIR>    switch_case
%type <INSTRUCTION>         default_case
%type <INSTRUCTION>         variable
%type <DECLARATION>         variable_declaration
%type <DECLARATION>         variable_specification
%type <DECLARATION_LIST>    variable_specification_list
%type <DECLARATION_LIST>    struct_body
%type <INSTRUCTION_PAIR>    array_element
%type <INSTRUCTION>         constant_literal
%type <INSTRUCTION>         string_literal
%type <INSTRUCTION>         anonymous_array
%type <INSTRUCTION>         anonymous_struct
%type <INSTRUCTION>         function_call
%type <INSTRUCTION>         scope_block
%type <INSTRUCTION>         code_body
%type <INSTRUCTION_LIST>    expression_list
%type <INSTRUCTION_LIST>    optional_expression_list
%type <PARAMETER>           parameter
%type <PARAMETER_LIST>      parameter_list
%type <PARAMETER_LIST>      optional_parameter_list
%type <BFX_FUNCTION>        function_header
%type <STRING_LIST>         field
%type <STRING>              unit_test_content
%type <STRING>              unit_test_label
%type <STRING>              unit_test_input
%type <STRING>              unit_test_expect
%type <STRING_TUPLE>        unit_test_case
%type <TEST_VECTOR>         unit_test_body


%%

program:
    // empty
|
    program error
|
    program function_definition
|
    program global_variable_list
|
    program include_directive
|
    program const_definition
|
    program struct_definition
|
    program unit_test_block
;

include_directive:
    INCLUDE STR
    {
        pushStream($2);
    }
;

function_definition:
    function_header scope_block
    {
        addFunction(($1).setBody($2));
    }
;

function_header:
    FUNCTION IDENT '(' optional_parameter_list ')'
    {
        $$ = BFXFunction($2, $4);
    }
|
    FUNCTION IDENT '=' IDENT '(' optional_parameter_list ')'
    {
        $$ = BFXFunction($4, $6);
        ($$).setReturnVariable($2);
    }
;

const_definition:
    CONST IDENT '=' NUM ';'
    {
        addConstant($2, $4);
    }
|
    CONST IDENT '=' HEX ';'
    {
        addConstant($2, $4);
    }
|
    CONST IDENT '=' CHR ';'
    {
        addConstant($2, $4);
    }
;

struct_definition:
    STRUCT IDENT '{' struct_body '}'
    {
        addStruct($2, $4);
    }
|
    struct_definition ';'
;

struct_body:
    variable_specification_list ';'
|
    struct_body variable_specification_list ';'
    {
        ($1).insert($1.begin(), $2.begin(), $2.end());
        $$ = $1;
    }
;    


scope_block:
    '{' code_body '}'
    {
        $$ = $2;
    }
;

parameter:
    IDENT
    {
        $$ = BFXFunction::Parameter(
             $1,
             BFXFunction::ParameterType::Value
        );
    }
|
    '&' IDENT
    {
        $$ = BFXFunction::Parameter(
             $2,
             BFXFunction::ParameterType::Reference
        );
    }
;

parameter_list:
    parameter
    {
        auto ret = std::vector<BFXFunction::Parameter>();
        ret.push_back($1);
        $$ = ret;
    }
|
    parameter_list ',' parameter
    {
        ($1).push_back($3);
        $$ = $1;
    }
;

optional_parameter_list:
    parameter_list
|
    // empty
    {
        $$ = std::vector<BFXFunction::Parameter>();
    }
;

global_variable_list:
    GLOBAL variable_specification_list ';'
    {
        addGlobals($2);
    }
;

code_body:
    {
        $$ = Instruction([](){ return -1; });
    }
|
    code_body statement
    {
        $$ = instruction<&Compiler::mergeInstructions>($1, $2);
    }
;

statement:
    statement_
    {
        $$ = instruction<&Compiler::statement>($1);
    }
;

statement_:
    error ';' // continue parsing at the next statement
|
    ';'
    {
        $$ = [](){ return -1; };
    }
|
    expression ';'
|
    static_assert ';'
|
    scope_block
|
    for_statement
|
    if_statement
|
    while_statement
|
    switch_statement
|
    break_statement 
|
    continue_statement
|
    return_statement
;

expression:
    variable_declaration
    {
        $$ = instruction<&Compiler::declareVariable>(($1).first, ($1).second);
    }
|    
    variable %prec _var
|
    '-' expression %prec unaryMinus
    {
        compilerError("Negative numbers are not supported.");
        $$ = []() { return -1; };
    }
|
    array_element %prec _arr
    {
        $$ = instruction<&Compiler::fetchElement>(
           ($1).first,
           ($1).second
        );
    }
|    
    constant_literal
|
    string_literal
|
    anonymous_array
|
    anonymous_struct
|
    function_call
|
    builtin_function
|
    SIZEOF '(' IDENT ')'
    {
        $$ = instruction<&Compiler::sizeOfOperator>($3);
    }
|
    variable_declaration '=' expression
    {
        $$ = instruction<&Compiler::initializeExpression>(
                ($1).first,
                ($1).second,
                $3
             );
    }
|
    variable '=' expression
    {
        $$ = instruction<&Compiler::assign>($1, $3);
    }
|
    array_element '=' expression
    {
        $$ = instruction<&Compiler::assignElement>(
           ($1).first,
           ($1).second,
           $3
        );
    }
|
    '(' expression ')'
    {
        $$ = $2;
    }
|
    INC variable
    {
        $$ = instruction<&Compiler::preIncrement>($2);
    }
|
    DEC variable
    {
        $$ = instruction<&Compiler::preDecrement>($2);
    }
|
    variable INC
    {
        $$ = instruction<&Compiler::postIncrement>($1);
    }
|
    variable DEC
    {
        $$ = instruction<&Compiler::postDecrement>($1);
    }
|
    INC array_element
    {
        $$ = instruction<&Compiler::applyUnaryFunctionToElement>(
                ($2).first,
                ($2).second,
                &Compiler::preIncrement
            );
    }
|
    DEC array_element
    {
        $$ = instruction<&Compiler::applyUnaryFunctionToElement>(
                ($2).first,
                ($2).second,
                &Compiler::preDecrement
            );
    }
|
    array_element INC
    {
        $$ = instruction<&Compiler::applyUnaryFunctionToElement>(
                ($1).first,
                ($1).second,
                &Compiler::postIncrement
            );
    }
|
    array_element DEC
    {
        $$ = instruction<&Compiler::applyUnaryFunctionToElement>(
                ($1).first,
                ($1).second,
                &Compiler::postDecrement
            );
    }
|
    variable ADD expression
    {
        $$ = instruction<&Compiler::addTo>($1, $3);
    }
|
    array_element ADD expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                &Compiler::addTo
           );
    }
|
    expression '+' expression
    {
        $$ = instruction<&Compiler::add>($1, $3);
    }
|
    variable SUB expression
    {
        $$ = instruction<&Compiler::subtractFrom>($1, $3);
    }
|
    array_element SUB expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                &Compiler::subtractFrom
           );
    }
|
    expression '-' expression
    {
        $$ = instruction<&Compiler::subtract>($1, $3);
    }
|
    variable MUL expression
    {
        $$ = instruction<&Compiler::multiplyBy>($1, $3);
    }
|
    array_element MUL expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                Compiler::BinaryFunction(&Compiler::multiplyBy)
           );
    }
|
    expression '*' expression
    {
        $$ = instruction<&Compiler::multiply>($1, $3);
    }
|
    variable DIV expression
    {
        $$ = instruction<&Compiler::divideBy>($1, $3);
    }
|
    array_element DIV expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                Compiler::BinaryFunction(&Compiler::divideBy)
            );
    }
|
    expression '/' expression
    {
        $$ = instruction<&Compiler::divide>($1, $3);
    }
|
    variable MOD expression
    {
        $$ = instruction<&Compiler::moduloBy>($1, $3);
    }
|
    array_element MOD expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                Compiler::BinaryFunction(&Compiler::moduloBy)
            );
    }
|
    variable DIVMOD expression
    {
        $$ = instruction<&Compiler::divMod>($1, $3);
    }
|
    array_element DIVMOD expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                Compiler::BinaryFunction(&Compiler::divMod)
            );
    }
|
    variable MODDIV expression
    {
        $$ = instruction<&Compiler::modDiv>($1, $3);
    }
|
    array_element MODDIV expression
    {
        $$ = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                Compiler::BinaryFunction(&Compiler::modDiv)
            );
    }
|
    expression '^' expression
    {
        $$ = instruction<&Compiler::power>($1, $3);
    }
|
    variable POW expression
    {
        $$ = instruction<&Compiler::powerBy>($1, $3);
    }
|
    array_element POW expression
    {
        $$  = instruction<&Compiler::applyBinaryFunctionToElement>(
                ($1).first,
                ($1).second,
                $3,
                Compiler::BinaryFunction(&Compiler::powerBy)
            ); 
    }
|
    expression '%' expression
    {
        $$ = instruction<&Compiler::modulo>($1, $3);
    }
|
    expression '<' expression
    {
        $$ = instruction<&Compiler::less>($1, $3);
    }
|
    expression '>' expression
    {
        $$ = instruction<&Compiler::greater>($1, $3);
    }
|
    expression LE expression
    {
        $$ = instruction<&Compiler::lessOrEqual>($1, $3);
    }
|
    expression GE expression
    {
        $$ = instruction<&Compiler::greaterOrEqual>($1, $3);
    }
|
    expression NE expression
    {
        $$ = instruction<&Compiler::notEqual>($1, $3);
    }
|
    expression EQ expression
    {
        $$ = instruction<&Compiler::equal>($1, $3);
    }
|
    '!' expression
    {
        $$ = instruction<&Compiler::logicalNot>($2);
    }
|
    expression AND expression
    {
        $$ = instruction<&Compiler::logicalAnd>($1, $3);
    }
|
    expression OR expression
    {
        $$ = instruction<&Compiler::logicalOr>($1, $3);
    }
;

builtin_function:
    PRINT '(' expression ')'
    {
        $$ = instruction<&Compiler::printCell>($3);
    }
|
    SCAN '(' ')'
    {
        $$ = instruction<&Compiler::scanCell>();
    }
|
    RAND '(' ')'
    {
        $$ = instruction<&Compiler::randomCell>();
    }
;

static_assert:
    ASSERT '(' expression ',' STR ')' 
    {
        $$ = instruction<&Compiler::staticAssert>($3, $5);
    }
;    

variable:
    IDENT 
    {
        $$ = instruction<&Compiler::fetch>($1);
    }
|
    field
    {
        $$ = instruction<&Compiler::fetchField>($1);
    }
;    

field:
    IDENT '.' IDENT
    {
        $$ = std::vector<std::string>{$1, $3};
    }
|
    field '.' IDENT
    {
        ($1).push_back($3);
        $$ = $1;
    }
;

constant_literal:
    NUM
    {
        $$ = instruction<&Compiler::constVal>($1);
    }
|
    HEX 
    {
        $$ = instruction<&Compiler::constVal>($1);
    }
|
    CHR
    {
        $$ = instruction<&Compiler::constVal>($1);
    }
;

array_element:
    expression '[' expression ']' 

    {
        $$ = std::pair<Instruction, Instruction>($1, $3);
    }
;


string_literal:
    STR
    {
        $$ = instruction<&Compiler::arrayFromString>($1);
    }
;

anonymous_array:
    '#' '{' expression_list '}'
    {
        $$ = instruction<&Compiler::arrayFromList>($3);
    }
|
    '#' '[' NUM ']'
    {
        $$ = instruction<&Compiler::arrayFromSize>(
                 $3,
                 instruction<&Compiler::constVal>(0)
            );
    }
|
    '#' '[' HEX ']'
    {
        $$ = instruction<&Compiler::arrayFromSize>(
                 $3,
                 instruction<&Compiler::constVal>(0)
            );
    }
|
    '#' '[' IDENT ']'
    {
        $$ = instruction<&Compiler::arrayFromSize>(
                compileTimeConstant($3),
                instruction<&Compiler::constVal>(0)
           );
    }
|
    '#' '[' NUM ',' expression ']'
    {
        $$ = instruction<&Compiler::arrayFromSize>($3, $5);
    }
|
    '#' '[' HEX ',' expression ']'
    {
        $$ = instruction<&Compiler::arrayFromSize>($3, $5);
    }
|
    '#' '[' IDENT ',' expression ']'
    {
        $$ = instruction<&Compiler::arrayFromSize>(
                compileTimeConstant($3),
                $5
           );
    }
;

anonymous_struct:
    IDENT '{' optional_expression_list '}'
    {
        $$ = instruction<&Compiler::anonymousStructObject>($1, $3);  
    }
;    

function_call:
    IDENT '(' optional_expression_list ')'
    {
       $$ = instruction<&Compiler::call>($1, $3);
    }
;

break_statement:
    BREAK ';'
    {
        $$ = instruction<&Compiler::breakStatement>();
    }
;

continue_statement:
    CONTINUE ';'
    {
        $$ = instruction<&Compiler::continueStatement>();
    }
;

return_statement:
    RETURN ';'
    {
        $$ = instruction<&Compiler::returnStatement>();
    }
;


for_statement:
    FOR '(' expression ';' expression ';' expression ')' statement
    {
        $$ = instruction<&Compiler::forStatement>($3, $5, $7, $9);
    }
|
    FOR '*' '(' expression ';' expression ';' expression ')' statement
    {
        $$ = instruction<&Compiler::forStatementRuntime>($4, $6, $8, $10);
    }
|
    FOR '(' LET parameter ':' expression ')' statement
    {
        $$ = instruction<&Compiler::forRangeStatement>($4, $6, $8);
    }
|
    FOR '*' '(' LET parameter ':' expression ')' statement
    {
        $$ = instruction<&Compiler::forRangeStatementRuntime>($5, $7, $9);
    }
;

if_statement:
    IF '(' expression ')' statement
    {
        $$ = instruction<&Compiler::ifStatement>($3, $5, Instruction([](){ return -1; }), true);
    }
|
    IF '(' expression ')' statement ELSE statement %prec then
    {
        $$ = instruction<&Compiler::ifStatement>($3, $5, $7, true);    
    }
;

while_statement:
    WHILE '(' expression ')' statement
    {
        $$ = instruction<&Compiler::whileStatement>($3, $5);
    }
|
    WHILE '*' '(' expression ')' statement
    {
        $$ = instruction<&Compiler::whileStatementRuntime>($4, $6);
    }   
;

switch_statement:
    SWITCH '(' expression ')' '{' case_list '}'
    {
        $$ = instruction<&Compiler::switchStatement>($3, $6, [](){ return -1; });
    }
|
    SWITCH '(' expression ')' '{' case_list default_case '}'
    {
        $$ = instruction<&Compiler::switchStatement>($3, $6, $7);
    } 

;

case_list:
    switch_case
    {
        $$ = std::vector<std::pair<Instruction, Instruction>>();
        ($$).push_back($1);
    }
|
    case_list switch_case
    {
        ($1).push_back($2);
        $$ = $1;
    }
;

switch_case:
    CASE expression ':' statement
    {
        $$ = std::make_pair($2, $4);
    }
;

default_case:
    DEFAULT ':' statement
    {
        $$ = $3;
    }
;    

variable_declaration:
    LET variable_specification
    {
        $$ = $2;
    }
;

variable_specification:
    IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $1,
            TypeSystem::Type(1)
        };
    }
|
    '&' IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $2,
            TypeSystem::Type{}
        };
    }   
|
    '[' ']' IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $3,
            TypeSystem::Type(-1)
        };
    }
|
    '[' NUM ']' IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $4,
            TypeSystem::Type($2)
        };
    }
|
    '[' HEX ']' IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $4,
            TypeSystem::Type($2)
        };
    }
|
    '[' IDENT ']' IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $4,
            TypeSystem::Type(compileTimeConstant($2))
        };
    }
|
    '[' STRUCT IDENT ']' IDENT
    {
        $$ = std::pair<std::string, TypeSystem::Type>
        {
            $5,
            TypeSystem::Type($3)
        };
    }
;    


variable_specification_list:
    variable_specification
    {   
        $$ = std::vector<std::pair<std::string, TypeSystem::Type>>();
        ($$).push_back($1);
    }
|
    variable_specification_list ',' variable_specification
    {
        ($1).push_back($3);
        $$ = $1;
    }
;


expression_list:
    expression
    {
        $$ = std::vector<Instruction>{};
        ($$).push_back($1);
    }
|
    expression_list ',' expression
    {
        ($1).push_back($3);
        $$ = $1;
    }
;

optional_expression_list:
    // empty
    {
        $$ = std::vector<Instruction>{};
    }
|
    expression_list
;


unit_test_block:
    START_TEST unit_test_label unit_test_body END_TEST
    {
        addTest($2, $3);
    }
;

unit_test_label:
    '<' IDENT '>'
    {
        $$ = $2;
    }
;

unit_test_body:
    unit_test_case
    {
        $$ = std::vector<std::tuple<std::string, std::string, std::string>>{};
        ($$).push_back($1);
    }
|
    unit_test_body unit_test_case
    {
        ($1).push_back($2);
        $$ = $1;
    }
;

unit_test_case:
    unit_test_label unit_test_input unit_test_expect
    {
        $$ = std::make_tuple($1, $2, $3);
    }
|
    unit_test_label unit_test_expect
    {
        $$ = std::make_tuple($1, std::string{}, $2);
    }
;

unit_test_input:
    INPUT unit_test_content END_TEST_CONTENT
    {
        $$ = $2;
    }
;

unit_test_expect:
    EXPECT unit_test_content END_TEST_CONTENT
    {
        $$ = $2;
    }
;

unit_test_content:
    //  empty
    {
        $$ = std::string("");
    }
|
    unit_test_content CHR
    {
        $$  = $1 + $2;
    }
;    
