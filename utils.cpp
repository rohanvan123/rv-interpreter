void print_tokens(const std::vector<Token>& tokens) {
    std::cout << "[";
    for (size_t i = 0; i < tokens.size() - 1; i++) {
        std::cout << token_to_string(tokens[i]) << ", ";
    }
    std::cout << token_to_string(tokens[tokens.size() - 1]) << "]" << std::endl;
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
                    std::string bool_str = const_exp->value.bool_val ? "true" : "false";
                    return res + "BoolConst " + bool_str + ')';
                }
                case ConstType::IntConst: return res + "IntConst " + std::to_string(const_exp->value.int_val) + ')';
                case ConstType::StringConst: {
                    std::string * s = const_exp->value.string_val;
                    return  "StringConst " + *s + ')';
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
                case MonadicOperator::NotOp:
                    res += "NotOp, ";
                    break;
            };
            res += string_of_expression(mon_exp->get_right());
            res += ")";
            break;
        }
        case ExpressionType::LET_EXP: {
            // std::cout << "let" << std::endl;
            AssignmentExpression * let_exp = dynamic_cast<AssignmentExpression*>(exp);
            res = let_exp->is_reassign() ? "ReassignExp(" + let_exp->get_id() + ", " : "LetExp(" + let_exp->get_id() + ", ";
            // std::cout << res << std::endl;
            res += string_of_expression(let_exp->get_right());
            // std::cout << res << std::endl;
            res +=  ")";
            break;
        }
    };

    return res;

}