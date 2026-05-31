# Tree Evaluator

The tree evaluator is the legacy RV runtime. It executes AST expressions directly without lowering them to IR.

Implementation:

- `includes/tree_evaluator.hpp`
- `src/tree_evaluator.cpp`

## Role

The tree evaluator remains supported through:

```bash
./bin/main program.rv --tree-evaluate
```

It is useful as:

- A simpler reference implementation for RV semantics
- A way to compare behavior against the bytecode interpreter
- A debugging tool when investigating IR generation or VM behavior

The default runtime is still the IR generator plus bytecode interpreter.

## Input And Output

Input:

```text
std::vector<Expression*>
```

Output:

```text
Program output printed to stdout
```

`TreeEvaluator::evaluate_commands()` walks each top-level expression and evaluates it.

## Runtime State

The evaluator maintains:

| State | Purpose |
| --- | --- |
| `env_stack` | Stack of variable environments. |
| `curr_env` | Pointer to the current environment. |
| `func_env` | Function name to function AST node map. |

An environment is:

```cpp
using Environment = std::map<std::string, Value>;
```

## Evaluation Results

Internal evaluation returns:

```cpp
std::pair<Value, bool>
```

The `Value` is the expression result. The boolean indicates whether a `return` should propagate out of the current block or function.

## Supported Expressions

The evaluator handles:

- Constants
- Variables
- Binary expressions
- Monadic expressions
- Assignments
- `if` / `else`
- `while`
- Function declarations
- Function calls
- Builtins
- List literals
- List/string access
- List/string modification
- Empty returns

## Function Calls

For user-defined functions:

1. Argument expressions are evaluated.
2. A copied environment is pushed.
3. Parameter names are bound to argument values.
4. Function body expressions execute in order.
5. A returnable expression stops execution of the function body.
6. The previous environment is restored.

This mirrors the intentional caller-frame-copying behavior used by the VM.

## Builtins

Builtin calls are handled directly in the evaluator:

- `append`
- `remove`
- `type`
- `string`

`size` is evaluated as a monadic operator.

## Design Notes

- The tree evaluator is easier to read than the VM path because it directly follows AST structure.
- It is not the primary runtime anymore, so new VM features should be checked against it when legacy support matters.
- It shares the same `Value` operations as the bytecode interpreter, which keeps most runtime type behavior consistent across both execution modes.
