#include "ir_generator.hpp"
#include "builtins.hpp"
#include "utils.hpp"

#include <format>
#include <iostream>

// const int PC_REG = -1; // PC id
// const int V0_REG = -2; // return reg id 
const int T0_REG = -3; // Temp0 reg id 

std::vector<Instruction>& IRGenerator::generate_ir_code(const std::vector<Expression*>& _exps) {
    for (auto exp : _exps) {
        generate_ir_block(exp);
    }

    _instr.push_back({ITYPE, END, -1, -1, -1});

    // define functions;
    while (!func_assign_queue.empty()) {
        int fid = func_assign_queue.front();
        func_assign_queue.pop();
        gen_func_assign_exp_ir(_func_table[fid]);
    }

    return _instr;
}

int IRGenerator::generate_ir_block(Expression* exp) {
    // std::cout << "called" << std::endl;
    switch (exp->get_signature()) {
        case ExpressionType::EMPTY_EXP: {
            EmptyExpression* empty_exp = dynamic_cast<EmptyExpression*>(exp);
            return gen_empty_exp_ir(empty_exp);
        }
        case ExpressionType::CONST_EXP: {
            ConstExp* const_exp = dynamic_cast<ConstExp*>(exp);
            return gen_const_exp_ir(const_exp);
        }
        case ExpressionType::LET_EXP: {
            AssignmentExpression* let_exp = dynamic_cast<AssignmentExpression*>(exp);
            return gen_let_exp_ir(let_exp);
        }
        case ExpressionType::MON_EXP: {
            MonadicExpression* mon_exp = dynamic_cast<MonadicExpression*>(exp);
            return gen_mon_exp_ir(mon_exp);
        }
        case ExpressionType::VAR_EXP: {
            VarExp* var_exp = dynamic_cast<VarExp*>(exp);
            return gen_var_exp_ir(var_exp);
        }
        case ExpressionType::BIN_EXP: {
            BinaryExpression* bin_exp = dynamic_cast<BinaryExpression*>(exp);
            return gen_bin_exp_ir(bin_exp);
        }
        case ExpressionType::IF_EXP: {
            IfExpression* if_exp = dynamic_cast<IfExpression*>(exp);
            return gen_if_exp_ir(if_exp);
        }
        case ExpressionType::WHILE_EXP: { 
            WhileExpression* while_exp = dynamic_cast<WhileExpression*>(exp);
            return gen_while_exp_ir(while_exp);
        }
        case ExpressionType::FUNC_ASSIGN_EXP: { 
            FunctionAssignmentExpression* func_exp = dynamic_cast<FunctionAssignmentExpression*>(exp);
            return store_func_assign_exp(func_exp);
        }
        case ExpressionType::FUNC_CALL_EXP: { 
            FunctionCallExpression* func_exp = dynamic_cast<FunctionCallExpression*>(exp);
            return gen_func_call_exp_ir(func_exp);
        }
        case ExpressionType::LIST_EXP: {
            ListExpression* list_exp = dynamic_cast<ListExpression*>(exp);
            return gen_list_exp_ir(list_exp);
        }
        case ExpressionType::LIST_ACCESS_EXP: {
            ListAccessExpression* access_exp = dynamic_cast<ListAccessExpression*>(exp);
            return gen_list_access_exp_ir(access_exp);
        }
        case ExpressionType::LIST_MODIFY_EXP: {
            ListModifyExpression* modify_exp = dynamic_cast<ListModifyExpression*>(exp);
            return gen_list_modify_exp_ir(modify_exp);
        }
        default: {
            return 0;
        }
    };
}

int IRGenerator::gen_empty_exp_ir(EmptyExpression* empty_exp) {
    if (empty_exp->is_returnable()) {
        _instr.push_back({JTYPE, RET, -1, -1, -1});
    } else {
        _instr.push_back({ITYPE, NOP, -1, -1, -1});
    }
    return curr_reg;
}

