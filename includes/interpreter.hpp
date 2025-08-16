#ifndef INTERPRETER_CPP
#define INTERPRETER_CPP

#include "ir_generator.hpp"
#include "expression.hpp"

#include <string>
#include <vector>

class Interpreter {
private:
    const std::vector<Instruction>& _instr;
    const std::vector<std::string>& _ident_table;
    // const std::vector<Value>& _const_table;
    // const std::vector<FunctionInfo>& _func_table;
public:
    Interpreter(const IRGenerator& gen);
    void execute();

};

#endif