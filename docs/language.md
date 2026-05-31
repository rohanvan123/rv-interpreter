# RV Language Reference

RV is a small Python-like language with explicit statement terminators, block-based control flow, first-class runtime values, lists, strings, functions, and builtins. RV programs are stored in `.rv` source files and executed by the RV interpreter CLI or backend.

The interpreter currently supports:

- Lexing `.rv` source into tokens
- Parsing top-level statements and nested expressions into an AST
- Direct AST execution with the legacy tree evaluator
- IR generation from AST expressions
- Bytecode-style execution of generated IR
- Debug output for lexer, parser, and IR stages
- A Flask backend for interpreter requests

## Program Structure

An RV program is a sequence of top-level expressions or statements.

```rv
let x = 10;
print(x);
```

Most statements end with `;`. Block constructs use braces:

```rv
if (x > 0) {
    print(x);
} else {
    print(0);
}
```

## Values

RV supports:

| Type | Example |
| --- | --- |
| Integer | `42` |
| Boolean | `true`, `false` |
| String | `"hello"` |
| List | `[1, 2, 3]` |

The runtime representation is implemented by `Value` in `includes/value.hpp` and `src/value.cpp`.

## Variables

Declare a variable with `let`:

```rv
let count = 0;
```

Reassign an existing variable:

```rv
count = count + 1;
```

Compound assignment is supported:

```rv
count += 1;
count -= 1;
count *= 2;
count /= 2;
count %= 2;
```

## Operators

Arithmetic:

```rv
1 + 2;
5 - 3;
4 * 2;
8 / 2;
7 % 3;
2 ^ 4;
```

Comparison and equality:

```rv
x > 0;
x >= 0;
x < 10;
x <= 10;
x == y;
x != y;
```

Boolean:

```rv
true && false;
true || false;
!true;
```

Unary:

```rv
-x;
print(x);
size(values);
```

## Operator Precedence

Expression precedence is implemented by the arithmetic parser:

1. Atomic expressions: literals, variables, calls, lists, indexing, parenthesized expressions
2. Unary operators: `print`, `-`, `size`, `!`
3. Exponentiation: `^`
4. Multiplication-style operators: `*`, `/`, `%`
5. Addition-style operators: `+`, `-`
6. Comparison and equality: `>`, `>=`, `<`, `<=`, `!=`, `==`
7. Boolean AND: `&&`
8. Boolean OR: `||`

Parentheses can override precedence:

```rv
print((1 + 2) * 3);
```

## Control Flow

`if` statements require a boolean condition:

```rv
if (x > 10) {
    print("large");
} else {
    print("small");
}
```

The `else` branch is optional:

```rv
if (ready) {
    print("go");
}
```

`while` loops repeatedly execute while the condition evaluates to `true`:

```rv
let i = 0;
while (i < 3) {
    print(i);
    i += 1;
}
```

## Functions

Functions are declared with `function`:

```rv
function add(a, b) {
    return a + b;
}

print(add(2, 3));
```

Functions can return a value:

```rv
return x;
```

They can also return without a value:

```rv
return;
```

Recursive calls are supported:

```rv
function fact(n) {
    if (n == 1) {
        return 1;
    }

    return n * fact(n - 1);
}
```

Function calls are lowered to IR jumps through the function table. Runtime function frames intentionally copy the caller frame, so functions can see caller state according to the current implementation.

## Lists And Strings

List literals:

```rv
let values = [1, 2, 3];
```

Indexing:

```rv
print(values[0]);
print("hello"[1]);
```

Mutation:

```rv
values[1] = 99;

let word = "cat";
word[0] = "b";
```

Nested list mutation is supported by parsing nested list-access expressions:

```rv
let matrix = [[1, 2], [3, 4]];
matrix[0][1] = 9;
```

## Builtins

RV has builtin functions:

| Builtin | Description |
| --- | --- |
| `append(list, value)` | Returns a new list with `value` appended. |
| `remove(list, index)` | Returns a new list with the element at `index` removed. |
| `type(value)` | Returns the RV type name as a string. |
| `string(value)` | Converts a value to a string representation. |

`size` is implemented as a unary operator rather than a builtin function:

```rv
print(size([1, 2, 3]));
```

## Example Program

```rv
function fib(n) {
    if (n <= 1) {
        return n;
    }

    return fib(n - 1) + fib(n - 2);
}

let results = [];
let i = 0;

while (i < 6) {
    results = append(results, fib(i));
    i += 1;
}

print(results);
```
