#include "tree_evaluator.hpp"
#include "utils.hpp"
#include "builtins.hpp"

#include <cmath>
#include <sstream>

void TreeEvaluator::push_env() {
    Environment env_copy = *curr_env; // make a copy of the topmost frame
    env_stack.push(env_copy);
    curr_env = &env_stack.top(); // set current frame to the top of the stack (this new frame)
}
void TreeEvaluator::pop_env() {
    env_stack.pop();
    curr_env = &env_stack.top();
}

std::string TreeEvaluator::string_of_env() {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : *curr_env) {
        if (!first) oss << ", ";
        oss << pair.first << ": " << pair.second.to_string();
        first = false;
    }
    oss << "}";
    return oss.str();
}

std::pair<Value, bool> TreeEvaluator::evaluate_expression(Expression * exp) {
    bool returnable = exp->is_returnable();

    switch (exp->get_signature()) {
        case ExpressionType::EMPTY_EXP: {
            return {Value(), returnable};
        }
        case ExpressionType::CONST_EXP: {
            // std::cout << "const" << std::endl;
            ConstExp * const_exp = dynamic_cast<ConstExp*>(exp);

            switch (const_exp->get_type()) {
                case ConstType::BoolConst: return {const_exp->value, returnable};
                case ConstType::IntConst: return {const_exp->value, returnable};
                case ConstType::StringConst: return {const_exp->value, returnable};
            };
        }
        case ExpressionType::VAR_EXP: {
            VarExp * var_exp = dynamic_cast<VarExp*>(exp);
            std::string var_name = var_exp->get_var_name();
            Environment& env = *curr_env;

            if (env.find(var_name) == env.end()) {
                throw std::runtime_error("Error identifier " + var_name + " does not exist in store");
            }

            return {env[var_name], returnable};
        }
        case ExpressionType::BIN_EXP: {
            BinaryExpression * bin_exp = dynamic_cast<BinaryExpression*>(exp);
            Value va1 = evaluate_expression(bin_exp->get_left()).first;
            Value va2 = evaluate_expression(bin_exp->get_right()).first;
            Value res;
            switch (bin_exp->get_type()) {
                case BinaryOperator::IntPlusOp: res = va1 + va2; break;
                case BinaryOperator::IntMinusOp: res = va1 - va2; break;
                case BinaryOperator::IntTimesOp: res = va1 * va2; break;
                case BinaryOperator::IntDivOp: res = va1 / va2; break;
                case BinaryOperator::IntPowOp: res = va1.pow(va2); break;
                case BinaryOperator::ModOp: res = va1 % va2; break;
                case BinaryOperator::GtOp: res = va1 > va2; break;
                case BinaryOperator::GteOp: res = va1 >= va2; break;
                case BinaryOperator::LtOp: res = va1 < va2; break;
                case BinaryOperator::LteOp: res = va1 <= va2; break;
                case BinaryOperator::EqualityOp: res = va1 == va2; break;
                case BinaryOperator::NotEqualsOp: res = va1 != va2; break;
                case BinaryOperator::AndOp: res = va1 && va2; break;
                case BinaryOperator::OrOp: res = va1 || va2; break;
                default: throw std::runtime_error("Incorrect BinOp (int): " + std::to_string(int(bin_exp->get_type())));
            };

            return {res, returnable};
        }
        case ExpressionType::MON_EXP: {
            // std::cout << "mon" << std::endl;
            MonadicExpression * mon_exp = dynamic_cast<MonadicExpression*>(exp);
            Value val = evaluate_expression(mon_exp->get_right()).first;

            switch (mon_exp->get_type()) {
                case MonadicOperator::IntNegOp: return {-val, returnable};
                case MonadicOperator::NotOp: return {!val, returnable};
                case MonadicOperator::PrintOp: std::cout << val.to_string() << "\n"; return {Value(), false}; // cannot return print statement
                case MonadicOperator::SizeOp: return {val.size(), returnable};
                default: throw std::runtime_error("Incorrect MonOp (int): " + std::to_string(int(mon_exp->get_type())));
            };
        }
        case ExpressionType::LET_EXP: {
            // std::cout << "let" << std::endl;
            AssignmentExpression * let_exp = dynamic_cast<AssignmentExpression*>(exp);
            Value val = evaluate_expression(let_exp->get_right()).first;
            std::string var_name = let_exp->get_id();
            Environment& env = *curr_env;
            env[var_name] = val;
            return {val, false};
        }
        case ExpressionType::IF_EXP: {
            IfExpression * if_statement = dynamic_cast<IfExpression*>(exp);
            Value cond_val = evaluate_expression(if_statement->get_conditional()).first;

            if (std::holds_alternative<bool>(cond_val.data)) {
                bool b = std::get<bool>(cond_val.data);
                std::vector<Expression*> branch =
                    b ? if_statement->get_if_exps() : if_statement->get_else_exps();

                Value last_result; // default fallback
                for (Expression* sub_exp : branch) {
                    auto [sub_res, sub_retunable] = evaluate_expression(sub_exp);
                    last_result = sub_res;
                    if (sub_retunable) return {last_result, true};
                }
                return {last_result, false};

            }
        }
        case ExpressionType::WHILE_EXP: {
            // std::cout << "if" << std::endl;
            WhileExpression * while_statment = dynamic_cast<WhileExpression*>(exp);

            Value last_result;
            while (true) {
                Value cond_val = evaluate_expression(while_statment->get_conditional()).first;
                if (!std::holds_alternative<bool>(cond_val.data)) { throw std::runtime_error("While loop condition does not evaluate to bool"); }

                bool b = std::get<bool>(cond_val.data);
                if (!b) break;

                // for (Expression* sub_exp : while_statment->get_body_exps()) {
                //     last_result = evaluate_expression(sub_exp);
                //     if (sub_exp->is_returnable()) {
                //         while_statment->set_returnable(true);
                //         return last_result;
                //     }
                // }

                Value last_result; // default fallback
                for (Expression* sub_exp : while_statment->get_body_exps()) {
                    auto [sub_res, sub_retunable] = evaluate_expression(sub_exp);
                    last_result = sub_res;
                    if (sub_retunable) return {last_result, true};
                }
            }

            return {last_result, false};
        }
        case ExpressionType::FUNC_ASSIGN_EXP: { 
            FunctionAssignmentExpression * func_exp = dynamic_cast<FunctionAssignmentExpression*>(exp);
            func_env[func_exp->get_name()] = func_exp;
            return {Value(), false};
        }
        case ExpressionType::FUNC_CALL_EXP: {
            FunctionCallExpression * func_call_exp = dynamic_cast<FunctionCallExpression*>(exp);
            std::string func_name = func_call_exp->get_name();
            // std::cout << "called " << func_name << " " << string_of_env() <<  " " << curr_env << "\n";

            std::vector<Value> evaluated_args;
            for (Expression* arg_exp : func_call_exp->get_arg_exps()) {
                evaluated_args.push_back(evaluate_expression(arg_exp).first);
            }

            if (builtin::is_builtin_func(func_name)) {
                if (func_name == "append") return {builtin::append(evaluated_args[0], evaluated_args[1]), returnable};
                if (func_name == "remove") return {builtin::remove(evaluated_args[0], evaluated_args[1]), returnable};
                if (func_name == "type") return {builtin::type(evaluated_args[0]), returnable};
                if (func_name == "string") return {builtin::string(evaluated_args[0]), returnable};
                throw std::runtime_error("unknown builtin");
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

            push_env();
            Environment& env = *curr_env;

            // add to environment
            for (size_t i = 0; i < arg_count; i++) {
                std::string arg_name = arg_names[i];
                Value arg_val = evaluated_args[i];
                env[arg_name] = arg_val;
            }

            // std::cout << "added new env " << string_of_env() << "\n";
            Value return_val = Value();
            for (Expression* inner_exp : func_exp->get_body_exps()) {
                // std::cout << utils::string_of_expression(inner_exp) << std::endl;
                auto [inner_val, inner_returnable] = evaluate_expression(inner_exp);
                return_val = inner_val;
                if (inner_returnable) break;
            }

            pop_env();

            return {return_val, returnable};
        }
        case ExpressionType::LIST_EXP: {
            ListExpression * list_statement = dynamic_cast<ListExpression*>(exp);
            std::vector<Value> elements;

            for (Expression * elem_exp : list_statement->get_elements()) {
                Value res = evaluate_expression(elem_exp).first;
                elements.push_back(res);
            }

            return {Value(elements), returnable};
        }
        case ExpressionType::LIST_ACCESS_EXP: {
            ListAccessExpression * access_exp = dynamic_cast<ListAccessExpression*>(exp);
            Value va1 = evaluate_expression(access_exp->get_arr_exp()).first;
            Value va2 = evaluate_expression(access_exp->get_idx_exp()).first;

            if (std::holds_alternative<std::vector<Value>>(va1.data) && std::holds_alternative<int>(va2.data)) {
                std::vector<Value> arr = std::get<std::vector<Value>>(va1.data);
                int idx = std::get<int>(va2.data);

                if (idx < 0 || static_cast<size_t>(idx) >= arr.size()) {
                    throw std::runtime_error("Index out of bounds");
                }

                return {Value(arr[idx]), returnable};

            } else if (std::holds_alternative<std::string>(va1.data) && std::holds_alternative<int>(va2.data)) {
                std::string str = std::get<std::string>(va1.data);
                int idx = std::get<int>(va2.data);

                if (idx < 0 || static_cast<size_t>(idx) >= str.size()) {
                    throw std::runtime_error("Index out of bounds");
                }

                std::string return_char = std::string(1, str[idx]);
                return {Value(return_char), returnable};
            }
        }
        case ExpressionType::LIST_MODIFY_EXP: {
            ListModifyExpression * access_exp = dynamic_cast<ListModifyExpression*>(exp);
            Value va_list = evaluate_expression(access_exp->get_ident_exp()).first;
            Value va1 = evaluate_expression(access_exp->get_idx_exp()).first;
            Value va2 = evaluate_expression(access_exp->get_exp()).first;
            
            if (std::holds_alternative<std::vector<Value>>(va_list.data) && std::holds_alternative<int>(va1.data)) {
                std::vector<Value>& arr = std::get<std::vector<Value>>(va_list.data);
                int idx = std::get<int>(va1.data);

                if (idx < 0 || static_cast<size_t>(idx) >= arr.size()) {
                    throw std::runtime_error("Index out of bounds");
                }
                
                arr[idx] = va2;
                return {Value(arr), false};
            }
        }
    };

    throw std::runtime_error("Unidentified Expression Type");
}
