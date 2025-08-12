#include "lexer.hpp"
#include "utils.hpp"

#include <utility>
#include <map>
#include <regex>

const std::map<char, TokenType> Lexer::single_chars = {
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

const std::map<std::string, TokenType> Lexer::keyword_tokens = {
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

Lexer::Lexer(const std::string& src): source(src) {}

bool Lexer::in_bounds() const { return current < source.size(); }

std::vector<Token> Lexer::generate_tokens() {
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
            Token new_token = Token(single_chars.at(source[current]), std::string(1, source[current]), line);
            tokens.push_back(new_token);
            current += 1;
            start += 1;

        } else {
            std::string curr_string = "";
            // unsigned curr_len = 0;
            while (in_bounds() && !utils::whitespace(source[current]) && (single_chars.find(source[current]) == single_chars.end())) {
                char c = source[current];
                curr_string.push_back(c);
                // curr_len += 1;
                current += 1;
                
            }

            static const std::regex integer_regex("[0-9]+");
            static const std::regex neg_integer_regex("-[0-9]+");
            static const std::regex identifier_regex("[a-z][a-zA-Z0-9_]*");
            
            if (keyword_tokens.find(curr_string) != keyword_tokens.end()) { 
                Token new_token = Token(keyword_tokens.at(curr_string), curr_string, line);
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