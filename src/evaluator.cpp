#include "evaluator.hpp"
#include "utils.hpp"

#include <cmath>

bool is_builtin_func(const std::string& func_name) {
    static const std::set<std::string> _builtin_functions = {"append", "remove", "type", "string"};
    return _builtin_functions.find(func_name) != _builtin_functions.end();
}

Value Evaluator::evaluate_expression(Expression * exp) {
    switch (exp->get_signature()) {
        case ExpressionType::EMPTY_EXP: {
            return Value();
        }
        case ExpressionType::CONST_EXP: {
            // std::cout << "const" << std::endl;
            ConstExp * const_exp = dynamic_cast<ConstExp*>(exp);
            
            switch (const_exp->get_type()) {
                case ConstType::BoolConst: return const_exp->value;
                case ConstType::IntConst: return const_exp->value;
                case ConstType::StringConst: return const_exp->value;
            };
        }
        case ExpressionType::VAR_EXP: {
            VarExp * var_exp = dynamic_cast<VarExp*>(exp);
            std::string var_name = var_exp->get_var_name();
            if (env.find(var_name) == env.end()) {
                throw std::runtime_error("Error identifier " + var_name + " does not exist in store");
            }

            return env[var_name];
        }
        case ExpressionType::IF_EXP: {
            IfExpression * if_statement = dynamic_cast<IfExpression*>(exp);
            Value cond_val = evaluate_expression(if_statement->get_conditional());

            if (std::holds_alternative<bool>(cond_val.data)) {
                bool b = std::get<bool>(cond_val.data);
                std::vector<Expression*> branch =
                    b ? if_statement->get_if_exps() : if_statement->get_else_exps();

                Value last_result; // default fallback
                for (Expression* sub_exp : branch) {
                    last_result = evaluate_expression(sub_exp);
                    if (sub_exp->is_returnable()) {
                        if_statement->set_returnable(true);
                        break;
                    }
                }
                return last_result;

            }
        }
        case ExpressionType::WHILE_EXP: {
            // std::cout << "if" << std::endl;
            WhileExpression * while_statment = dynamic_cast<WhileExpression*>(exp);

            Value last_result;
            while (true) {
                Value cond_val = evaluate_expression(while_statment->get_conditional());
                if (!std::holds_alternative<bool>(cond_val.data)) { throw std::runtime_error("While loop condition does not evaluate to bool"); }

                bool b = std::get<bool>(cond_val.data);
                if (!b) break;

                for (Expression* sub_exp : while_statment->get_body_exps()) {
                    last_result = evaluate_expression(sub_exp);
                    if (sub_exp->is_returnable()) {
                        while_statment->set_returnable(true);
                        return last_result;
                    }
                }
            }

            return last_result;
        }
        case ExpressionType::FUNC_ASSIGN_EXP: { 
            FunctionAssignmentExpression * func_exp = dynamic_cast<FunctionAssignmentExpression*>(exp);
            func_env[func_exp->get_name()] = func_exp;
            return Value();
        }
        case ExpressionType::FUNC_CALL_EXP: {
            FunctionCallExpression * func_call_exp = dynamic_cast<FunctionCallExpression*>(exp);
            std::string func_name = func_call_exp->get_name();

            std::vector<Value> evaluated_args;
            for (Expression* arg_exp : func_call_exp->get_arg_exps()) {
                evaluated_args.push_back(evaluate_expression(arg_exp));
            }

            if (is_builtin_func(func_name)) {
                if (func_name == "append") {
                    std::vector<Value> vec = std::get<std::vector<Value>>(evaluated_args[0].data);
                    Value v = (evaluated_args[1].data);
                    vec.push_back(v);
                    return Value(vec);
                } else if (func_name == "remove") {
                    std::vector<Value> vec = std::get<std::vector<Value>>(evaluated_args[0].data);
                    int idx = std::get<int>(evaluated_args[1].data);
                    if (idx < 0 || idx >= vec.size()) { throw std::runtime_error("idx out of range for remove()"); }
                    vec.erase(vec.begin() + idx);
                    return Value(vec);
                } else if (func_name == "type") {
                    Value arg = evaluated_args[0];
                    if (std::holds_alternative<int>(arg.data)) {
                        return Value("int");
                    } else if (std::holds_alternative<std::string>(arg.data)) {
                        return Value("string");
                    } else if (std::holds_alternative<bool>(arg.data)) {
                        return Value("bool");
                    } else if (std::holds_alternative<std::vector<Value>>(arg.data)) {
                        return Value("list");
                    } else {
                        return Value("other");
                    }
                } else if (func_name == "string") {
                    Value arg = evaluated_args[0];
                    if (std::holds_alternative<int>(arg.data)) {
                        int a = std::get<int>(arg.data);
                        return Value(std::to_string(a));
                    } else if (std::holds_alternative<std::string>(arg.data)) {
                        return arg.data;
                    } else if (std::holds_alternative<bool>(arg.data)) {
                        bool b = std::get<bool>(arg.data);
                        std::string result = b ? "true" : "false";
                        return Value(result);
                    } else if (std::holds_alternative<std::vector<Value>>(arg.data)) {
                        return Value("list");
                    }
                }
            } else if (func_env.find(func_name) == func_env.end()) {
                throw std::runtime_error("function does not exist");
            }

            FunctionAssignmentExpression * func_exp = func_env[func_name];
        
            if (func_call_exp->get_args_length() != func_exp->get_args_length()) {
                throw std::runtime_error("function call does not have same # of args as declaration");
            }

            // execute the function by adding to env, and then removing
            size_t arg_count = func_call_exp->get_args_length();
            std::vector<std::string> arg_names = func_exp->get_arg_names();
            Value return_val = Value();

            // add to environment
            for (size_t i = 0; i < arg_count; i++) {
                std::string arg_name = arg_names[i];
                Value arg_val = evaluated_args[i];
                env[arg_name] = arg_val;
            }

            for (Expression* inner_exp : func_exp->get_body_exps()) {
                return_val = evaluate_expression(inner_exp);
                if (inner_exp->is_returnable()) break;
            }

            // remove from env
            for (size_t i = 0; i < arg_count; i++) {
                std::string arg_name = arg_names[i];
                env.erase(arg_name);
            }

            return return_val;
        }
        case ExpressionType::LIST_EXP: {
            ListExpression * list_statement = dynamic_cast<ListExpression*>(exp);
            std::vector<Value> elements;

            for (Expression * elem_exp : list_statement->get_elements()) {
                Value res = evaluate_expression(elem_exp);
                elements.push_back(res);
            }

            return Value(elements);
        }
        case ExpressionType::LIST_ACCESS_EXP: {
            ListAccessExpression * access_exp = dynamic_cast<ListAccessExpression*>(exp);
            Value va1 = evaluate_expression(access_exp->get_arr_exp());
            Value va2 = evaluate_expression(access_exp->get_idx_exp());

            if (std::holds_alternative<std::vector<Value>>(va1.data) && std::holds_alternative<int>(va2.data)) {
                std::vector<Value> arr = std::get<std::vector<Value>>(va1.data);
                int idx = std::get<int>(va2.data);

                if (idx < 0 || idx >= arr.size()) {
                    throw std::runtime_error("Index out of bounds");
                }

                return Value(arr[idx]);

            } else if (std::holds_alternative<std::string>(va1.data) && std::holds_alternative<int>(va2.data)) {
                std::string str = std::get<std::string>(va1.data);
                int idx = std::get<int>(va2.data);

                if (idx < 0 || idx >= str.size()) {
                    throw std::runtime_error("Index out of bounds");
                }

                std::string return_char = std::string(1, str[idx]);
                return Value(return_char);
            }
        }
        case ExpressionType::LIST_MODIFY_EXP: {
            ListModifyExpression * access_exp = dynamic_cast<ListModifyExpression*>(exp);
            Value va_list = evaluate_expression(access_exp->get_ident_exp());
            Value va1 = evaluate_expression(access_exp->get_idx_exp());
            Value va2 = evaluate_expression(access_exp->get_exp());
            
            if (std::holds_alternative<std::vector<Value>>(va_list.data) && std::holds_alternative<int>(va1.data)) {
                std::vector<Value>& arr = std::get<std::vector<Value>>(va_list.data);
                int idx = std::get<int>(va1.data);

                if (idx < 0 || idx >= arr.size()) {
                    throw std::runtime_error("Index out of bounds");
                }
                
                arr[idx] = va2;
                return Value(arr);
            }
        }
        case ExpressionType::BIN_EXP: {
            BinaryExpression * bin_exp = dynamic_cast<BinaryExpression*>(exp);
            Value va1 = evaluate_expression(bin_exp->get_left());
            Value va2 = evaluate_expression(bin_exp->get_right());

            if (std::holds_alternative<int>(va1.data) && std::holds_alternative<int>(va2.data)) {
                int val1 = std::get<int>(va1.data);
                int val2 = std::get<int>(va2.data);
                switch (bin_exp->get_type()) {
                    case BinaryOperator::IntPlusOp: return Value(val1 + val2);
                    case BinaryOperator::IntMinusOp: return Value(val1 - val2);
                    case BinaryOperator::IntTimesOp: return Value(val1 * val2);
                    case BinaryOperator::IntDivOp: return Value(val1 / val2);
                    case BinaryOperator::IntPowOp: {
                        int res = static_cast<int>(std::pow(val1, val2));
                        return Value(res);
                    }
                    case BinaryOperator::ModOp: return Value(val1 % val2);
                    case BinaryOperator::GtOp: return Value(val1 > val2);
                    case BinaryOperator::GteOp: return Value(val1 >= val2);
                    case BinaryOperator::LtOp: return Value(val1 < val2);
                    case BinaryOperator::LteOp: return Value(val1 <= val2);
                    case BinaryOperator::EqualityOp: return Value(val1 == val2);
                    case BinaryOperator::NotEqualsOp: return Value(val1 != val2);
                    default: throw std::runtime_error("Incorrect BinOp (int): " + std::to_string(int(bin_exp->get_type())));
                };
            } else if (std::holds_alternative<bool>(va1.data) && std::holds_alternative<bool>(va2.data)) {
                bool val1 = std::get<bool>(va1.data);
                bool val2 = std::get<bool>(va2.data);

                switch (bin_exp->get_type()) {
                    case BinaryOperator::EqualityOp: return Value(val1 == val2);
                    case BinaryOperator::NotEqualsOp: return Value(val1 != val2);
                    case BinaryOperator::AndOp: return Value(val1 && val2);
                    case BinaryOperator::OrOp: return Value(val1 || val2);
                    default: throw std::runtime_error("Incorrect BinOp (bool): " + std::to_string(int(bin_exp->get_type())));

                };
            } 
            else if (std::holds_alternative<std::string>(va1.data) && std::holds_alternative<std::string>(va2.data)) {
                std::string s1 = std::get<std::string>(va1.data);
                std::string s2 = std::get<std::string>(va2.data);
                switch (bin_exp->get_type()) {
                    case BinaryOperator::IntPlusOp: return Value(s1 + s2);
                    default: throw std::runtime_error("Incorrect BinOp (string): " + std::to_string(int(bin_exp->get_type())));
                }
            } 
            else if (std::holds_alternative<std::string>(va1.data) && std::holds_alternative<int>(va2.data)) {
                // String MULTIPLICATION 
                std::string str = std::get<std::string>(va1.data);
                int multiplier = std::get<int>(va2.data);
                switch (bin_exp->get_type()) {
                    case BinaryOperator::IntTimesOp: return Value(utils::multiply(str, multiplier));
                    default: throw std::runtime_error("Incorrect BinOp (string, int): " + std::to_string(int(bin_exp->get_type())));
                }
            } else if (std::holds_alternative<std::vector<Value>>(va1.data) && std::holds_alternative<std::vector<Value>>(va2.data)) {
                std::vector<Value> v1 = std::get<std::vector<Value>>(va1.data);
                std::vector<Value> v2 = std::get<std::vector<Value>>(va2.data);
                switch (bin_exp->get_type()) {
                    case BinaryOperator::IntPlusOp: {
                        v1.insert(v1.end(), v2.begin(), v2.end());
                        return Value(v1);
                    }
                    default: throw std::runtime_error("Incorrect BinOp (list, list): " + std::to_string(int(bin_exp->get_type())));
                }
            } else if (std::holds_alternative<std::vector<Value>>(va1.data) && std::holds_alternative<int>(va2.data)) {
                std::vector<Value> v1 = std::get<std::vector<Value>>(va1.data);
                int v2 = std::get<int>(va2.data);

                switch (bin_exp->get_type()) {
                    case BinaryOperator::IntTimesOp: return Value(utils::multiply(v1, v2));
                    default: throw std::runtime_error("Incorrect BinOp (list, int): " + std::to_string(int(bin_exp->get_type())));
                }
            }
        }
        case ExpressionType::MON_EXP: {
            // std::cout << "mon" << std::endl;
            MonadicExpression * mon_exp = dynamic_cast<MonadicExpression*>(exp);
            Value val = evaluate_expression(mon_exp->get_right());

            if (std::holds_alternative<std::string>(val.data)) {
                std::string s = std::get<std::string>(val.data);
                switch (mon_exp->get_type()) {
                    case MonadicOperator::PrintOp: {
                        std::cout << s << std::endl;
                        return Value();
                    }
                    case MonadicOperator::SizeOp: return Value(static_cast<int>(s.size()));
                    default: throw std::runtime_error("Incorrect MonOp (string): " + std::to_string(int(mon_exp->get_type())));
                }
            } else if (std::holds_alternative<int>(val.data)) {
                int i = std::get<int>(val.data);
                switch (mon_exp->get_type()) {
                    case MonadicOperator::IntNegOp: return Value(-1 * i);
                    case MonadicOperator::PrintOp: {
                        std::cout << i << std::endl;
                        return Value();
                    }
                    default: throw std::runtime_error("Incorrect MonOp (int): " + std::to_string(int(mon_exp->get_type())));
                };
            } else if (std::holds_alternative<bool>(val.data)) {
                bool b = std::get<bool>(val.data);
                switch (mon_exp->get_type()) {
                    case MonadicOperator::NotOp: return Value(!b);
                    case MonadicOperator::PrintOp: {
                        std::string bool_str = b ? "true" : "false";
                        std::cout << bool_str << std::endl;
                        return Value();
                    }
                    default: throw std::runtime_error("Incorrect MonOp (bool): " + std::to_string(int(mon_exp->get_type())));
                };
                
            } else if (std::holds_alternative<std::vector<Value>>(val.data)) {
                std::vector<Value> arr = std::get<std::vector<Value>>(val.data);

                switch (mon_exp->get_type()) {
                    case MonadicOperator::PrintOp: {
                        utils::print_evaluated_list(arr);
                        std::cout << "\n";
                        return Value();
                    }
                    case MonadicOperator::SizeOp: return Value(static_cast<int>(arr.size()));
                    default: throw std::runtime_error("Incorrect MonOp (list): " + std::to_string(int(mon_exp->get_type())));
                };
            }
            break;
        }
        case ExpressionType::LET_EXP: {
            // std::cout << "let" << std::endl;
            AssignmentExpression * let_exp = dynamic_cast<AssignmentExpression*>(exp);
            Value val = evaluate_expression(let_exp->get_right());
            std::string var_name = let_exp->get_id();

            env[var_name] = val;
            return val;
        }
    };

    throw std::runtime_error("Unidentified Expression Type");
}
