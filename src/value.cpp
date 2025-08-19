#include "value.hpp"
#include "utils.hpp"

std::string Value::string_of_list(std::vector<Value> arr, bool string_quotes) const {
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (Value v : arr) {
        if (!first) oss << ", ";
        oss << v.to_string(string_quotes);
        first = false;
    }
    oss << "]";
    return oss.str();
}


std::string Value::to_string(bool string_quotes) const {
    if (std::holds_alternative<int>(data)) {
        int i = std::get<int>(data);
        return std::to_string(i);
    } else if (std::holds_alternative<bool>(data)) {
        bool b = std::get<bool>(data);
        return b ? "true" : "false";
    } else if (std::holds_alternative<std::string>(data)) {
        std::string s = std::get<std::string>(data);
        return string_quotes ? "\"" + s + "\"" : s;
    } else if ((std::holds_alternative<std::vector<Value>>(data))) {
        std::vector<Value> v = std::get<std::vector<Value>>(data);
        return string_of_list(v, string_quotes);

    }

    return "UNKOWN VALUE";
};

std::string Value::get_type() const {
    if (is_int()) return "int";
    if (is_bool()) return "bool";
    if (is_string()) return "string";
    if (is_list()) return "list";

    return "UNKOWN TYPE";
}

Value Value::operator+(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 + val2);
    } else if (is_string() && rhs.is_string()) {
        std::string s1 = std::get<std::string>(data);
        std::string s2 = std::get<std::string>(rhs.data);
        return Value(s1 + s2);
    } else if (is_list() && rhs.is_list()) {
        std::vector<Value> v1 = std::get<std::vector<Value>>(data);
        std::vector<Value> v2 = std::get<std::vector<Value>>(rhs.data);
        std::vector<Value> res(v1); 

        res.insert(res.end(), v2.begin(), v2.end());
        return Value(res);
    }
    throw std::runtime_error("incorrect types for + operator");
}

Value Value::operator-(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 - val2);
    }
    throw std::runtime_error("incorrect types for - operator");
}

Value Value::operator*(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 * val2);
    } else if (is_string() && rhs.is_int()) {
        std::string str = std::get<std::string>(data);
        int multiplier = std::get<int>(rhs.data);
        return Value(utils::multiply(str, multiplier));
    } else if (is_list() && rhs.is_int()) {
        std::vector<Value> v1 = std::get<std::vector<Value>>(data);
        int multiplier = std::get<int>(rhs.data);
        return Value(utils::multiply(v1, multiplier));
    }
    throw std::runtime_error("incorrect types for * operator " + get_type() + " " + rhs.get_type());
}

Value Value::operator/(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 / val2);
    }
    throw std::runtime_error("incorrect types for / operator");
}

Value Value::pow(const Value& exp) const {
    if (is_int() && exp.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(exp.data);
        int res = static_cast<int>(std::pow(val1, val2));
        return Value(res);
    }
    throw std::runtime_error("incorrect types for ^ operator");
}

Value Value::operator%(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 % val2);
    }
    throw std::runtime_error("incorrect types for mod operator");  
}

Value Value::operator>(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 > val2);
    }
    throw std::runtime_error("incorrect types for > operator");  
}

Value Value::operator>=(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 >= val2);
    }
    throw std::runtime_error("incorrect types for >= operator");  
}

Value Value::operator<(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 < val2);
    }
    throw std::runtime_error("incorrect types for < operator");  
}

Value Value::operator<=(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return Value(val1 <= val2);
    }
    throw std::runtime_error("incorrect types for <= operator");  
}

