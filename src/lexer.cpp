#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <map>
#include <regex>
#include <variant>

using namespace std;

// Lexer (sequence of characters) -> sequence of tokens

// tokens
// Identifiers
// Strings
// Integers
// End of File

enum TokenType {
    // Single Characters
    SEMI,
    EQUALS,
    LEFT_PAREN,
    RIGHT_PAREN,
    PLUS,
    TIMES,
    MINUS,
    DIVIDES,
    MOD,
    POW,
    RBRACE,
    LBRACE,
    RBRACKET,
    LBRACKET,
    GT,
    LT,
    COMMA,
    END_BLOCK,

    // Two Characters
    NEQ,
    GEQ,
    LEQ,
    EQUALITY,
    AND,
    OR,
    PLUS_EQUALS,
    MINUS_EQUALS,
    TIMES_EQUALS,
    DIVIDES_EQUALS,
    MOD_EQUALS,

    // Literals
    IDENTIFIER,
    INTEGER,
    STRING,

    // Key Words
    TRUE,
    FALSE,
    PRINT,
    SIZE,
    LET,
    WHILE,
    IF,
    ELSE,
    FUNCTION,
    RETURN,
    END_OF_FILE
};

class Token {
    public:
        Token(TokenType t, std::string l, int ln) {
            type = t;
            lexeme = l;
            line = ln;
        }
        TokenType get_type() const {
            return type;
        }
        std::string get_string() const {
            return lexeme;
        }
        int get_line() {
            return line;
        }
    private:
        TokenType type;
        std::string lexeme;
        int line;
};

struct TokenInfo {
    TokenType token;
};

// Forward declaration
struct TokenBlock;

using TokenOrBlock = std::variant<Token, TokenBlock>;
struct TokenBlock {
    std::vector<TokenOrBlock> contents;
};


std::string token_to_string(Token token) {
    switch (token.get_type()) {
        case SEMI: return "SEMI";
        case EQUALS: return "EQUALS";
        case RIGHT_PAREN: return "RIGHT_PAREN";
        case LEFT_PAREN: return "LEFT_PAREN";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case TIMES: return "TIMES";
        case DIVIDES: return "DIVIDES";
        case POW: return "POW";
        case PLUS_EQUALS: return "PLUS_EQUALS";
        case MINUS_EQUALS: return "MINUS_EQUALS";
        case TIMES_EQUALS: return "TIMES_EQUALS";
        case DIVIDES_EQUALS: return "PLUS_EQUALS";
        case MOD_EQUALS: return "MOD_EQUALS";
        case MOD: return "MOD";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case GT: return "GT";
        case LT: return "LT";
        case COMMA: return "COMMA";
        case END_BLOCK: return "END_BLOCK";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";

        case NEQ: return "NEQ";
        case LEQ: return "LEQ";
        case GEQ: return "GEQ";
        case EQUALITY: return "EQUALITY";
        case AND: return "AND";
        case OR: return "OR";

        case TRUE: return "BOOL true";
        case FALSE: return "BOOL false";

        case IDENTIFIER: return "IDENT " + token.get_string();
        case INTEGER: return "INT " + token.get_string();
        case STRING: return "STRING \"" + token.get_string() + "\"";
        case PRINT: return "PRINT";
        case SIZE: return "SIZE";
        case LET: return "LET";
        case WHILE: return "WHILE";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case FUNCTION: return "FUNCTION";
        case RETURN: return "RETURN";
        case END_OF_FILE: return "EOF";
        default: return "UNIDENTIFIED TOKEN " + token.get_string();
    }
}

bool whitespace(char c) {
    return isspace(c) || c == '\n';
}

class Lexer {
    public:
        Lexer(std::string src) {
            source = src;
            start = 0;
            current = 0;
            line = 1;
            split_idx = 0;
        }

        bool in_bounds() {
            return current < source.size();
        }


