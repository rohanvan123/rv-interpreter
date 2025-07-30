#include <map>
#include <variant>
#include <vector>
#include "expression.h"
// using AstValue = std::variant<int, bool, std::string>;
using Environment = std::map<std::string, Value>;

class Evaluator {
    public:
        void evaluate_commands(std::vector<Expression*> commands) {
            for (Expression * exp : commands) {
                auto result = evaluate_expression(exp);
            }
        }

    private:
        Environment env;

        Value evaluate_expression(Expression * exp) {
            switch (exp->get_signature()) {
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
                        if (std::holds_alternative<bool>(cond_val.data)) {
                            bool b = std::get<bool>(cond_val.data);
                            if (!b) break;

                            for (Expression* sub_exp : while_statment->get_body_exps()) {
                                last_result = evaluate_expression(sub_exp);
                            }

                        } else {
                            throw std::runtime_error("While loop condition does not evaluate to bool");
                        }
                    }

                    return last_result;
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
                            case BinaryOperator::IntTimesOp: return Value(multiply(str, multiplier));
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
                            case BinaryOperator::IntTimesOp: return Value(multiply(v1, v2));
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
                                print_evaluated_list(arr);
                                std::cout << "\n";
                                return Value();
                            }
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
};
