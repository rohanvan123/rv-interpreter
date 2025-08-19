#include "interpreter.hpp"
#include "builtins.hpp"

#include <unistd.h>

const int V0_REG = -2;

const Value TRUE_VAL = Value(true);

Interpreter::Interpreter(IRGenerator& gen): 
    _instr(gen._instr), 
    _ident_table(gen._ident_table),
    _const_table(gen._const_table), 
    _func_table(gen._func_table) 
{
    program_stack.push(RvStackFrame{{}, {}, 0});
    current_frame = &program_stack.top();
}

void Interpreter::push_stack_frame() {
    RvStackFrame frame_copy = *current_frame; // make a copy of the topmost frame
    program_stack.push(frame_copy);
    current_frame = &program_stack.top(); // set current frame to the top of the stack (this new frame)
}

void Interpreter::pop_stack_frame() {
    program_stack.pop();
    current_frame = &program_stack.top();
}

void Interpreter::execute() {

    // Interpreter Loop - each iter is a virtual clock cycle
    while (true) {
        Instruction curr_instr = _instr[pc];
        // std::cout << "PC: " << pc << std::endl;
        int a1 = curr_instr.arg1, a2 = curr_instr.arg2, a3 = curr_instr.arg3;
        Environment& env = current_frame->env;
        auto& register_file = current_frame->register_file;
        int& frame_return_addr = current_frame->return_addr;

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

        //     POP,
        // };

        switch (curr_instr.op) {
            case END: return; // terminate program
            case NOP: pc += 1; break;

            case ADD_OP: register_file[a1] = register_file[a2] + register_file[a3]; pc += 1; break;
            case SUB_OP: register_file[a1] = register_file[a2] - register_file[a3]; pc += 1; break;
            case MUL_OP: register_file[a1] = register_file[a2] * register_file[a3]; pc += 1; break;
            case DIV_OP: register_file[a1] = register_file[a2] / register_file[a3]; pc += 1; break;
            case MOD_OP: register_file[a1] = register_file[a2] % register_file[a3]; pc += 1; break;
            case POW_OP: register_file[a1] = register_file[a2].pow(register_file[a3]); pc += 1; break;
            case GT_OP: register_file[a1] = register_file[a2] > register_file[a3]; pc += 1; break;
            case GTE_OP: register_file[a1] = register_file[a2] >= register_file[a3]; pc += 1; break;
            case LT_OP: register_file[a1] = register_file[a2] < register_file[a3]; pc += 1; break;
            case LTE_OP: register_file[a1] = register_file[a2] <= register_file[a3]; pc += 1; break;

            case EQ_OP: register_file[a1] = register_file[a2] == register_file[a3]; pc += 1; break;
            case NEQ_OP: register_file[a1] = register_file[a2] != register_file[a3]; pc += 1; break;
            case AND_OP: register_file[a1] = register_file[a2] && register_file[a3]; pc += 1; break;
            case OR_OP: register_file[a1] = register_file[a2] || register_file[a3]; pc += 1; break;

            case PRINT_OP: std::cout << register_file[a1].to_string() << std::endl; pc += 1; break;
            case NEG_OP: register_file[a1] = -register_file[a2]; pc += 1; break;
            case NOT_OP: register_file[a1] = !register_file[a2]; pc += 1; break;
            case SIZE_OP: register_file[a1] = register_file[a2].size(); pc += 1; break;

            case LOAD_CONST_OP: register_file[a1] = _const_table[a2]; pc += 1; break;
            case STORE_VAR_OP: env[_ident_table[a1]] = register_file[a2]; pc += 1; break;
            case LOAD_VAR_OP: register_file[a1] = env[_ident_table[a2]]; pc += 1; break;
            case INIT_LIST: register_file[a1] = Value(std::vector<Value>()); pc += 1; break;
            case APPEND: register_file[a1].append_ref(register_file[a2]); pc += 1; break;
            case ACCESS: {
                Value list = register_file[a2]; 
                register_file[a1] = list[register_file[a3]]; 
                pc += 1; 
                break;
            }

            case PUSH: push_stack_frame(); pc += 1; break; // push a copy of env onto the stack
            case MOVE_OP: {
                if (a1 == V0_REG) {
                    v0 = register_file[a2];
                } else if (a2 == V0_REG) {
                    register_file[a1] = v0;
                }
                pc += 1; 
                break;
            }
            case JUMP: pc = a1; break;
            case JUMPF: {
                frame_return_addr = pc + 1; 
                if (a1 < 0) { 
                    handle_builtin_func(a1, a2, a3);
                } else {
                    pc = _func_table[a1].start_addr;
                }
                break;
            }
            case JNT: pc = (register_file[a1].equals(TRUE_VAL)) ? pc + 1 : a2; break;
            case RET: pc = frame_return_addr; pop_stack_frame(); break;
            default: break;
        };
        // sleep(1);
    }
}

void Interpreter::handle_builtin_func(int a1, int a2, int a3) {
    Environment& env = current_frame->env;
    int& frame_return_addr = current_frame->return_addr;

    std::string func_name = builtin::fid_to_builtin.at(a1);
    // std::cout << func_name << " " << env["ele_val"].to_string() << "\n";
    Value res;
    if (func_name == "append") res = builtin::append(env["arr_val"], env["ele_val"]);
    if (func_name == "remove") res = builtin::remove(env["arr_val"], env["idx_val"]);
    if (func_name == "type") res = builtin::type(env["val"]);
    if (func_name == "string") res = builtin::string(env["val"]);

    v0 = res;
    pc = frame_return_addr; 
    pop_stack_frame();
}

void Interpreter::print_reg_file() const {
    for (const auto& pair : current_frame->register_file) {
        std::cout << "R" << pair.first << ": " << pair.second.to_string() << "\n";
    }
    std::cout << "v0: " << v0.to_string() << "\n";
}

void Interpreter::print_env() const {
    for (const auto& pair : current_frame->env) {
        std::cout << pair.first << ": " << pair.second.to_string() << "\n";
    }
}