#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include "value.hpp"
#include "expression.hpp"

#include <set>
#include <vector>
#include <map>

namespace builtin {

static const std::set<std::string> _builtin_functions = {"append", "remove", "type", "string"};
static const std::map<std::string, int> builtin_to_fid = {
    {"append", -1},
    {"remove", -2},
    {"type", -3},
    {"string", -4},
};

static const std::map<int, std::string> fid_to_builtin = {
    {-1, "append"},
    {-2, "remove"},
    {-3, "type"},
    {-4, "string"},
};

Value append(Value v, Value x);
static auto append_func_exp = FunctionAssignmentExpression("append", {"arr_val", "ele_val"}, {});

Value remove(Value v, Value x);
static auto remove_func_exp = FunctionAssignmentExpression("remove", {"arr_val", "idx_val"}, {});

Value type(Value v);
static auto type_func_exp = FunctionAssignmentExpression("type", {"val"}, {});

Value string(Value v);
static auto string_func_exp = FunctionAssignmentExpression("string", {"val"}, {});

static std::map<std::string, FunctionAssignmentExpression*> builtin_func_exps = {
    {"append", &append_func_exp},
    {"remove", &remove_func_exp},
    {"type", &type_func_exp},
    {"string", &string_func_exp},
};

bool is_builtin_func(const std::string& func_name);

}


#endif // BUILTINS_HPP