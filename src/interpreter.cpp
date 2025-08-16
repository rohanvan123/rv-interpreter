#include "interpreter.hpp"
#include "types.hpp"

#include <map>
#include <stack>

Interpreter::Interpreter(const IRGenerator& gen): 
    _instr(gen._instr), 
    _ident_table(gen._ident_table) {}
    // _const_table(gen._const_table), 
    // _func_table(gen._func_table) {}

void Interpreter::execute() {
    int pc = 0;
    int v0 = 0;
    std::map<int, Value> register_file;
    Environment env;
    std::stack<int> program_stack;

    // Interpreter Loop - each iter is a virtual clock cycle
    while (true) {
        Instruction curr_instr = _instr[pc];
        int a1 = curr_instr.arg1, a2 = curr_instr.arg1, a3 = curr_instr.arg3;

        switch (curr_instr.op) {
            case END: return; // terminate program

            case STORE_VAR_OP: evn[_ident_table[a1]] = register_file[a2]; pc += 1;
            case LOAD_VAR_OP: register_file[a1] = evn[_ident_table[a2]]; pc += 1;
            default: break;
        };
    }
}