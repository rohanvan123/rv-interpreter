# Parser

The top-level parser builds AST expressions for RV statements and block constructs. It delegates expression precedence and atomic expression parsing to the arithmetic parser.

Implementation:

- `includes/parser.hpp`
- `src/parser.cpp`
- `includes/expression.hpp`

## Input And Output

Input:

```text
std::vector<Token>
```

Output:

```text
std::vector<Expression*>
```

Each `Expression*` is a heap-allocated AST node. The CLI later releases these nodes through `utils::cleanup_expressions`.

## Top-Level Parsing

`Parser::parse_top_level_expressions()` repeatedly calls `parse_expression()` until it reaches `END_OF_FILE`.

The parser recognizes:

- `let` declarations
- Reassignment
- Compound assignment
- List/string indexed assignment
- `if` / `else`
- `while`
- Function declarations
- Return statements
- Plain expressions delegated to `ArithmeticParser`

## Statement Forms

Variable declaration:

```rv
let x = 1 + 2;
```

AST:

```text
AssignmentExpression("x", BinaryExpression(...), reassignment=false)
```

Reassignment:

```rv
x = x + 1;
```

AST:

```text
AssignmentExpression("x", BinaryExpression(...), reassignment=true)
```

Compound assignment:

```rv
x += 1;
```

is parsed as a reassignment whose right side is equivalent to:

```rv
x + 1
```

Indexed assignment:

```rv
arr[0] = 99;
matrix[0][1] = 7;
```

is converted into `ListModifyExpression` nodes. Nested indexed assignment is represented by nested list access and modify expressions.

## Blocks

`if`:

```rv
if (condition) {
    print("yes");
} else {
    print("no");
}
```

produces:

```text
IfExpression(condition, if_body, else_body)
```

`while`:

```rv
while (condition) {
    body;
}
```

produces:

```text
WhileExpression(condition, body)
```

Function declaration:

```rv
function add(a, b) {
    return a + b;
}
```

produces:

```text
FunctionAssignmentExpression("add", ["a", "b"], body)
```

## Return Handling

The parser marks returnable expressions with `Expression::set_returnable(true)`.

Examples:

```rv
return 1;
return x + y;
return;
```

An empty return is represented by `EmptyExpression`.

## Delegation To ArithmeticParser

For simple expressions, conditions, assignment right-hand sides, function-call arguments, list elements, and index expressions, the parser slices a token range and delegates to `ArithmeticParser`.

This separation keeps top-level grammar handling separate from expression precedence.

## Design Notes

- The parser is a hand-written recursive parser over a token vector.
- It mutates an integer index by reference as it consumes tokens.
- It assumes source is structurally valid in many places. Invalid syntax can currently lead to generic runtime exceptions or null expressions rather than structured diagnostics.
