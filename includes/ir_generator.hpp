#ifndef IR_GENERATOR_HPP
#define IR_GENERATOR_HPP

#include "expression.hpp"

#include <string>
#include <vector>
#include <format>
#include <queue>

enum InstructionType {
    ITYPE, // immediate
    RTYPE, // memory based
    JTYPE  // jumps/branching
};

enum OPCode {
    ADD_OP,
    ADDI_OP,
    MUL_OP, 
    MULI_OP,
    SUB_OP,
    SUBI_OP,
    DIV_OP,
    DIVI_OP,
    POW_OP,
    POWI_OP,
    MOD_OP,
    MODI_OP,

    AND_OP,
    OR_OP,
    EQ_OP,
    NEQ_OP,
    LT_OP,
    LTE_OP,
    GT_OP,
    GTE_OP,

    NOT_OP,
    PRINT_OP,
    SIZE_OP,

    NOP, // skip
    END,

    // Load/Store Ops
    LOAD_CONST_OP,
    LOAD_VAR_OP,
    STORE_VAR_OP,
    MOVE_OP,
    // Control Flow Ops
    JNT, // Jump if not true
    JUMP,
    JUMPF, // Function jump (decode the register)

    PUSH, 
    POP,
    RET,
};

struct Instruction {
    InstructionType type;
    OPCode op;
    int arg1;
    int arg2;
    int arg3;
};

struct FunctionInfo {
    std::string name; // name of function 
    int start_addr; // address (idx) of function's instructions
    FunctionAssignmentExpression* func_exp;
};

class IRGenerator {
private:
    std::queue<int> func_assign_queue;

    std::map<std::string, int> ident_to_idx;
    std::map<std::string, int> ident_to_fid;
    std::map<int, std::string> addr_to_ident;

    int curr_reg = 0;

    int gen_empty_exp_ir(EmptyExpression* empty_exp);
    int gen_const_exp_ir(ConstExp* const_exp);
    int gen_var_exp_ir(VarExp* var_exp);
    int gen_let_exp_ir(AssignmentExpression* let_exp);
    int gen_mon_exp_ir(MonadicExpression* mon_exp);
    int gen_bin_exp_ir(BinaryExpression* bin_exp);
    int gen_if_exp_ir(IfExpression* if_exp);
    int gen_while_exp_ir(WhileExpression* while_exp);
    int gen_func_assign_exp_ir(FunctionInfo& func_info);
    int gen_func_call_exp_ir(FunctionCallExpression* call_exp);
    
    int store_func_assign_exp(FunctionAssignmentExpression* func_exp);
    int generate_ir_block(Expression* exp);

    OPCode map_binexp_to_opcode(BinaryOperator op) const;

public:
    IRGenerator() {}
    std::vector<Instruction>& generate_ir_code(const std::vector<Expression*>& _exps);

    std::vector<Instruction> _instr;
    std::vector<std::string> _ident_table;
    std::vector<Value> _const_table;
    std::vector<FunctionInfo> _func_table;

    // helpers
    void print_ident_table() const;
    void print_instructions() const;
    void print_instruction(Instruction instr) const;
};

#endif // IR_GENERATOR_HPP