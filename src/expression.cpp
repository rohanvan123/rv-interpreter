#include "expression.hpp"
#include "utils.hpp"

#include <sstream>

// Blue print for overriding evaluate functions

Value MonadicExpression::evaluate(Environment& env) const {
    Value val = get_right()->evaluate(env);

    auto handle_string = [&](std::string s) -> Value {
        switch (get_type()) {
            case MonadicOperator::PrintOp: {
                std::cout << s << std::endl;
                return Value();
            }
            case MonadicOperator::SizeOp: return Value(static_cast<int>(s.size()));
            default: throw std::runtime_error("Incorrect MonOp (string): " + std::to_string(int(get_type())));
        }
    };

    auto handle_int = [&](int i) -> Value {
        switch (get_type()) {
            case MonadicOperator::IntNegOp: return Value(-1 * i);
            case MonadicOperator::PrintOp: {
                std::cout << i << std::endl;
                return Value();
            }
            default: throw std::runtime_error("Incorrect MonOp (int): " + std::to_string(int(get_type())));
        };

    };

    auto handle_bool = [&](bool b) -> Value {
        switch (get_type()) {
            case MonadicOperator::NotOp: return Value(!b);
            case MonadicOperator::PrintOp: {
                std::string bool_str = b ? "true" : "false";
                std::cout << bool_str << std::endl;
                return Value();
            }
            default: throw std::runtime_error("Incorrect MonOp (bool): " + std::to_string(int(get_type())));
        };
    };

    auto handle_list = [&](std::vector<Value> arr) {
        switch (get_type()) {
            case MonadicOperator::PrintOp: {
                utils::print_evaluated_list(arr);
                std::cout << "\n";
                return Value();
            }
            case MonadicOperator::SizeOp: return Value(static_cast<int>(arr.size()));
            default: throw std::runtime_error("Incorrect MonOp (list): " + std::to_string(int(get_type())));
        };
    };

    if (std::holds_alternative<std::string>(val.data)) {
        return handle_string(std::get<std::string>(val.data));
    } else if (std::holds_alternative<int>(val.data)) {
        return handle_int(std::get<int>(val.data));
    } else if (std::holds_alternative<bool>(val.data)) {
        return handle_bool(std::get<bool>(val.data));
    } else if (std::holds_alternative<std::vector<Value>>(val.data)) {
        return handle_list(std::get<std::vector<Value>>(val.data));
    }

    return Value();
}