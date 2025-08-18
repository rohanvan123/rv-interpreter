#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include "value.hpp"

#include <set>
#include <vector>

namespace builtin {

static const std::set<std::string> _builtin_functions = {"append", "remove", "type", "string"};

Value append(Value v, Value x);
Value remove(Value v, Value x);
Value type(Value v);
Value string(Value v);

bool is_builtin_func(const std::string& func_name);

}


#endif // BUILTINS_HPP