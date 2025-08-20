#include "builtins.hpp"

Value builtin::append(Value v, Value x) {
    // std::cout << v.to_string() << " " << x.to_string() << "\n";
    if (v.is_list()) {
        std::vector<Value> arr = std::get<std::vector<Value>>(v.data);
        arr.push_back(x);
        return Value(arr);
    }
    throw std::runtime_error("incorrect use of append function"); 
}

Value builtin::remove(Value v, Value x) {
    // std::cout << "Remove " << v.to_string() << " " << x.to_string() << "\n";
    if (v.is_list()) {
        std::vector<Value> vec = std::get<std::vector<Value>>(v.data);
        int idx = std::get<int>(x.data);
        if (idx < 0 || static_cast<size_t>(idx) >= vec.size()) { throw std::runtime_error("idx out of range for remove()"); }
        vec.erase(vec.begin() + idx);
        return Value(vec);
    }
    throw std::runtime_error("incorrect use of remove function"); 
}

Value builtin::type(Value v) {
    return Value(v.get_type());
}

Value builtin::string(Value v) {
    if (v.is_list()) return Value("list");
    return Value(v.to_string(false));
}

bool builtin::is_builtin_func(const std::string& func_name) {
    return _builtin_functions.find(func_name) != _builtin_functions.end();
}