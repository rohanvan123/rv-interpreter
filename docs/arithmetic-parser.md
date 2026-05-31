# Arithmetic Parser

The arithmetic parser handles RV expressions and operator precedence. Despite the name, it parses more than arithmetic: booleans, comparisons, function calls, list literals, list/string indexing, unary operations, and parenthesized expressions.

Implementation:

- `includes/arithmetic_parser.hpp`
- `src/arithmetic_parser.cpp`

## Input And Output

Input:

```text
std::vector<Token>
```

Output:

```text
Expression*
```

The output is an AST subtree rooted at an `Expression` subclass.

## Recursive Descent Structure

The parser implements precedence through a recursive descent chain:

```text
expression
  disjunction       ||
  conjunction       &&
  comparison        > >= < <= != ==
  term              + -
  factor            * / %
  exponent          ^
  unary             print - size !
  atomic            literals, variables, calls, lists, indexing, groups
```

Higher-level methods call lower-precedence methods and fold repeated operators into `BinaryExpression` nodes.

## Atomic Expressions

Supported atomic forms:

```rv
false
true
123
"hello"
name
name(arg1, arg2)
name[index]
[1, 2, 3]
(1 + 2)
```

Function calls become:

```text
FunctionCallExpression(name, arg_expressions)
```

List literals become:

```text
ListExpression(elements)
```

Indexing becomes:

```text
ListAccessExpression(target, index)
```

Nested indexing is represented as nested `ListAccessExpression` nodes.

## Unary Expressions

Unary operators are parsed recursively:

```rv
print x
-x
size x
!ready
```

They become `MonadicExpression` nodes with one of:

- `PrintOp`
- `IntNegOp`
- `SizeOp`
- `NotOp`

## Binary Expressions

Binary operators become `BinaryExpression` nodes with the matching `BinaryOperator`.

Examples:

```rv
1 + 2 * 3
```

parses as:

```text
IntPlusOp(
  1,
  IntTimesOp(2, 3)
)
```

```rv
a <= b
```

parses as:

```text
LteOp(a, b)
```

## Design Notes

- The parser consumes tokens with `current`, `peek`, `advance`, and `match`.
- The `match` helper accepts a variable number of token types.
- The parser returns raw `Expression*` nodes. Ownership is passed upward to the top-level parser and eventually cleaned up after execution.
- Error handling is intentionally lightweight today; malformed expressions may return `nullptr` or throw generic runtime errors.
