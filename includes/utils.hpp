#ifndef UTILS_HPP
#define UTILS_HPP

#include "expression.hpp"
#include "token.hpp"

#include <set>

namespace utils {

std::string read_file_into_buffer(char *filepath);

bool whitespace(char c);

void print_tokens(const std::vector<Token>& tokens);
void print_tokens_by_line(const std::vector<Token>& tokens);

std::string string_of_expression(Expression* exp);

void print_evaluated_list(std::vector<Value> arr);

std::string multiply(std::string str, int m);
std::vector<Value> multiply(std::vector<Value> arr, int m);

void cleanup_expressions(std::vector<Expression*> expressions);

} // namespace utils

#endif // UTILS_HPP