#ifndef ARITHMETIC_PARSER_HPP
#define ARITHMETIC_PARSER_HPP

#include "lexer.hpp"
#include "expression.hpp"
#include "token.hpp"

#include <cstdarg>

class ArithmeticParser {
    private:
        size_t current = 0;
        const std::vector<Token> tokens;

        Token peek();
        bool is_at_end();
        bool check(TokenType type);

        Token previous();
        Token advance();

        bool match(int count, ...);

        Expression * expression();
        Expression * disjunction();
        Expression * conjunction();
        Expression * comparison();
        Expression * term();
        Expression * factor();
        Expression * exponent();
        Expression * unary();
        Expression * atomic();
        
    public:
        ArithmeticParser(const std::vector<Token>& tok);
        Expression * parse();

};

#endif // ARITHMETIC_PARSER_HPP
