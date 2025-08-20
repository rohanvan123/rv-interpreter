#ifndef INTERPRETER_CPP
#define INTERPRETER_CPP

#include "ir_generator.hpp"
#include "expression.hpp"

#include <string>
#include <vector>
#include <stack>
#include <map>


struct RvStackFrame {
    std::map<int, Value> register_file;
    Environment env;
    int return_addr;
};

class Interpreter {
private:
    // IRGenerator& _gen;
    std::vector<Instruction>& _instr;
    std::vector<std::string>& _ident_table;
    std::vector<Value>& _const_table;
    std::vector<FunctionInfo>& _func_table;
    // std::map<int, Value> register_file;

    int pc = 0;
    Value v0 = Value();
    Value t0 = Value();

    RvStackFrame* current_frame;
    std::stack<RvStackFrame> program_stack;
    void push_stack_frame();
    void pop_stack_frame();

    void handle_builtin_func(int a1, int a2, int a3);

public:
    Interpreter(IRGenerator& gen);
    void execute();
    void print_reg_file() const;
    void print_env() const;

};

#endif // INTERPRETER_CPP