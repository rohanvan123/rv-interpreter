#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include "arithmetic_parser.cpp"
#include "parser.cpp"
#include "evaluator.cpp"

void print_tokens_backend(const std::vector<Token>& tokens) {
    for (size_t i = 0; i < tokens.size() - 1; i++) {
        std::cout << token_to_string(tokens[i]) << ",";
    }
    std::cout << token_to_string(tokens[tokens.size() - 1]) << std::endl;
}

void print_commands(const std::vector<std::vector<Token>>& sequence) {
    std::cout << "[" << std::endl;
    for (size_t i = 0; i < sequence.size() - 1; i++) {
        std::cout << "  ";
        print_tokens(sequence[i]);
    }

    std::cout << "  ";
    print_tokens(sequence[sequence.size() - 1]);
    std::cout << "]" << std::endl;
}

std::vector<std::vector<Token>> generate_sequence(std::vector<Token>& tokens) {
    std::vector<std::vector<Token>> sequence;
    std::vector<Token> curr;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].get_type() == SEMI) {
            sequence.push_back(curr);
            curr.clear();
        } else {
            curr.push_back(tokens[i]);
        }
    }

    return sequence;
}

int main(int argc, char *argv[]) {
    // std::cout << "test" << std::endl;
    std::string input_str;
    std::ifstream program_file (argv[1]);
    // std::string buffer(std::istream_iterator<char>(program_file), std::istream_iterator<char>());
    
    std::string buffer;
    std::string line;
    while (std::getline(program_file, line)) {
        buffer += line + "\n"; // Append each line to the buffer with a newline character
    }
    
    // std::cout << buffer << std::endl;
    Lexer lex(buffer);
    std::vector<Token> tokens = lex.generate_tokens();
    print_tokens_backend(tokens);
    std::vector<std::vector<Token>> sequence = generate_sequence(tokens);
    size_t idx = 0;

    Parser np(tokens);

    std::vector<Expression*> expressions = np.parse_top_level_expressions();

    // for (size_t i = 0; i < sequence.size(); i++) {
    //     Parser parser(sequence[i]);
    //     Expression * abstract_syntax_tree = parser.parse();
    //     expressions.push_back(abstract_syntax_tree);
    //     std::string exp_str = string_of_expression(abstract_syntax_tree);
    //     std::cout << exp_str << std::endl;
    // }

    for (size_t i = 0; i < expressions.size(); i++) {
        Expression * abstract_syntax_tree = expressions[i];
        std::string exp_str = string_of_expression(abstract_syntax_tree);
        std::cout << exp_str << std::endl;
    }

    Evaluator evaluator;
    evaluator.evaluate_commands(expressions);
    
}
