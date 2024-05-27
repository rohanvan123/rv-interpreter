#include <map>
#include <vector>
#include <variant>
#include "expression.h"

using AstValue = std::variant<int, bool, std::string>;
using Environment = std::map<std::string, AstValue>;

class Interpreter {
    public:
        // Interpreter();
        // ~Interpreter();

        void evaluate_commands(std::vector<Expression*> commands) {
            Environment env;
            for (Expression * exp : commands) {
                auto result = evaluate_expression(exp, env);
                if (std::holds_alternative<Environment>(result)) {
                    Environment new_env = std::get<Environment>(result);
                    env = new_env;
                } else {
                    // Value v = std::get<Value>(result);
                }
            }
        }
    private:

        std::variant<Environment, AstValue> evaluate_expression(Expression * exp, Environment env) {

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
                    // std::cout << "var" << std::endl;
                    // std::cout << "its variable" << std::endl;
                    VarExp * var_exp = dynamic_cast<VarExp*>(exp);
                    std::string var_name = var_exp->get_var_name();
                    if (env.find(var_name) != env.end()) {
                        return env[var_name];
                    } else {
                        throw std::runtime_error("Error identifier " + var_name + " does not exist in store");
                    }
                }
                case ExpressionType::IF_EXP: {
                    // std::cout << "if" << std::endl;
                    IfExpression * if_statement = dynamic_cast<IfExpression*>(exp);
                    auto raw_val = evaluate_expression(if_statement->get_conditional(), env);
                    if (std::holds_alternative<AstValue>(raw_val)) {
                        AstValue val = std::get<AstValue>(raw_val);
                        if (std::holds_alternative<bool>(val)) {
                            bool b = std::get<bool>(val);
                            // std::cout << b << std::endl;
                            if (b == true) {
                                return evaluate_expression(if_statement->get_if_exp(), env);
                            } else {
                                return evaluate_expression(if_statement->get_else_exp(), env);
                            }

                        }
                    }
                }
                case ExpressionType::BIN_EXP: {
                    // std::cout << "bin" << std::endl;
                    BinaryExpression * bin_exp = dynamic_cast<BinaryExpression*>(exp);
                    auto v1 = evaluate_expression(bin_exp->get_left(), env);
                    auto v2 = evaluate_expression(bin_exp->get_right(), env);
                    if (std::holds_alternative<AstValue>(v1) && std::holds_alternative<AstValue>(v2)) {
                        AstValue va1 = std::get<AstValue>(v1);
                        AstValue va2 = std::get<AstValue>(v2);
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
                            };
                        }
                    }
                }
                case ExpressionType::MON_EXP: {
                    // std::cout << "mon" << std::endl;
                    MonadicExpression * mon_exp = dynamic_cast<MonadicExpression*>(exp);
                    auto v = evaluate_expression(mon_exp->get_right(), env);
                    if (std::holds_alternative<AstValue>(v)) {
                        AstValue val = std::get<AstValue>(v);
                        if (std::holds_alternative<std::string>(val)) {

                        } else if (std::holds_alternative<int>(val)) {
                            int i = std::get<int>(val);
                            switch (mon_exp->get_type()) {
                                case MonadicOperator::IntNegOp: return -1 * i;
                                case MonadicOperator::PrintOp: {
                                    std::cout << i << std::endl;
                                }
                                return 0;
                            };
                        } else if (std::holds_alternative<bool>(val)) {
                            bool b = std::get<bool>(val);
                            switch (mon_exp->get_type()) {
                                case MonadicOperator::NotOp: return !b;
                                case MonadicOperator::PrintOp: {
                                    std::string bool_str = b ? "true" : "false";
                                    std::cout << bool_str << std::endl;
                                }
                                return 0;
                            };
                            
                        }
                    }
                    break;
                }
                case ExpressionType::LET_EXP: {
                    // std::cout << "let" << std::endl;
                    AssignmentExpression * let_exp = dynamic_cast<AssignmentExpression*>(exp);
                    auto v = evaluate_expression(let_exp->get_right(), env);
                    std::string var_name = let_exp->get_id();
                    if (std::holds_alternative<AstValue>(v)) {
                        AstValue val = std::get<AstValue>(v);
                        env[var_name] = val;
                        return env;
                    }
                }
            };
        }
};
