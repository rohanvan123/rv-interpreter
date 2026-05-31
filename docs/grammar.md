# RV Grammar

This document describes the practical grammar accepted by the current RV parser. The implementation is split across the top-level parser and the arithmetic parser:

- `Parser` handles statements, blocks, declarations, assignments, functions, `if`, and `while`.
- `ArithmeticParser` handles expressions, operator precedence, literals, calls, lists, and indexing.

The grammar below is descriptive rather than generated from a formal parser specification.

## Program

```ebnf
Program ::= Statement* EOF
```

RV programs are stored in `.rv` files.

## Statements

```ebnf
Statement ::=
      LetDeclaration
    | Reassignment
    | CompoundAssignment
    | IndexedAssignment
    | IfStatement
    | WhileStatement
    | FunctionDeclaration
    | ReturnStatement
    | ExpressionStatement
```

## Declarations And Assignment

```ebnf
LetDeclaration ::= "let" Identifier "=" Expression ";"

Reassignment ::= Identifier "=" Expression ";"

CompoundAssignment ::=
    Identifier CompoundAssignOperator Expression ";"

CompoundAssignOperator ::= "+=" | "-=" | "*=" | "/=" | "%="

IndexedAssignment ::=
    Identifier IndexSuffix+ "=" Expression ";"

IndexSuffix ::= "[" Expression "]"
```

Compound assignment is parsed into a normal reassignment with a binary expression on the right side.

Example:

```rv
x += 1;
```

is represented like:

```rv
x = x + 1;
```

## Control Flow

```ebnf
IfStatement ::=
    "if" Condition Block ElseClause?

ElseClause ::= "else" Block

WhileStatement ::=
    "while" Condition Block

Condition ::= "(" Expression ")"

Block ::= "{" Statement* "}"
```

Examples:

```rv
if (x > 0) {
    print(x);
} else {
    print(0);
}

while (i < 10) {
    i += 1;
}
```

## Functions

```ebnf
FunctionDeclaration ::=
    "function" Identifier "(" ParameterList? ")" Block

ParameterList ::=
    Identifier ("," Identifier)*

ReturnStatement ::=
      "return" Expression ";"
    | "return" ";"
```

Example:

```rv
function add(a, b) {
    return a + b;
}
```

## Expressions

The expression grammar is organized by precedence, from lowest precedence to highest precedence.

```ebnf
Expression ::= Disjunction

Disjunction ::=
    Conjunction ("||" Conjunction)*

Conjunction ::=
    Comparison ("&&" Comparison)*

Comparison ::=
    Term (ComparisonOperator Term)*

ComparisonOperator ::=
      ">"
    | ">="
    | "<"
    | "<="
    | "!="
    | "=="

Term ::=
    Factor (("+" | "-") Factor)*

Factor ::=
    Exponent (("*" | "/" | "%") Exponent)*

Exponent ::=
    Unary ("^" Unary)*

Unary ::=
      UnaryOperator Unary
    | Atomic

UnaryOperator ::= "print" | "-" | "size" | "!"
```

## Atomic Expressions

```ebnf
Atomic ::=
      Integer
    | Boolean
    | String
    | IdentifierOrCallOrAccess
    | ListLiteral
    | "(" Expression ")"

IdentifierOrCallOrAccess ::=
    Identifier CallSuffix?
  | Identifier IndexSuffix+

CallSuffix ::=
    "(" ArgumentList? ")"

ArgumentList ::=
    Expression ("," Expression)*

ListLiteral ::=
    "[" ElementList? "]"

ElementList ::=
    Expression ("," Expression)*
```

Examples:

```rv
123
true
"hello"
x
add(1, 2)
values[0]
matrix[0][1]
[1, 2, 3]
(1 + 2) * 3
```

## Lexical Forms

```ebnf
Identifier ::= LowercaseLetter (Letter | Digit | "_")*

Integer ::= Digit+

Boolean ::= "true" | "false"

String ::= '"' Character* '"'
```

Identifiers currently begin with a lowercase letter.

The lexer also expands compact unary forms:

```rv
-123
!ready
```

into token sequences equivalent to:

```rv
- 123
! ready
```

## Notes On The Implementation

- The top-level parser slices token ranges and delegates expression parsing to `ArithmeticParser`.
- Conditions are parsed as parenthesized expressions.
- Function declarations are represented by `FunctionAssignmentExpression`.
- Function calls are represented by `FunctionCallExpression`.
- List and string indexing share `ListAccessExpression`.
- Indexed mutation uses `ListModifyExpression`, including nested mutations such as `matrix[0][1] = 9;`.
- The grammar is permissive in places because syntax errors are not yet reported through a structured diagnostic system.
