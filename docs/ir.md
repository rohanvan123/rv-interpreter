# RV IR Reference

RV IR is the bytecode-like intermediate representation generated from AST expressions and executed by the bytecode interpreter.

The relevant implementation files are:

- `includes/ir_generator.hpp`
- `src/ir_generator.cpp`
- `includes/interpreter.hpp`
- `src/interpreter.cpp`

## Instruction Shape

Each instruction is represented by:

```cpp
struct Instruction {
    InstructionType type;
    OPCode op;
    int arg1;
    int arg2;
    int arg3;
};
```

The instruction type is mostly descriptive:

| Type | Meaning |
| --- | --- |
| `ITYPE` | Immediate or table-backed operation. |
| `RTYPE` | Register or environment operation. |
| `JTYPE` | Jump, branch, call, or return operation. |

The interpreter dispatches primarily on `op`.

## Runtime Tables

The IR generator produces an instruction stream plus supporting tables:

| Table | Purpose |
| --- | --- |
| `_const_table` | Stores literal `Value` objects loaded by `LOAD_CONST_OP`. |
| `_ident_table` | Maps identifier slots to variable names. |
| `_func_table` | Stores function metadata: name, start address, and AST body pointer. |

The generated instruction stream refers to table positions by integer index.

## Registers

The VM uses integer register ids inside each stack frame.

Special ids:

| Register | Id | Purpose |
| --- | --- | --- |
| `V0` | `-2` | Function return value register. |
| `T0` | `-3` | Temporary register used for list/string mutation results. |

Normal generated registers use non-negative integer ids.

## Stack Frames

Each frame contains:

```cpp
struct RvStackFrame {
    std::map<int, Value> register_file;
    Environment env;
    int return_addr;
};
```

On function call, the current frame is copied and pushed. This is intentional in the current RV semantics: functions inherit the caller's environment and register state.

## Core Opcodes

Arithmetic:

| Opcode | Meaning |
| --- | --- |
| `ADD_OP` | `dest = lhs + rhs` |
| `SUB_OP` | `dest = lhs - rhs` |
| `MUL_OP` | `dest = lhs * rhs` |
| `DIV_OP` | `dest = lhs / rhs` |
| `POW_OP` | `dest = lhs ^ rhs` |
| `MOD_OP` | `dest = lhs % rhs` |

Comparison and boolean:

| Opcode | Meaning |
| --- | --- |
| `EQ_OP` | `dest = lhs == rhs` |
| `NEQ_OP` | `dest = lhs != rhs` |
| `LT_OP` | `dest = lhs < rhs` |
| `LTE_OP` | `dest = lhs <= rhs` |
| `GT_OP` | `dest = lhs > rhs` |
| `GTE_OP` | `dest = lhs >= rhs` |
| `AND_OP` | `dest = lhs && rhs` |
| `OR_OP` | `dest = lhs || rhs` |
| `NOT_OP` | `dest = !value` |

Values and variables:

| Opcode | Meaning |
| --- | --- |
| `LOAD_CONST_OP` | Load from `_const_table` into a register. |
| `LOAD_VAR_OP` | Load an environment variable into a register. |
| `STORE_VAR_OP` | Store a register value into the environment. |
| `MOVE_OP` | Move values to or from special registers such as `V0` and `T0`. |

Lists and strings:

| Opcode | Meaning |
| --- | --- |
| `INIT_LIST` | Create an empty list. |
| `APPEND` | Append a register value to a list register. |
| `ACCESS` | Read `value[index]`. |
| `MODIFY` | Compute an updated list/string value after indexed assignment. |

Control flow and functions:

| Opcode | Meaning |
| --- | --- |
| `JNT` | Jump if a condition register is not `true`. |
| `JUMP` | Unconditional jump. |
| `JUMPF` | Jump to a function start address or builtin handler. |
| `PUSH` | Push a copied stack frame. |
| `RET` | Return to the saved frame return address. |
| `END` | Terminate execution. |
| `NOP` | No operation. |

## Example

RV source:

```rv
let x = 1 + 2;
print(x);
```

Conceptually lowers to:

```text
LOAD_CONST R0 1
LOAD_CONST R1 2
ADD R2 R0 R1
STORE_VAR x R2
LOAD_VAR R3 x
PRINT R3
END
```

Actual register numbers and table indexes depend on the full program.

## Function Calls

Function declarations are first stored as metadata. After the main program emits `END`, function bodies are emitted later in the instruction stream.

At call sites:

1. `PUSH` copies the current stack frame.
2. Argument expressions are evaluated.
3. Argument values are stored under parameter names in the new frame environment.
4. `JUMPF` transfers control to the function start address.
5. Return values are moved through `V0`.
6. `RET` restores the caller frame and program counter.

Builtin functions use negative function ids and are handled directly by `Interpreter::handle_builtin_func`.