int IRGenerator::gen_const_exp_ir(ConstExp* const_exp) { 
    // std::cout << "called const" << std::endl;
    int table_idx = _const_table.size();
    _const_table.push_back(const_exp->get_val());

    _instr.push_back({ITYPE, LOAD_CONST_OP, curr_reg, table_idx, -1});

    if (const_exp->is_returnable()) {
        _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
        _instr.push_back({JTYPE, RET, -1, -1, -1});
    }

    return curr_reg++; 
}

int IRGenerator::gen_var_exp_ir(VarExp* var_exp) {
    std::string var_name = var_exp->get_var_name();

    _instr.push_back({RTYPE, LOAD_VAR_OP, curr_reg, ident_to_idx[var_name], -1}); // curr_reg <- VAR

    if (var_exp->is_returnable()) {
        _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
        _instr.push_back({JTYPE, RET, -1, -1, -1});
    }
    
    return curr_reg++; 
}

int IRGenerator::gen_let_exp_ir(AssignmentExpression* let_exp) {
    // std::cout << "building let" << std::endl ;
    int t1 = generate_ir_block(let_exp->get_right());
    std::string var_name = let_exp->get_id();

    int ident_idx;
    if (!let_exp->is_reassign()) {
        // does not exist in our map, so add it
        ident_idx = _ident_table.size();
        _ident_table.push_back(var_name);
        ident_to_idx[var_name] = ident_idx;
    } else {
        ident_idx = ident_to_idx[var_name];
    }

    _instr.push_back({RTYPE, STORE_VAR_OP, ident_idx, t1, -1}); // Var name -> curr_reg
    return curr_reg;
}

int IRGenerator::gen_mon_exp_ir(MonadicExpression* mon_exp) {
    int t1 = generate_ir_block(mon_exp->get_right());

    switch (mon_exp->get_type()) {
        case MonadicOperator::NotOp: {
            _instr.push_back({RTYPE, NOT_OP, curr_reg, t1, -1});

            if (mon_exp->is_returnable()) {
                _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
                _instr.push_back({JTYPE, RET, -1, -1, -1});
            }

            return curr_reg++;
        }
        case MonadicOperator::IntNegOp: {
            _instr.push_back({ITYPE, NEG_OP, curr_reg, t1, -1});

            if (mon_exp->is_returnable()) {
                _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
                _instr.push_back({JTYPE, RET, -1, -1, -1});
            }

            return curr_reg++;
        }
        case MonadicOperator::PrintOp: {
            _instr.push_back({RTYPE, PRINT_OP, t1, -1, -1});
            return curr_reg;
        }
        case MonadicOperator::SizeOp: {
            _instr.push_back({RTYPE, SIZE_OP, curr_reg, t1, -1}); // cur_reg = size(t1)

            if (mon_exp->is_returnable()) {
                _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
                _instr.push_back({JTYPE, RET, -1, -1, -1});
            }

            return curr_reg++;
        }
    };
}

int IRGenerator::gen_bin_exp_ir(BinaryExpression* bin_exp) {
    // std::cout << "bin_exp" << std::endl;
    int t1 = generate_ir_block(bin_exp->get_left());
    int t2 = generate_ir_block(bin_exp->get_right());
    OPCode bin_op_code = map_binexp_to_opcode(bin_exp->get_type());

    _instr.push_back({ITYPE, bin_op_code, curr_reg, t1, t2});

    if (bin_exp->is_returnable()) {
        _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
        _instr.push_back({JTYPE, RET, -1, -1, -1});
    }

    return curr_reg++;
}

