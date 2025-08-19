#include "lexer.hpp"
#include "parser.hpp"
#include "tree_evaluator.hpp"
#include "utils.hpp"
#include "ir_generator.hpp"
#include "interpreter.hpp"

#include <string>
#include <vector>
#include <unordered_map>

const std::string DELIMITER = "=================================";

std::unordered_map<std::string, bool> flags = {
    {"--output-lexer", false},
    {"--output-parser", false},
    {"--tree-evaluate", false},
    {"--output-ir", true},
};

void print_lexer_output(const std::vector<Token>& tokens) {
    utils::print_tokens_by_line(tokens);
    std::cout << DELIMITER << "\n";
}

void print_parser_output(const std::vector<Expression*>& expressions) {
    for (size_t i = 0; i < expressions.size(); i++) {
        Expression* abstract_syntax_tree = expressions[i];
        std::string exp_str = utils::string_of_expression(abstract_syntax_tree);
        std::cout << exp_str << "\n";
    }
    std::cout << DELIMITER << "\n";
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    // set flags based on executable args
    for (int i = 2; i < argc; i++) {
        std::string arg_str(argv[i]);
        if (flags.find(arg_str) == flags.end()) {
            std::cerr << "Unkown flag: " << argv[i] << "\n";
            return 1;
        }

        flags[arg_str] = true;
    } 

    // Convert text file into string 
    std::string filename = argv[1];
    std::string buffer = utils::read_file_into_buffer(argv[1]);

    // Convert buffer string into lexical tokens
    Lexer lex(buffer);
    std::vector<Token> tokens = lex.generate_tokens();
    if (flags["--output-lexer"]) print_lexer_output(tokens);

    // convert lexical tokens into tree
    Parser np(tokens);
    std::vector<Expression*> expressions = np.parse_top_level_expressions();
    if (flags["--output-parser"]) print_parser_output(expressions);
    
    if (flags["--tree-evaluate"]) {
        // use TreeEvaluator
        TreeEvaluator evaluator;
        evaluator.evaluate_commands(expressions);
    } else {
        // use RV VM
        IRGenerator gen;
        std::vector<Instruction> instr = gen.generate_ir_code(expressions);
        // gen.print_instructions();
        // gen.print_ident_table();
        // std::cout << DELIMITER << "\n";

        Interpreter interpreter(gen);
        interpreter.execute();
        // interpreter.print_reg_file();
        // interpreter.print_env();
    }
    
    utils::cleanup_expressions(expressions);
}
