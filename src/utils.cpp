#include <fstream>

void print_tokens(const std::vector<Token>& tokens) {
    // std::cout << "[";
    for (size_t i = 0; i < tokens.size() - 1; i++) {
        std::cout << token_to_string(tokens[i]) << ", ";
    }
    std::cout << token_to_string(tokens[tokens.size() - 1]) << std::endl;
}

void print_tokens_backend(const std::vector<Token>& tokens) {
    for (size_t i = 0; i < tokens.size() - 1; i++) {
        std::cout << token_to_string(tokens[i]) << ",";
    }
    std::cout << token_to_string(tokens[tokens.size() - 1]) << std::endl;
}

void print_tokens_by_line(const std::vector<Token>& tokens) {
    std::vector<Token> curr;
    int line_no = 1;

    for (int i = 0; i < tokens.size(); i++) {
        Token tok = tokens[i];
        if (tok.get_line() != line_no) {
            print_tokens(curr);
            curr.clear();
            line_no = tok.get_line();
        }
        curr.push_back(tok);
    }
}

void print_commands(const std::vector<std::vector<Token>>& sequence) {
    std::cout << "[" << std::endl;
    for (size_t i = 0; i < sequence.size() - 1; i++) {
        std::cout << "  ";
        print_tokens(sequence[i]);
    }

    std::cout << "  ";
    print_tokens(sequence[sequence.size() - 1]);
    std::cout << "]" << std::endl;
}

std::vector<std::vector<Token>> generate_sequence(std::vector<Token>& tokens) {
    std::vector<std::vector<Token>> sequence;
    std::vector<Token> curr;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].get_type() == SEMI) {
            sequence.push_back(curr);
            curr.clear();
        } else {
            curr.push_back(tokens[i]);
        }
    }

    return sequence;
}

std::string read_file_into_buffer(char *filepath) {
    std::ifstream program_file(filepath);
    std::string buffer;
    std::string line;
    while (std::getline(program_file, line)) {
        buffer += line + "\n"; // Append each line to the buffer with a newline character
    }

    return buffer;
}

