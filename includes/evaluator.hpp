#ifndef EVALUATOR_CPP
#define EVALUATOR_CPP

#include "expression.hpp"

#include <map>
#include <variant>
#include <vector>

using Environment = std::map<std::string, Value>;
using FunctionEnvironment = std::map<std::string, FunctionAssignmentExpression*>;

class Evaluator {
    private:
        Environment env;
        FunctionEnvironment func_env;

        Value evaluate_expression(Expression * exp);

    public:
        void evaluate_commands(const std::vector<Expression*>& commands) {
            for (Expression * exp : commands) {
                auto result = evaluate_expression(exp);
            }
        }
};

#endif // EVALUATOR_CPP