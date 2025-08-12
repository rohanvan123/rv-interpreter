#ifndef UTILS_HPP
#define UTILS_HPP

namespace utils {

void print_tokens(const std::vector<Token>& tokens);
void print_tokens_backend(const std::vector<Token>& tokens);
void print_tokens_by_line(const std::vector<Token>& tokens);
void print_commands(const std::vector<std::vector<Token>>& sequence);

std::vector<std::vector<Token>> generate_sequence(std::vector<Token>& tokens);

std::string read_file_into_buffer(char *filepath);

std::set<ExpressionType> returnable_exps = {
    ExpressionType::CONST_EXP,
    ExpressionType::VAR_EXP,
    ExpressionType::BIN_EXP,
    ExpressionType::MON_EXP,
    ExpressionType::LIST_EXP,
    ExpressionType::LIST_ACCESS_EXP,
    ExpressionType::FUNC_CALL_EXP,
    ExpressionType::EMPTY_EXP,
};

std::string string_of_expression(Expression* exp);

void print_evaluated_list(std::vector<Value> arr);

std::string multiply(std::string str, int m);

std::vector<Value> multiply(std::vector<Value> arr, int m);

void cleanup_expressions(std::vector<Expression*> expressions);

} // namespace utils

#endif // UTILS_HPP