std::string string_of_expression(Expression* exp) {
    // std::cout << exp << std::endl;
    // std::cout << "called" << std::endl;
    std::string res = "";
    switch (exp->get_signature()) {
        case ExpressionType::CONST_EXP: {
            // std::cout << "const" << std::endl;
            ConstExp * const_exp = dynamic_cast<ConstExp*>(exp);
            res += "ConstExp(";
            
            switch (const_exp->get_type()) {
                case ConstType::BoolConst: {
                    std::string bool_str = std::get<bool>(const_exp->value.data) ? "true" : "false";
                    return res + "BoolConst " + bool_str + ')';
                }
                case ConstType::IntConst: return res + "IntConst " + std::to_string(std::get<int>(const_exp->value.data)) + ')';
                case ConstType::StringConst: {
                    std::string s = std::get<std::string>(const_exp->value.data);
                    return  res + "StringConst \"" + s + "\")";
                }
            };
            break;
        }
        case ExpressionType::VAR_EXP: {
            // std::cout << "var" << std::endl;
            // std::cout << "its variable" << std::endl;
            VarExp * var_exp = dynamic_cast<VarExp*>(exp);
            return "VarExp(" + var_exp->get_var_name() + ")";
            break;
        }
        case ExpressionType::IF_EXP: {
            IfExpression * if_statement = dynamic_cast<IfExpression*>(exp);
            res += "IfExp(";
            res += string_of_expression(if_statement->get_conditional());
            res += ", [";
            bool first = true;
            for (Expression * inner_exp : if_statement->get_if_exps()) {
                if (!first) res += ", ";
                res += string_of_expression(inner_exp);
                first = false;
            }
            
            res += "], [";
            first = true;
            for (Expression * inner_exp : if_statement->get_else_exps()) {
                if (!first) res += ", ";
                res += string_of_expression(inner_exp);
                first = false;
            }

            res += "])";
            break;
        }
        case ExpressionType::WHILE_EXP: {
            WhileExpression * while_statment = dynamic_cast<WhileExpression*>(exp);
            res += "WhileExp(";
            res += string_of_expression(while_statment->get_conditional());
            res += ", [";
            bool first = true;
            for (Expression * inner_exp : while_statment->get_body_exps()) {
                if (!first) res += ", ";
                res += string_of_expression(inner_exp);
                first = false;
            }

            res += "])";
            break;
        }
        case ExpressionType::LIST_EXP: {
            ListExpression * list_exp = dynamic_cast<ListExpression*>(exp);
            res += "ListExp(";
            res += "[";
            bool first = true;
            for (Expression * inner_exp : list_exp->get_elements()) {
                if (!first) res += ", ";
                res += string_of_expression(inner_exp);
                first = false;
            }

            res += "])";
            break;

        }
        case ExpressionType::LIST_ACCESS_EXP: {
            ListAccessExpression * list_access_exp = dynamic_cast<ListAccessExpression*>(exp);
            res += "ListAccessExp(";
            res += string_of_expression(list_access_exp->get_arr_exp()) + ", ";
            res += string_of_expression(list_access_exp->get_idx_exp());
            res += ")";

            break;
        }
        case ExpressionType::LIST_MODIFY_EXP: {
            ListModifyExpression * list_assign_exp = dynamic_cast<ListModifyExpression*>(exp);
            res += "ListModifyExp(";
            res += string_of_expression(list_assign_exp->get_ident_exp()) + ", ";
            res += string_of_expression(list_assign_exp->get_idx_exp()) + ", ";;
            res += string_of_expression(list_assign_exp->get_exp());
            res += ")";

            break;
        }
        case ExpressionType::BIN_EXP: {
            // std::cout << "bin" << std::endl;
            BinaryExpression * bin_exp = dynamic_cast<BinaryExpression*>(exp);
            res += "BinaryExp(";
            std::map<BinaryOperator, std::string> op_to_string = {
                {BinaryOperator::IntPlusOp, "IntPlusOp, "},
                {BinaryOperator::IntMinusOp, "IntMinusOp, "},
                {BinaryOperator::IntTimesOp, "IntTimesOp, "},
                {BinaryOperator::IntDivOp, "IntDivOp, "},
                {BinaryOperator::ModOp, "ModOp, "},
                {BinaryOperator::GtOp, "GtOp, "},
                {BinaryOperator::GteOp, "GteOp, "},
                {BinaryOperator::LtOp, "LtOp, "},
                {BinaryOperator::LteOp, "LteOp, "},
                {BinaryOperator::NotEqualsOp, "NotEqualsOp, "},
                {BinaryOperator::EqualityOp, "EqualsOp, "},
                {BinaryOperator::AndOp, "AndOp, "},
                {BinaryOperator::OrOp, "OrOp, "},

            };
            // switch (bin_exp->get_type()) {
            //     case BinaryOperator::IntPlusOp: 
            //         res += "IntPlusOp, "; 
            //         break;
            //     case BinaryOperator::IntMinusOp: 
            //         res += "IntMinusOp, ";
            //         break;
            //     case BinaryOperator::IntTimesOp: 
            //         res += "IntTimesOp, ";
            //         break;
            //     case BinaryOperator::IntDivOp: 
            //         res += "IntDivOp, ";
            //         break;
            //     case BinaryOperator::ModOp: 
            //         res += "ModOp, ";
            //         break;
            // };
            res += op_to_string[bin_exp->get_type()];
            res += string_of_expression(bin_exp->get_left());
            res += ", ";
            res += string_of_expression(bin_exp->get_right());
            res += ")";
            break;
        }
        case ExpressionType::MON_EXP: {
            // std::cout << "mon" << std::endl;
            MonadicExpression * mon_exp = dynamic_cast<MonadicExpression*>(exp);
            res += "MonadicExp(";
            switch (mon_exp->get_type()) {
                case MonadicOperator::IntNegOp: 
                    res += "IntNegOp, ";
                    break;
                case MonadicOperator::PrintOp: 
                    res += "Print, ";
                    break;
                case MonadicOperator::SizeOp: 
                    res += "Size, ";
                    break;
                case MonadicOperator::NotOp:
                    res += "NotOp, ";
                    break;
            };
            res += string_of_expression(mon_exp->get_right());
            res += ")";
            break;
        }
        case ExpressionType::LET_EXP: {
            AssignmentExpression * let_exp = dynamic_cast<AssignmentExpression*>(exp);
            res = let_exp->is_reassign() ? "ReassignExp(" + let_exp->get_id() + ", " : "LetExp(" + let_exp->get_id() + ", ";
            res += string_of_expression(let_exp->get_right());
            res +=  ")";
            break;
        }
    };

    return res;

}

void print_evaluated_list(std::vector<Value> arr) {
    std::cout << "[";
    bool first = true;
    for (Value v : arr) {
        if (!first) std::cout << ", ";
        if (std::holds_alternative<int>(v.data)) {
            int i = std::get<int>(v.data);
            std::cout << i;
        } else if (std::holds_alternative<bool>(v.data)) {
            bool b = std::get<bool>(v.data);
            std::string bool_str = b ? "true" : "false";
            std::cout << bool_str;
        } else if (std::holds_alternative<std::string>(v.data)) {
            std::string s = std::get<std::string>(v.data);
            std::cout << s;
        } else {
            std::vector<Value> sub_arr = std::get<std::vector<Value>>(v.data);
            print_evaluated_list(sub_arr);
        }
        first = false;
    }
    std::cout << "]";
}

std::string multiply(std::string str, int m) {
    std::string res = "";
    while (m > 0) {
        res += str;
        m -= 1;
    }
    return res;
}

std::vector<Value> multiply(std::vector<Value> arr, int m) {
    std::vector<Value> res;

    while (m > 0) {
        res.insert(res.end(), arr.begin(), arr.end());
        m -= 1;
    }
    return res;
}

void cleanup_expressions(std::vector<Expression*> expressions) {
    for (Expression *e : expressions) {
        delete e;
    }

    expressions.clear();
}