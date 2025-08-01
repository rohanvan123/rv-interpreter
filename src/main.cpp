#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include "arithmetic_parser.cpp"
#include "parser.cpp"
#include "evaluator.cpp"

const std::string DELIMITER = "=================================";

int main(int argc, char *argv[]) {
    std::string filename = argv[1];
    std::string buffer = read_file_into_buffer(argv[1]);

    Lexer lex(buffer);
    std::vector<Token> tokens = lex.generate_tokens();
    // print_tokens_backend(tokens);
    print_tokens_by_line(tokens);
    std::cout << DELIMITER << std::endl;

    Parser np(tokens);
    std::vector<Expression*> expressions = np.parse_top_level_expressions();

    for (size_t i = 0; i < expressions.size(); i++) {
        Expression * abstract_syntax_tree = expressions[i];
        std::string exp_str = string_of_expression(abstract_syntax_tree);
        std::cout << exp_str << std::endl;
    }
    std::cout << DELIMITER << std::endl;

    Evaluator evaluator;
    evaluator.evaluate_commands(expressions);
    
    cleanup_expressions(expressions);
}