        std::vector<Token> generate_tokens() {
            while (in_bounds()) {
                if (source[current] == ' ') {
                    current += 1;
                    start += 1;
                } else if (source[current] == '\n') {
                    current += 1;
                    start += 1;
                    line += 1;

                } else if (source[current] == '"') {
                    // STRING
                    current += 1;
                    std::string lexeme = "";
                    while (in_bounds() && source[current] != '"') {
                        lexeme += source[current];
                        current += 1;
                    }
                    Token new_token = Token(STRING, lexeme, line);
                    tokens.push_back(new_token);

                    current += 1;
                    start += 1;
                } else if (single_chars.find(source[current]) != single_chars.end()) {
                    Token new_token = Token(single_chars[source[current]], std::string(1, source[current]), line);
                    tokens.push_back(new_token);
                    current += 1;
                    start += 1;

                } else {
                    std::string curr_string = "";
                    unsigned curr_len = 0;
                    while (in_bounds() && !whitespace(source[current]) && (single_chars.find(source[current]) == single_chars.end())) {
                        char c = source[current];
                        curr_string.push_back(c);
                        curr_len += 1;
                        current += 1;
                        
                    }

                    static const std::regex integer_regex("[0-9]+");
                    static const std::regex neg_integer_regex("-[0-9]+");
                    static const std::regex identifier_regex("[a-z][a-zA-Z0-9_]*");
                    
                    if (keyword_tokens.find(curr_string) != keyword_tokens.end()) { 
                        Token new_token = Token(keyword_tokens[curr_string], curr_string, line);
                        tokens.push_back(new_token);

                    } else if (std::regex_match(curr_string, integer_regex)) {
                        Token new_token = Token(INTEGER, curr_string, line);
                        tokens.push_back(new_token);

                    } else if (std::regex_match(curr_string, neg_integer_regex)) {
                        Token neg_token = Token(MINUS, "-", line);
                        Token new_token = Token(INTEGER, curr_string.substr(1), line);
                        tokens.push_back(neg_token);
                        tokens.push_back(new_token);

                    } else if (std::regex_match(curr_string, identifier_regex)) {
                        Token new_token = Token(IDENTIFIER, curr_string, line);
                        tokens.push_back(new_token);
                    }
                        
                    start = current;                    
                }
            }

            Token new_token = Token(END_OF_FILE, "EOF", line);
            tokens.push_back(new_token);
            return tokens;
        }
        
    private:
        std::map<char, TokenType> single_chars = {
            {';', SEMI},
            {'(', LEFT_PAREN},
            {')', RIGHT_PAREN},
            {'{', LBRACE},
            {'}', RBRACE},
            {',', COMMA},
            {'$', END_BLOCK},
            {'[', LBRACKET},
            {']', RBRACKET}
        };

        std::map<std::string, TokenType> keyword_tokens = {
            {"<", LT},
            {">", GT},
            {"=", EQUALS},
            {"+", PLUS},
            {"*", TIMES},
            {"-", MINUS},
            {"/", DIVIDES},
            {"%", MOD},
            {"let", LET},
            {"while", WHILE},
            {"print", PRINT},
            {"size", SIZE},
            {"+=", PLUS_EQUALS},
            {"-=", MINUS_EQUALS},
            {"*=", TIMES_EQUALS},
            {"/=", DIVIDES_EQUALS},
            {"^", POW},
            {"%=", MOD_EQUALS},
            {">=", GEQ},
            {"!=", NEQ},
            {"==", EQUALITY},
            {"&&", AND},
            {"||", OR},
            {"true", TRUE},
            {"false", FALSE},
            {"if", IF},
            {"else", ELSE},
            {"function", FUNCTION},
            {"return", RETURN},
        };

        std::vector<Token> tokens;
        std::string source;
        size_t start;
        size_t current;
        size_t split_idx;
        int line;
};

// int main(int argc, char *argv[]) {
//     // std::cout << "test" << std::endl;
//     std::string input_str;
//     std::ifstream program_file (argv[1]);
//     // std::string buffer(std::istream_iterator<char>(program_file), std::istream_iterator<char>());
    
//     std::string buffer;
//     std::string line;
//     while (std::getline(program_file, line)) {
//         buffer += line + "\n"; // Append each line to the buffer with a newline character
//     }
    
//     // std::cout << buffer << std::endl;
//     Lexer lex(buffer);
//     std::vector<Token> tokens = lex.generate_tokens();
//     std::cout << "[";
//     for (size_t i = 0; i < tokens.size() - 1; i++) {
//         std::cout << token_to_string(tokens[i]) << ", ";
//     }
//     std::cout << token_to_string(tokens[tokens.size() - 1]) << "]" << std::endl;
//     return 0;
// }
