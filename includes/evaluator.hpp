#ifndef EVALUATOR_CPP
#define EVALUATOR_CPP

#include "expression.hpp"
#include "types.hpp"

#include <vector>
#include <stack>

class Evaluator {
private:
    // Environment env;
    FunctionEnvironment func_env;

    std::stack<Environment> env_stack;
    Environment* curr_env;
    void push_env();
    void pop_env();

    std::string string_of_env();

    std::pair<Value, bool>  evaluate_expression(Expression * exp);

public:
    Evaluator() { 
        env_stack.push({});
        curr_env = &env_stack.top();
    }

    void evaluate_commands(const std::vector<Expression*>& commands) {
        for (Expression * exp : commands) {
            auto result = evaluate_expression(exp);
        }
    }
};

#endif // EVALUATOR_CPP