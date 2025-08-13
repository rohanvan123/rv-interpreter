#include <string>
#include <fstream>
#include <vector>
#include <stack>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "utils.hpp"

const std::string DELIMITER = "=================================";

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    
    std::string filename = argv[1];
    std::string buffer = utils::read_file_into_buffer(argv[1]);

    Lexer lex(buffer);
    std::vector<Token> tokens = lex.generate_tokens();

    utils::print_tokens_by_line(tokens);
    std::cout << DELIMITER << std::endl;

    Parser np(tokens);
    std::vector<Expression*> expressions = np.parse_top_level_expressions();

    for (size_t i = 0; i < expressions.size(); i++) {
        Expression* abstract_syntax_tree = expressions[i];
        std::string exp_str = utils::string_of_expression(abstract_syntax_tree);
        std::cout << exp_str << std::endl;
    }
    std::cout << DELIMITER << std::endl;

    Evaluator evaluator;
    evaluator.evaluate_commands(expressions);
    
    utils::cleanup_expressions(expressions);
}
