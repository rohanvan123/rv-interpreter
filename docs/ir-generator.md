# IR Generator

The IR generator lowers RV AST expressions into an instruction stream consumed by the bytecode interpreter.

Implementation:

- `includes/ir_generator.hpp`
- `src/ir_generator.cpp`

## Input And Output

Input:

```text
std::vector<Expression*>
```

Output:

```text
std::vector<Instruction>&
```

The generator also populates:

- `_instr`
- `_ident_table`
- `_const_table`
- `_func_table`

## Generation Flow

`IRGenerator::generate_ir_code()`:

1. Generates IR for each top-level expression.
2. Emits `END` to terminate main-program execution.
3. Emits function bodies after main code.
4. Resolves function start addresses in `_func_table`.

This layout allows the main program to skip over function bodies during normal execution while still allowing calls to jump into them.

## Expression Dispatch

`generate_ir_block()` switches on `ExpressionType` and delegates to specific generation methods:

| Expression type | Generator method |
| --- | --- |
| `CONST_EXP` | `gen_const_exp_ir` |
| `VAR_EXP` | `gen_var_exp_ir` |
| `LET_EXP` | `gen_let_exp_ir` |
| `MON_EXP` | `gen_mon_exp_ir` |
| `BIN_EXP` | `gen_bin_exp_ir` |
| `IF_EXP` | `gen_if_exp_ir` |
| `WHILE_EXP` | `gen_while_exp_ir` |
| `FUNC_ASSIGN_EXP` | `store_func_assign_exp` |
| `FUNC_CALL_EXP` | `gen_func_call_exp_ir` |
| `LIST_EXP` | `gen_list_exp_ir` |
| `LIST_ACCESS_EXP` | `gen_list_access_exp_ir` |
| `LIST_MODIFY_EXP` | `gen_list_modify_exp_ir` |

## Registers

The generator uses `curr_reg` to allocate fresh virtual registers. Most expression generators return the register id containing that expression's result.

Example:

```rv
1 + 2
```

conceptually emits:

```text
LOAD_CONST R0 1
LOAD_CONST R1 2
ADD R2 R0 R1
```

and returns `R2`.

## Variables

Variable names are stored in `_ident_table`.

Declaration:

```rv
let x = 1;
```

adds `x` to the identifier table and emits `STORE_VAR_OP`.

Reassignment:

```rv
x = 2;
```

requires `x` to already exist in the identifier map.

## Control Flow

`if` expressions are generated with conditional and unconditional jumps:

1. Evaluate condition.
2. Emit `JNT` with unresolved else target.
3. Emit if-body instructions.
4. Emit `JUMP` with unresolved end target.
5. Patch `JNT` to point at else-body start.
6. Emit else-body instructions.
7. Patch `JUMP` to point after the else-body.

`while` expressions:

1. Record condition instruction address.
2. Evaluate condition.
3. Emit `JNT` with unresolved loop-exit target.
4. Emit body.
5. Emit `JUMP` back to condition.
6. Patch `JNT` to point after the loop.

## Functions

Function declarations are stored first:

```cpp
_func_table.push_back({name, -1, func_exp});
```

Their bodies are emitted after the main `END` instruction. The function table stores the final start address.

Function calls:

1. Emit `PUSH`.
2. Evaluate argument expressions.
3. Store argument values under parameter names in the new frame.
4. Emit `JUMPF`.
5. Move `V0` into a normal register for the call result.

Builtin functions use negative ids and are dispatched by the interpreter.

## Return Values

Returnable expressions emit:

```text
MOVE V0 result_register
RET
```

Empty return emits:

```text
RET
```

## Debug Output

`print_instructions()` prints a human-readable IR listing. It is exposed through:

```bash
./bin/main program.rv --output-ir
```
