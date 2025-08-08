#include <string>
#include <set>

std::set<std::string> _builtin_functions = {"push", "remove"};

void _builtin_push(std::vector<Value>& v, Value e) {
    v.push_back(e);
}

void _builtin_remove(std::vector<Value>& v, int idx) {
    if (idx < 0 || idx >= v.size()) { throw std::runtime_error("idx out of range for remove()"); }
    v.erase(v.begin() + idx);
}

bool is_builtin_func(std::string func_name) {
    return _builtin_functions.find(func_name) != _builtin_functions.end();
}