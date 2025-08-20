#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

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
    NOT,

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
    private:
        TokenType type;
        std::string lexeme;
        int line;

    public:
        Token(TokenType t, const std::string& l, int ln): 
            type(t),
            lexeme(l),
            line(ln) {}

        TokenType get_type() const { return type; }
        std::string get_string() const { return lexeme; }
        int get_line() const { return line; }

        std::string to_string() const {
            switch (get_type()) {
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
                case NOT: return "NOT";

                case NEQ: return "NEQ";
                case LEQ: return "LEQ";
                case GEQ: return "GEQ";
                case EQUALITY: return "EQUALITY";
                case AND: return "AND";
                case OR: return "OR";

                case TRUE: return "BOOL true";
                case FALSE: return "BOOL false";

                case IDENTIFIER: return "IDENT " + get_string();
                case INTEGER: return "INT " + get_string();
                case STRING: return "STRING \"" + get_string() + "\"";
                case PRINT: return "PRINT";
                case SIZE: return "SIZE";
                case LET: return "LET";
                case WHILE: return "WHILE";
                case IF: return "IF";
                case ELSE: return "ELSE";
                case FUNCTION: return "FUNCTION";
                case RETURN: return "RETURN";
                case END_OF_FILE: return "EOF";
                default: return "UNIDENTIFIED TOKEN " + get_string();
            }
        }

};

#endif // TOKEN_HPP