bool Value::equals(const Value& rhs) const {
    if (is_int() && rhs.is_int()) {
        int val1 = std::get<int>(data);
        int val2 = std::get<int>(rhs.data);
        return val1 == val2;
    } else if (is_string() && rhs.is_string()) {
        std::string s1 = std::get<std::string>(data);
        std::string s2 = std::get<std::string>(rhs.data);
        return s1 == s2;
    } else if (is_bool() && rhs.is_bool()) {
        bool b1 = std::get<bool>(data);
        bool b2 = std::get<bool>(rhs.data);
        return b1 == b2;
    } if (is_list() && rhs.is_list()) {
        std::vector<Value> v1 = std::get<std::vector<Value>>(data);
        std::vector<Value> v2 = std::get<std::vector<Value>>(rhs.data);

        if (v1.size() != v2.size()) return false;
        for (size_t i = 0; i < v1.size(); i++) {
            if (v1[1].not_equals(v2[i])) return false;
        }
        return true;
    }
    throw std::runtime_error("incorrect types for ==/!= operators"); 
}
bool Value::not_equals(const Value& rhs) const {
    return !this->equals(rhs);
}

Value Value::operator==(const Value& rhs) const {
    return Value(this->equals(rhs));
}

Value Value::operator!=(const Value& rhs) const {
    return Value(this->not_equals(rhs));
}

Value Value::operator&&(const Value& rhs) const {
    if (is_bool() && rhs.is_bool()) {
        bool val1 = std::get<bool>(data);
        bool val2 = std::get<bool>(rhs.data);
        return Value(val1 && val2);
    }
    throw std::runtime_error("incorrect types for && operator");
}

Value Value::operator||(const Value& rhs) const {
    if (is_bool() && rhs.is_bool()) {
        bool val1 = std::get<bool>(data);
        bool val2 = std::get<bool>(rhs.data);
        return Value(val1 || val2);
    }
    throw std::runtime_error("incorrect types for || operator");
}

Value Value::operator-() const {
    if (is_int()) {
        int val1 = std::get<int>(data);
        return Value(-1 * val1);
    }
    throw std::runtime_error("incorrect type for - operator");
}

Value Value::operator!() const {
    if (is_bool()) {
        bool b = std::get<bool>(data);
        return Value(!b);
    }
    throw std::runtime_error("incorrect type for ! operator");    
}

Value Value::operator[](const Value& idx_val) const {
    if (!idx_val.is_int()) throw std::runtime_error("index for [] is not an int");
    int idx = std::get<int>(idx_val.data);

    if (is_string()) {
        std::string s = std::get<std::string>(data);
        if (idx < 0 || idx >= s.size()) throw std::runtime_error("idx out of bounds for string[]");
        return Value(std::string(1, s[idx]));
    } else if (is_list()) {
        std::vector<Value> arr = std::get<std::vector<Value>>(data);
        if (idx < 0 || idx >= arr.size()) throw std::runtime_error("idx out of bounds for list[]");
        return Value(arr[idx]);
    }

    throw std::runtime_error("incorrect type for [] operator");  
}

Value Value::modify_arr(const Value& idx_val, Value replace_val) {
    if (!idx_val.is_int()) throw std::runtime_error("index for arr[] = ... is not an int");
    int idx = std::get<int>(idx_val.data);

    if (is_string() && replace_val.is_string()) {
        std::string s(std::get<std::string>(data));
        std::string c = (std::get<std::string>(replace_val.data));
        if (idx < 0 || idx >= s.size()) throw std::runtime_error("idx out of bounds for string[]");
        if (c.size() != 1) throw std::runtime_error("value for string[i] = x is not a single char");

        s[idx] = c[0];
        return Value(s);
    } else if (is_list()) {
        std::vector<Value> arr(std::get<std::vector<Value>>(data));
        if (idx < 0 || idx >= arr.size()) throw std::runtime_error("idx out of bounds for list[]");

        arr[idx] = replace_val;
        return Value(arr);
    }

    throw std::runtime_error("incorrect type for [] = ... operator"); 
}

Value Value::size() const {
    if (is_string()) {
        std::string s = std::get<std::string>(data);
        return Value(static_cast<int>(s.size()));
    } else if (is_list()) {
        std::vector<Value> arr = std::get<std::vector<Value>>(data);
        return Value(static_cast<int>(arr.size()));
    }

    throw std::runtime_error("incorrect type for size operator"); 
}

void Value::append_ref(Value e) {
    if (is_list()) {
        std::vector<Value>& arr = std::get<std::vector<Value>>(data);
        arr.push_back(e);
        return;
    }

    throw std::runtime_error("cannot append to type -- " + get_type()); 
}