int IRGenerator::gen_if_exp_ir(IfExpression* if_exp) {
    int t1 = generate_ir_block(if_exp->get_conditional());
    int cond_jump_instr_idx = _instr.size();
    _instr.push_back({JTYPE, JNT, t1, -1, -1}); // if not t1, jumpt to arg2 (empty for now until exps)
    int ti;
    for (Expression* exp : if_exp->get_if_exps()) {
        ti = generate_ir_block(exp);
    }
    // need to add a jump so that end if jumps past the else portion
    int endif_jump_instr_idx = _instr.size();
    _instr.push_back({JTYPE, JUMP, -1, -1, -1});

    _instr[cond_jump_instr_idx].arg2 = _instr.size();

    for (Expression* exp : if_exp->get_else_exps()) {
        ti = generate_ir_block(exp);
    }

    _instr[endif_jump_instr_idx].arg1 = _instr.size();

    return ti;
}

int IRGenerator::gen_while_exp_ir(WhileExpression* while_exp) {
    int cond_calc_idx = _instr.size();
    int t1 = generate_ir_block(while_exp->get_conditional());
    int jump_instr_idx = _instr.size();
    _instr.push_back({JTYPE, JNT, t1, -1, -1}); // if not t1, jumpt to arg2 (empty for now until exps)
    int ti;
    for (Expression* exp : while_exp->get_body_exps()) {
        ti = generate_ir_block(exp);
    }
    _instr.push_back({JTYPE, JUMP, cond_calc_idx, -1, -1}); // Jump to cond check start
    _instr[jump_instr_idx].arg2 = _instr.size();

    return ti;

}

int IRGenerator::store_func_assign_exp(FunctionAssignmentExpression* func_exp) {
    // this just creates and stores meta data, the actual function will be declared at the end
    int fid = _func_table.size();
    ident_to_fid[func_exp->get_name()] = fid;
    _func_table.push_back({func_exp->get_name(), -1, func_exp}); // function addr is resolved at the end
    func_assign_queue.push(fid);
    return curr_reg;
}

int IRGenerator::gen_func_assign_exp_ir(FunctionInfo& func_info) {
    
    FunctionAssignmentExpression* func_exp = func_info.func_exp;
    func_info.start_addr = _instr.size();
    addr_to_ident[func_info.start_addr] = func_info.name;

    for (Expression* exp : func_exp->get_body_exps()) {
        generate_ir_block(exp);
    }

    _instr.push_back({JTYPE, RET, -1, -1, -1}); // This instruction is psuedo for POP eip (which puts the top stack value into PC) (acts as the return)

    return curr_reg;
}

int IRGenerator::gen_func_call_exp_ir(FunctionCallExpression* call_exp) {
    FunctionAssignmentExpression* func_exp;
    int fid;

    if (builtin::is_builtin_func(call_exp->get_name())) {
        fid = builtin::builtin_to_fid.at(call_exp->get_name());
        func_exp = builtin::builtin_func_exps[call_exp->get_name()];
    } else {
        fid = ident_to_fid[call_exp->get_name()];
        func_exp = _func_table[fid].func_exp;
    }
    

    const std::vector<std::string> arg_names = func_exp->get_arg_names();
    const std::vector<Expression*> arg_exps = call_exp->get_arg_exps();
    
    _instr.push_back({RTYPE, PUSH, -1, -1, -1}); // PUSH PC (-1) reg onto stack
    // load variables
    for (size_t i = 0; i < call_exp->get_args_length(); i++) {
        std::string argi = arg_names[i];
        Expression* arg_expi = arg_exps[i];
        int t1 = generate_ir_block(arg_expi);

        int ident_idx = _ident_table.size();
        _ident_table.push_back(argi);
        ident_to_idx[argi] = ident_idx;
        // std::cout << argi << " " << utils::string_of_expression(arg_expi) << "\n";
        // print_instruction({RTYPE, STORE_VAR_OP, ident_idx, t1, -1});
        _instr.push_back({RTYPE, STORE_VAR_OP, ident_idx, t1, -1}); // Var name -> curr_reg
    }

    _instr.push_back({JTYPE, JUMPF, fid, -1}); // FID is evaluated eventually using the table to get the start adress
    _instr.push_back({RTYPE, MOVE_OP, curr_reg, -2, -1}); // stores the return value of the function (if it returns)

    return curr_reg++;
    
}

