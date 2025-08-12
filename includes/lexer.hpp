#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"

#include <string>
#include <vector>
#include <map>

// Lexer (sequence of characters) -> sequence of tokens

// tokens
// Identifiers
// Strings
// Integers
// End of File

class Lexer {
    private:
        static const std::map<char, TokenType> single_chars;
        static const std::map<std::string, TokenType> keyword_tokens;

        bool in_bounds() const;

        std::vector<Token> tokens;
        std::string source;
        size_t start = 0;
        size_t current = 0;
        int line = 1;

    public:
        Lexer(const std::string& src);
        std::vector<Token> generate_tokens();
};

#endif // LEXER_HPP