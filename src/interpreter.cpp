#include "interpreter.hpp"

void Interpreter::print_reg_file() const {
    for (const auto& pair : register_file) {
        std::cout << "R" << pair.first << ": " << pair.second.to_string() << "\n";
    }
}

void Interpreter::print_env() const {
    for (const auto& pair : env) {
        std::cout << pair.first << ": " << pair.second.to_string() << "\n";
    }
}

Interpreter::Interpreter(IRGenerator& gen): 
    // _gen(gen),
    _instr(gen._instr), 
    _ident_table(gen._ident_table),
    _const_table(gen._const_table), 
    _func_table(gen._func_table) {}

void Interpreter::execute() {

    // Interpreter Loop - each iter is a virtual clock cycle
    while (true) {
        Instruction curr_instr = _instr[pc];
        // _gen.print_instruction(curr_instr);
        int a1 = curr_instr.arg1, a2 = curr_instr.arg2, a3 = curr_instr.arg3;
        //         enum OPCode {
        //     ADD_OP,
        //     ADDI_OP,
        //     MUL_OP, 
        //     MULI_OP,
        //     SUB_OP,
        //     SUBI_OP,
        //     DIV_OP,
        //     DIVI_OP,
        //     POW_OP,
        //     POWI_OP,
        //     MOD_OP,
        //     MODI_OP,

        //     AND_OP,
        //     OR_OP,
        //     EQ_OP,
        //     NEQ_OP,
        //     LT_OP,
        //     LTE_OP,
        //     GT_OP,
        //     GTE_OP,

        //     NOT_OP,
        //     PRINT_OP,
        //     SIZE_OP,

        //     NOP, // skip
        //     END,

        //     // Load/Store Ops
        //     LOAD_CONST_OP,
        //     LOAD_VAR_OP,
        //     STORE_VAR_OP,
        //     MOVE_OP,
        //     // Control Flow Ops
        //     JNT, // Jump if not true
        //     JUMP,
        //     JUMPF, // Function jump (decode the register)

        //     PUSH, 
        //     POP,
        //     RET,
        // };

        switch (curr_instr.op) {
            case END: return; // terminate program

            case ADD_OP: register_file[a1] = register_file[a2] + register_file[a3]; pc += 1; break;

            case PRINT_OP: std::cout << register_file[a1].to_string() << std::endl; pc += 1; break;

            case LOAD_CONST_OP: register_file[a1] = _const_table[a2]; pc += 1; break;
            case STORE_VAR_OP: env[_ident_table[a1]] = register_file[a2]; pc += 1; break;
            case LOAD_VAR_OP: register_file[a1] = env[_ident_table[a2]]; pc += 1; break;

            case JUMP: pc = a1; break;
            case JUMPF: pc = _func_table[a1].start_addr; break;
            case RET: pc = program_stack.top(); program_stack.pop(); pc += 1; break;
            default: break;
        };
    }
}