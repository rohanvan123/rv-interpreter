#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <map>

// Forward declarations to avoid circular includes
class Value;
class FunctionAssignmentExpression;

using Environment = std::map<std::string, Value>;
using FunctionEnvironment = std::map<std::string, FunctionAssignmentExpression*>;

#endif // TYPES_HPP