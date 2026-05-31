# Lexer

The lexer converts RV source text into a sequence of `Token` objects.

Implementation:

- `includes/lexer.hpp`
- `src/lexer.cpp`
- `includes/token.hpp`

## Input And Output

Input:

```text
Raw RV source as a string
```

Output:

```text
std::vector<Token>
```

Each token contains:

- `TokenType type`
- `std::string lexeme`
- `int line`

The lexer always appends an `END_OF_FILE` token at the end of the stream.

## Token Categories

Single-character tokens are looked up from `Lexer::single_chars`:

```text
; ( ) { } , $ [ ]
```

Operator and keyword tokens are looked up from `Lexer::keyword_tokens`, including:

```text
let while print size if else function return
+ - * / % ^ = += -= *= /= %= == != > >= < <= && || !
true false
```

Literals:

- Integers such as `123`
- Strings such as `"hello"`
- Identifiers such as `count` or `my_var1`

## Lexing Flow

`Lexer::generate_tokens()` advances through `source` with a `current` index.

The high-level flow is:

1. Skip spaces and newlines.
2. Track source line numbers.
3. Parse quoted strings as `STRING`.
4. Parse known single-character tokens.
5. Otherwise, read a contiguous non-whitespace token fragment.
6. Classify the fragment as keyword/operator, integer, negative integer, identifier, negated identifier, or negative identifier.
7. Append `END_OF_FILE`.

## Negative And Negated Forms

The lexer expands compact unary forms into separate tokens:

```rv
-123
```

becomes:

```text
MINUS, INT 123
```

```rv
!ready
```

becomes:

```text
NOT, IDENT ready
```

This keeps unary expression handling in the arithmetic parser.

## Example

Source:

```rv
let x = 10;
print(x <= 20);
```

Token stream:

```text
LET, IDENT x, EQUALS, INT 10, SEMI
PRINT, LEFT_PAREN, IDENT x, LEQ, INT 20, RIGHT_PAREN, SEMI
EOF
```

## Design Notes

- The lexer is whitespace-oriented for multi-character operators and identifiers.
- Line numbers are preserved for debug output.
- Unknown fragments are currently ignored if they do not match any recognized token regex. Better diagnostics would be a future improvement.
