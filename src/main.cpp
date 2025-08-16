#include <string>
#include <fstream>
#include <vector>
#include <stack>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "utils.hpp"
#include "ir_generator.hpp"

const std::string DELIMITER = "=================================";

void print_lexer_output(const std::vector<Token>& tokens) {
    utils::print_tokens_by_line(tokens);
}

void print_parser_output(const std::vector<Expression*>& expressions) {
    for (size_t i = 0; i < expressions.size(); i++) {
        Expression* abstract_syntax_tree = expressions[i];
        std::string exp_str = utils::string_of_expression(abstract_syntax_tree);
        std::cout << exp_str << std::endl;
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    
    std::string filename = argv[1];
    bool output_lexer = argc >= 3 && strcmp(argv[2], "--output-lexer") == 0;
    bool output_parser = argc >= 4 && strcmp(argv[3], "--output-parser") == 0;

    std::string buffer = utils::read_file_into_buffer(argv[1]);

    Lexer lex(buffer);
    std::vector<Token> tokens = lex.generate_tokens();

    if (output_lexer) {
        print_lexer_output(tokens);
        std::cout << DELIMITER << std::endl;
    }

    Parser np(tokens);
    std::vector<Expression*> expressions = np.parse_top_level_expressions();

    if (output_parser) {
        print_parser_output(expressions);
        std::cout << DELIMITER << std::endl;
    }

    // Evaluator evaluator;
    // evaluator.evaluate_commands(expressions);

    IRGenerator gen;
    std::vector<Instruction> instr = gen.generate_ir_code(expressions);
    gen.print_instructions();
    
    utils::cleanup_expressions(expressions);
}
