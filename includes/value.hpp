#ifndef VALUE_HPP
#define VALUE_HPP

#include <variant>
#include <vector>
#include <string>
#include <sstream>

class Value;

using ValueData = std::variant<int, bool, std::string, std::vector<Value>>;

class Value {
private:
    bool equals(const Value& rhs) const; // wrapper functions
    bool not_equals(const Value& rhs) const;
public:
    ValueData data;
    Value() = default;
    Value(ValueData vd): data(vd) {}

    bool is_int() const { return std::holds_alternative<int>(data); }
    bool is_bool() const { return std::holds_alternative<bool>(data); }
    bool is_string() const { return std::holds_alternative<std::string>(data); }
    bool is_list() const { return std::holds_alternative<std::vector<Value>>(data); }
    
    std::string to_string() const;
    std::string string_of_list(std::vector<Value> arr) const;
    std::string get_type() const;

    // overloaded ops
    Value operator+(const Value& rhs) const;
    Value operator-(const Value& rhs) const;
    Value operator*(const Value& rhs) const;
    Value operator/(const Value& rhs) const;
    Value pow(const Value& exp) const;
    Value operator%(const Value& rhs) const;
    Value operator>(const Value& rhs) const;
    Value operator>=(const Value& rhs) const;
    Value operator<(const Value& rhs) const;
    Value operator<=(const Value& rhs) const;
    Value operator!=(const Value& rhs) const;
    Value operator==(const Value& rhs) const;
    Value operator&&(const Value& rhs) const;
    Value operator||(const Value& rhs) const;

    Value operator-() const;
    Value operator!() const;
    Value size() const;

    // built ins
};

namespace builtin {
Value append(Value v, Value x);
Value remove(Value v, Value x);
Value type(Value v);
Value string(Value v);

}

#endif // VALUE_HPP