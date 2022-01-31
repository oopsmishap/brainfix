# Brainfix v2.0
This is a reboot of a little project I did as a student. BrainFix is a compiler that takes a C-style
language (with elements taken from other languages or designed to match the needs) and compiles this
into [BrainF*ck](https://en.wikipedia.org/wiki/Brainfuck), an esoteric programming language consisting of only 8 operations.

## Bisonc++ and Flexc++
The lexical scanner and parser that are used to parse the Brainfix language are generated by [Bisonc++](https://fbb-git.gitlab.io/bisoncpp/)
and [Flexc++](https://fbb-git.gitlab.io/flexcpp/) (not to be confused with bison++ and flex++). This software is readily available in the
Debian repositories, but not necessarily needed to build this project. The sourcefiles generated by these programs based on the
current grammar and lexer specifications are included in the source-folder.

## Building
To build the project, simply cd into the src folder and call make:

```
cd src
make
```

To let bisonc++ and flexc++ regenerate the sourcecode for the scanner and parser, do

```
make regenerate
make
```

To run the resulting Brainf*ck code, you can use any third party utility that does the job. However, a simple
interpreter is included in the project, which you can build by running

```
make interpreter
```

## Usage
Building the project results in a executable called `bfx`. It takes either one or two arguments: the path to the
BrainFix file (usually .bfx) and an optional second path to the output file containing the generated BrainF*ck (usually .bf). When only one argument is provided, the BF output will be directed to stdout.

```
./bfx path/to/file.bfx path/to/output.bf
./bfx path/to/file.bfx > output.bf
```

To run the resulting BF, call the included interpreter or any other utility that was designed to run or compile
BF. The included interpreter takes 2 arguments: the size of the machine (number of bytes, canonically at least 30000)
and the input-file. For example:

```
./bfint 30000 output.bf
```

### Example: Hello World
Every programming language tutorial starts with a "Hello, World!" program of some sort. This is no exception:

```javascript
// File: hello.bfx
include "std/io.bfx"

function main()
{
    println("Hello, World!");
}
```

The program starts with an end-of-line comment (C-comment blocks between `/*` and `*/` are also allowed) and then
includes the IO-library which is included with this project. This exposes some basic IO-facilities the sourcefile.

Next, the main-function is defined. Every valid BFX-program should contain a `main` function which takes no arguments
and does not return anything. The order in which the functions are defined in a BFX-file does not matter; the compiler
will always try to find main and use this as the entrypoint.

In `main()`, the function `println()` from the IO library is called to print the argument and a newline to the console.
Let's try:

```
$ ./bfx hello.bfx hello.bf
$ ./bfint 30000 hello.bf
$ Hello, World!
$
```

## Language
### Functions
A BrainFix program consists of a series of functions (one of which is called `main()`). Apart from global variable declarations, `const` declarations and file inclusion (more on those later), no other syntax is allowed at global scope. In other words: BF code is only generated in function-bodies.

A function without a return-value is defined like we saw in the Hello World example and may take any number of parameters. For example:

```javascript
function foo(x, y, z)
{
    // body
}
```

When a function has a return-value, the syntax becomes:

```javascript
function ret = bar(x, y, z)
{
    // body --> must contain instantiation of 'ret' !
}
```

It does not matter where a function is defined with respect to the call:
```javascript
function foo()
{
    x = 31;
    y = 38;

    nice = bar(x, y); // works, even if bar is defined below
}

function z = bar(x, y)
{
    z = x + y; // return variable is instantiated here
}
```

### Operators
The following operators are supported by BrainFix:

| Operator | Description |
| --- | --- |
| `++`  |  post- and pre-increment |
| `--`   |  post- and pre-decrement |
| `+`    |  add |
| `-`    |  subtract |
| `*`    |  multiply |
| `/`    |  divide |
| `%`    |  modulo |
| `+=`   |  add to left-hand-side (lhs), returns lhs |
| `-=`   |  subtract from lhs, returns lhs |
| `*=`   |  multiply lhs by rhs, returns lhs |
| `/=`   |  divide lhs by rhs, returns lhs |
| `%=`   |  assigns the remainder of lhs / rhs and assigns it to lhs |
| `/=%`  |  divides lhs by rhs, returns the remainder |
| `%=/`  |  assignes the remainder to lhs, returns the result of the division |
| `&&`   |  logical AND |
| `\|\|` |  logical OR |
| `!`    |  (unary) logical NOT |
| `==`   |  equal to |
| `!=`   |  not equal to |
| `<`    |  less than |
| `>`    |  greater than |
| `<=`   |  less than or equal to |
| `>=`   |  greater than or equal to |

#### The div-mod and mod-div operators
Most of these operators are commonplace and use well known notation. The exception might be the div-mod and mod-div operators, which were added as a small optimizing feature. The BF-algorithm that is implemented to execute a division, calculates the remainder in the process. These operators reflect this fact, and let you collect both results in a single operation.

```javascript
function divModExample()
{
    x = 42;
    y = 5;

    z = (x /=% y);

    // x -> x / y (8) and
    // z -> x % y (2)
}

function modDivExample()
{
    x = 42;
    y = 5;

    z = (x %=/ y);
	
    // x -> x % y (2) and
    // z -> x / y (8)
}
```

### Arrays and Strings
#### Creating Arrays
BrainFix supports arrays of up to 250 elements. This upper limit exists due to the fact that the maximum value of an index is 255 (0xff), the fact that some algorithms need some additional cells to work and 250 is a nice and easy to remember number. For the compiler, there's no real difference between regular variables and arrays; variables are simply arrays of size 1. To declare and initialize an array the BrainFix language, one can use a number of different constructs:

```javascript
// Anonymous array from initializer list
#(1, 2, 3, 4, 5);

// Anonymous array from a given size, zero initialized
#[5]

// Anonymous array from a given size, value initialized
#[5, value]

// Declare x as an array of 5 elements, do not initialize
[5]x;

// Declare x as an array of 5 elements, then initialized all elements to 1
[5]x = 1;

// Use a placeholder for the size
[]x = #(1, 2, 3, 4, 5, 6, 7);
[]x = #[10, 42];

// Initialize with a string
[]str = "Hello World\n";

```

Once an array has a definite size, only arrays of the same size or variables of size 1 can be assigned to it. When assigning a different array of the same size, all elements are copied. When assigning a single element, this value is copied into every element of the array.

```javascript
function main()
{
    [5]x = 1;     // all 1's
    x = 4;        // x now contains only five 4's

    x = "Hello";         // fine
    x = "Hello World";   // ERROR: different sizes
}
```

Especially when storing a string in some variable, it is recommended to use the placeholder notation and let the compiler figure out the size of the string. Keep in mind that once the string has been stored, you can only store same-sized strings into the same variable.

```javascript
function main()
{
    [12]str1 = "Hello World\n";     // Inconvenient
    []str2   = "Hello World\n";     // Easy
}
```
The size of an array has to be known at compiletime and may not be given by a runtime variable, not even if the value of this variable is strictly known at compiletime (BrainFix does not do any compiletime processing).

#### Indexing
Once an array is created, it can be indexed using the familiar index-operator `[]`. Elements can be both accessed and changed using this operator:

```javascript
function main()
{
    []arr = #(42, 69, 123);

	++arr[0];    // 42  -> 43
    --arr[1];    // 69  -> 68
	arr[2] = 0;  // 123 -> 0
}

```

### Constants
BrainFix provides a simple way to define constants in your program, using the `const` keyword. `const` declarations can only appear at global scope. Throughout the program, occurrences of the variable are replaced at compiletime by the literal value they've been assigned. This means that `const` variables can be used as array-sizes (which is their most common usecase):

```javascript
const SIZE = 10;

function main()

    [SIZE]arr1 = #[SIZE, 42]; 
	[SIZE]arr2 = #[SIZE, 69];

	arr1 = arr2; // guaranteed to work, sizes will always match
}
```
