# Bytecode Interpreter

The bytecode interpreter executes the IR instruction stream generated from RV AST expressions.

Implementation:

- `includes/interpreter.hpp`
- `src/interpreter.cpp`

## Role

This is the default RV runtime.

```bash
./bin/main program.rv
```

The CLI uses the tree evaluator only when `--tree-evaluate` is passed.

## Input

The interpreter is constructed from an `IRGenerator`:

```cpp
Interpreter interpreter(gen);
```

It references:

- Instruction stream
- Identifier table
- Constant table
- Function table

## Runtime State

The interpreter maintains:

| State | Purpose |
| --- | --- |
| `pc` | Program counter into the instruction stream. |
| `v0` | Function return value register. |
| `t0` | Temporary mutation result register. |
| `program_stack` | Stack of RV frames. |
| `current_frame` | Pointer to the active frame. |

Each frame contains:

```cpp
std::map<int, Value> register_file;
Environment env;
int return_addr;
```

## Execution Loop

`Interpreter::execute()` repeatedly:

1. Reads `_instr[pc]`.
2. Extracts `arg1`, `arg2`, and `arg3`.
3. Dispatches on `curr_instr.op`.
4. Mutates registers, environment, stack, or program counter.
5. Stops on `END`.

Most instructions increment `pc` by one. Jump and return instructions set `pc` directly.

## Values And Variables

Constants:

```text
LOAD_CONST_OP dest const_index
```

loads from `_const_table`.

Variables:

```text
STORE_VAR_OP ident_index source_register
LOAD_VAR_OP dest ident_index
```

read and write the current frame environment.

## Arithmetic And Boolean Operations

The VM delegates runtime semantics to `Value` operators:

```cpp
register_file[a1] = register_file[a2] + register_file[a3];
register_file[a1] = register_file[a2] <= register_file[a3];
register_file[a1] = !register_file[a2];
```

This keeps VM execution consistent with the tree evaluator because both use the same `Value` implementation.

## Lists And Strings

`INIT_LIST` creates a list value.

`APPEND` appends to a list register during list literal construction.

`ACCESS` evaluates list or string indexing.

`MODIFY` computes an updated list or string and stores it in `t0`; a following `MOVE_OP` transfers `T0` into a normal register.

## Control Flow

`JNT` branches when a condition register is not equal to `true`:

```cpp
pc = condition_is_true ? pc + 1 : jump_target;
```

`JUMP` sets `pc` unconditionally.

## Function Calls

`PUSH` copies the current frame and pushes it onto `program_stack`.

`JUMPF` either:

- Jumps to a user function's start address from `_func_table`
- Dispatches to `handle_builtin_func()` for negative builtin ids

`RET` sets `pc` to the current frame's return address and pops the frame.

Return values move through `V0`:

```text
MOVE V0 Rn
RET
MOVE Rm V0
```

## Builtins

Builtin functions are executed by `handle_builtin_func()`:

- `append`
- `remove`
- `type`
- `string`

The argument values are read from the current frame environment using the builtin function parameter names defined in `includes/builtins.hpp`.

## Debugging Helpers

The interpreter includes helpers for inspecting runtime state:

- `print_reg_file()`
- `print_env()`

They are not currently exposed as CLI flags.

## Design Notes

- Stack-frame copying is intentional and matches the current RV function-scope model.
- The VM is register-based rather than stack-based for expression results.
- The instruction set still has unused immediate opcodes such as `ADDI_OP`; these are available for future optimization but are not currently emitted.
