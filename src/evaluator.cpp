#include <map>
#include <variant>
#include <vector>
#include "expression.h"

using AstValue = std::variant<int, bool, std::string>;
using Environment = std::map<std::string, AstValue>;

class Evaluator {
    public:
        void evaluate_commands(std::vector<Expression*> commands) {
            for (Expression * exp : commands) {
                auto result = evaluate_expression(exp);
            }
        }

    private:
        Environment env;

        AstValue evaluate_expression(Expression * exp) {
            switch (exp->get_signature()) {
                case ExpressionType::CONST_EXP: {
                    // std::cout << "const" << std::endl;
                    ConstExp * const_exp = dynamic_cast<ConstExp*>(exp);
                    
                    switch (const_exp->get_type()) {
                        case ConstType::BoolConst: return const_exp->value.bool_val;
                        case ConstType::IntConst: return const_exp->value.int_val;
                        case ConstType::StringConst: return *const_exp->value.string_val;
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
                    AstValue cond_val = evaluate_expression(if_statement->get_conditional());

                    if (std::holds_alternative<bool>(cond_val)) {
                        bool b = std::get<bool>(cond_val);
                        std::vector<Expression*> branch =
                            b ? if_statement->get_if_exps() : if_statement->get_else_exps();

                        AstValue last_result = 0; // default fallback
                        for (Expression* sub_exp : branch) {
                            last_result = evaluate_expression(sub_exp);
                        }
                        return last_result;

                    }
                }
                case ExpressionType::WHILE_EXP: {
                    // std::cout << "if" << std::endl;
                    WhileExpression * while_statment = dynamic_cast<WhileExpression*>(exp);

                    AstValue last_result = 0;
                    while (true) {
                        AstValue cond_val = evaluate_expression(while_statment->get_conditional());
                        if (std::holds_alternative<bool>(cond_val)) {
                            bool b = std::get<bool>(cond_val);
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
                case ExpressionType::BIN_EXP: {
                    BinaryExpression * bin_exp = dynamic_cast<BinaryExpression*>(exp);
                    AstValue va1 = evaluate_expression(bin_exp->get_left());
                    AstValue va2 = evaluate_expression(bin_exp->get_right());

                    if (std::holds_alternative<int>(va1) && std::holds_alternative<int>(va2)) {
                        int val1 = std::get<int>(va1);
                        int val2 = std::get<int>(va2);
                        switch (bin_exp->get_type()) {
                            case BinaryOperator::IntPlusOp: return val1 + val2;
                            case BinaryOperator::IntMinusOp: return val1 - val2;
                            case BinaryOperator::IntTimesOp: return val1 * val2;
                            case BinaryOperator::IntDivOp: return val1 / val2;
                            case BinaryOperator::ModOp: return val1 % val2;
                            case BinaryOperator::GtOp: return val1 > val2;
                            case BinaryOperator::GteOp: return val1 >= val2;
                            case BinaryOperator::LtOp: return val1 < val2;
                            case BinaryOperator::LteOp: return val1 <= val2;
                            case BinaryOperator::EqualityOp: return val1 == val2;
                            case BinaryOperator::NotEqualsOp: return val1 != val2;
                            default: throw std::runtime_error("Incorrect BinOp (int): " + std::to_string(int(bin_exp->get_type())));
                            return -1;
                        };
                    } else if (std::holds_alternative<bool>(va1) && std::holds_alternative<bool>(va2)) {
                        bool val1 = std::get<bool>(va1);
                        bool val2 = std::get<bool>(va2);

                        switch (bin_exp->get_type()) {
                            case BinaryOperator::EqualityOp: return val1 == val2;
                            case BinaryOperator::NotEqualsOp: return val1 != val2;
                            case BinaryOperator::AndOp: return val1 && val2;
                            case BinaryOperator::OrOp: return val1 || val2;
                            default: throw std::runtime_error("Incorrect BinOp (bool): " + std::to_string(int(bin_exp->get_type())));
                            return -1;

                        };
                    }
                }
                case ExpressionType::MON_EXP: {
                    // std::cout << "mon" << std::endl;
                    MonadicExpression * mon_exp = dynamic_cast<MonadicExpression*>(exp);
                    AstValue val = evaluate_expression(mon_exp->get_right());

                    if (std::holds_alternative<std::string>(val)) {

                    } else if (std::holds_alternative<int>(val)) {
                        int i = std::get<int>(val);
                        switch (mon_exp->get_type()) {
                            case MonadicOperator::IntNegOp: return -1 * i;
                            case MonadicOperator::PrintOp: {
                                std::cout << i << std::endl;
                                return 0;
                            }
                            default: throw std::runtime_error("Incorrect MonOp (int): " + std::to_string(int(mon_exp->get_type())));
                            return -1;
                        };
                    } else if (std::holds_alternative<bool>(val)) {
                        bool b = std::get<bool>(val);
                        switch (mon_exp->get_type()) {
                            case MonadicOperator::NotOp: return !b;
                            case MonadicOperator::PrintOp: {
                                std::string bool_str = b ? "true" : "false";
                                std::cout << bool_str << std::endl;
                                return 0;
                            }
                            default: throw std::runtime_error("Incorrect MonOp (bool): " + std::to_string(int(mon_exp->get_type())));
                            return -1;
                        };
                        
                    }
                    break;
                }
                case ExpressionType::LET_EXP: {
                    // std::cout << "let" << std::endl;
                    AssignmentExpression * let_exp = dynamic_cast<AssignmentExpression*>(exp);
                    AstValue val = evaluate_expression(let_exp->get_right());
                    std::string var_name = let_exp->get_id();

                    env[var_name] = val;
                    return val;
                }
            };
            std::cerr << "Unhandled expression type: " << exp << "\n";
            return -1;
        }
};