int IRGenerator::gen_list_exp_ir(ListExpression* list_exp) {
    // std::cout << "called list" << std::endl;
    int list_reg = curr_reg++;
    _instr.push_back({ITYPE, INIT_LIST, list_reg, -1, -1}); // curr_reg -> []

    for (Expression* exp : list_exp->get_elements()) {
        int ti = generate_ir_block(exp);
        _instr.push_back({RTYPE, APPEND, list_reg, ti, -1});  // => list.append(ti)
    }

    if (list_exp->is_returnable()) {
        _instr.push_back({RTYPE, MOVE_OP, -2, list_reg, -1});
        _instr.push_back({JTYPE, RET, -1, -1, -1});
    } 

    return list_reg;
}

int IRGenerator::gen_list_access_exp_ir(ListAccessExpression* access_exp) {
    int t1 = generate_ir_block(access_exp->get_arr_exp());
    int t2 = generate_ir_block(access_exp->get_idx_exp());
    _instr.push_back({RTYPE, ACCESS, curr_reg, t1, t2}); // curr = t1[t2]

    if (access_exp->is_returnable()) {
        _instr.push_back({RTYPE, MOVE_OP, -2, curr_reg, -1});
        _instr.push_back({JTYPE, RET, -1, -1, -1});
    }

    return curr_reg++;
}

int IRGenerator::gen_list_modify_exp_ir(ListModifyExpression* modify_exp) {
    int t1 = generate_ir_block(modify_exp->get_ident_exp());
    int t2 = generate_ir_block(modify_exp->get_idx_exp());
    int t3 = generate_ir_block(modify_exp->get_exp());

    _instr.push_back({RTYPE, MODIFY, t1, t2, t3}); // T0 = modify(x, 3, ele)
    _instr.push_back({RTYPE, MOVE_OP, curr_reg, T0_REG});

    return curr_reg++;
}

// Helpers

OPCode IRGenerator::map_binexp_to_opcode(BinaryOperator op) const {
    switch (op) {
        case BinaryOperator::IntPlusOp: return ADD_OP;
        case BinaryOperator::IntTimesOp: return MUL_OP;
        case BinaryOperator::IntMinusOp: return SUB_OP;
        case BinaryOperator::IntDivOp: return DIV_OP;
        case BinaryOperator::IntPowOp: return POW_OP;
        case BinaryOperator::ModOp: return MOD_OP;
        case BinaryOperator::EqualityOp: return EQ_OP;
        case BinaryOperator::NotEqualsOp: return NEQ_OP;
        case BinaryOperator::AndOp: return AND_OP;
        case BinaryOperator::OrOp: return OR_OP;
        case BinaryOperator::LtOp:  return LT_OP;
        case BinaryOperator::LteOp: return LTE_OP;
        case BinaryOperator::GtOp: return GT_OP;
        case BinaryOperator::GteOp: return GTE_OP;
        default:
            throw std::runtime_error("Unknown binary operation");
    };
}


std::string to_string(OPCode op) {
    switch (op) {
        case ADD_OP: return "ADD";
        case ADDI_OP: return "ADDI";
        case MUL_OP: return "MUL";
        case MULI_OP: return "MULI";
        case SUB_OP: return "SUB";
        case SUBI_OP: return "SUBI";
        case DIV_OP: return "DIV";
        case DIVI_OP: return "DIVI";
        case POW_OP: return "POW";
        case MOD_OP: return "MOD";
        
        case GT_OP: return "GT";
        case GTE_OP: return "GTE";
        case LT_OP: return "LT";
        case LTE_OP: return "LTE";
        case AND_OP: return "AND";
        case OR_OP: return "OR";
        case EQ_OP: return "EQ";
        case NEQ_OP: return "NEQ";

        case LOAD_CONST_OP: return "LOAD_CONST";
        case LOAD_VAR_OP: return "LOAD_VAR";
        case STORE_VAR_OP: return "STORE_VAR";
        case INIT_LIST: return "INIT_LIST";
        case APPEND: return "APPEND";
        case ACCESS: return "ACCESS";
        case MODIFY: return "MODIFY";

        case PRINT_OP: return "PRINT";
        case SIZE_OP: return "SIZE";
        case NEG_OP: return "NEG";
        case NOT_OP: return "NOT";

        case JNT: return "JNT";
        case JUMP: return "JUMP";
        case JUMPF: return "JUMP";
        case MOVE_OP: return "MOVE";
        case RET: return "RET";
        case PUSH: return "PUSH";
        case POP: return "POP";

        case NOP: return "NOP";
        case END: return "END";
        default: return "UNKOWN OP";
    };
}

