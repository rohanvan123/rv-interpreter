#ifndef EVALUATOR_CPP
#define EVALUATOR_CPP

#include "expression.hpp"
#include "types.hpp"

#include <map>
#include <variant>
#include <vector>

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