void IRGenerator::print_instructions() const {
    std::cout << "main" << "\n";
    for (size_t i = 0; i < _instr.size(); i++) {
        if (addr_to_ident.find(i) != addr_to_ident.end()) std::cout << addr_to_ident.at(i) << "\n";
        
        std::string indent;
        if (i < 10) indent = "    ";
        if (i >= 10 && i < 100) indent = "   ";
        if (i >= 100 && i < 1000) indent = "  ";
        std::cout << indent << i << "   ";
        print_instruction(_instr[i]);
    }
}

void IRGenerator::print_ident_table() const {
    for (size_t i = 0; i < _ident_table.size(); i++) {
        std::cout << i << ": " << _ident_table[i] << "\n";
    }
}

std::string reg_string(int reg) {
    switch (reg) {
        // General Purpose Registers
        case -1: return "PC"; // program counter
        case -2: return "V0"; // return reg
        case -3: return "T0";
        default: return "R" + std::to_string(reg);
    };
}

void IRGenerator::print_instruction(Instruction inst) const {
    std::cout << to_string(inst.op) << " ";
    switch(inst.op) {
        case (MULI_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 <<  " " << inst.arg3; break;

        case (MUL_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (ADD_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (SUB_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (DIV_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (MOD_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (POW_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (EQ_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (NEQ_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (LT_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (LTE_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (GT_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (GTE_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (AND_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        case (OR_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;

        case (STORE_VAR_OP): {
            // std::cout << "DEBUG " << inst.arg1 << " " << _ident_table[inst.arg1] << "\n";
            std::cout << _ident_table[inst.arg1] << " R" << inst.arg2; break;
        }
        case (LOAD_CONST_OP): std::cout << "R" << inst.arg1 << " " << _const_table[inst.arg2].to_string(); break;
        case (LOAD_VAR_OP): {
            // std::cout << "DEBUG " << inst.arg2 << " " << _ident_table[inst.arg2] << "\n";
            std::cout << "R" << inst.arg1 << " " << _ident_table[inst.arg2]; break;
        }
        case (INIT_LIST): std::cout << "R" << inst.arg1; break;
        case (APPEND): std::cout << "R" << inst.arg1 << " " << "R" << inst.arg2; break;
        case (ACCESS): std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
        
        case (PRINT_OP): std::cout << "R" << inst.arg1; break;
        case (SIZE_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2; break;
        case (NEG_OP): std::cout << "R" << inst.arg1 << " R" << inst.arg2; break;
        case (JNT): std::cout << "R" << inst.arg1 << " " << inst.arg2; break;
        case (JUMP): std::cout << inst.arg1; break;
        case (JUMPF): {
            std::string func_string = (inst.arg1 < 0) ? builtin::fid_to_builtin.at(inst.arg1) : _func_table[inst.arg1].name;
            std::cout << func_string; 
            break;
        }
        case (MOVE_OP): std::cout << reg_string(inst.arg1) << " " << reg_string(inst.arg2); break;
        case (PUSH): std::cout << reg_string(inst.arg1); break;
        case (POP): std::cout << reg_string(inst.arg1); break;
        case (RET): break;
        case (NOP): break;
        case (END): break;
        default: std::cout << "R" << inst.arg1 << " R" << inst.arg2 << " R" << inst.arg3; break;
    }

    std::cout << "\n";
}
