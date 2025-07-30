#include "lexer.cpp"
#include "expression.h"
#include <cstdarg>

class ArithmeticParser {
    public:
        ArithmeticParser(const std::vector<Token>& tok) : tokens(tok) { 
            current = 0;
        }

        Token peek() {
            return tokens.at(current);
        }

        bool is_at_end() {
            return current == tokens.size();
        }

        bool check(TokenType type) {
            if (is_at_end()) return false;
            // std::cout << peek().get_type() << " " << type << std::endl;
            // std::cout << "checking ";
            return peek().get_type() == type;
        }

        Token previous() {
            return tokens.at(current - 1);
        }

        Token advance() {
            if (!is_at_end()) current++;
            return previous();
        }

        Expression * parse() {
            return expression();
        }

        Expression *  expression() {
            return comparison();
        }

        Expression * comparison() {
            // std::cout << "comparison - " << current << std::endl;
            Expression * left = term();
            while (match(6, GEQ, LEQ, GT, LT, NEQ, EQUALITY)) {
                Token op = previous();
                Expression * right = term();

                switch (op.get_type()) {
                    case GT: left = new BinaryExpression(BinaryOperator::GtOp, left, right); break;
                    case GEQ: left =  new BinaryExpression(BinaryOperator::GteOp, left, right); break;
                    case LT: left =  new BinaryExpression(BinaryOperator::LtOp, left, right); break;
                    case LEQ: left = new BinaryExpression(BinaryOperator::GteOp, left, right); break;
                    case NEQ: left = new BinaryExpression(BinaryOperator::NotEqualsOp, left, right); break;
                    case EQUALITY: left = new BinaryExpression(BinaryOperator::EqualityOp, left, right); break;
                    default: return nullptr;
                };
                
            }

            return left;
        };

        Expression * term() {
            // std::cout << "term - " << current << std::endl;
            Expression * left = factor();
            while (match(2, PLUS, MINUS)) {
                Token op = previous();
                Expression * right = factor();

                switch (op.get_type()) {
                    case PLUS: left = new BinaryExpression(BinaryOperator::IntPlusOp, left, right); break;
                    case MINUS: left =  new BinaryExpression(BinaryOperator::IntMinusOp, left, right); break;
                    default: return nullptr;
                };
                
            }

            return left;
        }

        Expression * factor() {
            // std::cout << "factor - " << current << std::endl;
            Expression * left = unary();
            while (match(3, TIMES, DIVIDES, MOD)) {
                Token op = previous();
                Expression * right = unary();

                switch (op.get_type()) {
                    case TIMES: left = new BinaryExpression(BinaryOperator::IntTimesOp, left, right); break;
                    case DIVIDES: left =  new BinaryExpression(BinaryOperator::IntDivOp, left, right); break;
                    case MOD: left = new BinaryExpression(BinaryOperator::ModOp, left, right); break;
                    default: return nullptr;
                };
                
            }

            return left;
        }

        Expression * unary() {
            // std::cout << "unary - " << current << std::endl;
            if (match(2, PRINT, MINUS)) {
                Token op = previous();
                Expression * right = unary();

                switch (op.get_type()) {
                    case PRINT: return new MonadicExpression(MonadicOperator::PrintOp, right);
                    case MINUS: return new MonadicExpression(MonadicOperator::IntNegOp, right);
                    default: return nullptr;
                };
            }
            
            return atomic();
        }

        Expression * atomic() {
            // std::cout << "atomic - " << current << std::endl;
            if (match(1, FALSE)) return new ConstExp(false);
            // std::cout << "atomic - " << current << std::endl;
            if (match(1, TRUE)) return new ConstExp(true);
            // std::cout << "atomic - " << current << std::endl;
            if (match(1, INTEGER)) {
                return new ConstExp(stoi(previous().get_string()));
            }
            // std::cout << "atomic - " << current << std::endl;
            if (match(1, STRING)) return new ConstExp(previous().get_string());
            // std::cout << "atomic - " << current << std::endl;
            if (match(1, IDENTIFIER)) {
                Expression* ident_exp = new VarExp(previous().get_string());
                
                if (check(LBRACKET)) {
                    std::vector<Expression*> idx_exps;

                    while (match(1, LBRACKET)) {
                        Expression * idx_exp = expression();
                        if (!match(1, RBRACKET)) {
                            throw std::runtime_error("Expected a ']'");
                        }
                        idx_exps.push_back(idx_exp);
                    }

                    Expression* curr = new ListAccessExpression(ident_exp, idx_exps[0]);
                    for (int i = 1; i < idx_exps.size(); i++) {
                        curr = curr = new ListAccessExpression(curr, idx_exps[i]);
                    }

                    return curr;
                } else {
                    return ident_exp;
                }
            }
            // std::cout << "atomic - " << current << std::endl;

            if (match(1, LBRACKET)) {
                std::vector<Expression*> elements;

                if (!check(RBRACKET)) {
                    do {
                        Expression * elem_exp = expression();
                        elements.push_back(elem_exp);
                    } while (match(1, COMMA));
                }

                if (!match(1, RBRACKET)) {
                    throw std::runtime_error("Expected a ']'");
                }

                return new ListExpression(elements);
            }

            if (match(1, LEFT_PAREN)) {
                Expression * inner_exp = expression();
                if (match(1, RIGHT_PAREN)) {
                    return inner_exp;
                }
            }

            return nullptr;
        }

        // Expression * if_expression() {
        //     // std::cout << "if-statment - " << current << std::endl;
        //     advance(); // left paren 
        //     Expression * conditional = expression();
        //     advance(); // right paren
        //     advance(); // left brace
        //     Expression * if_exp = expression();
        //     // std::cout << current << std::endl;
        //     advance(); // right brace
        //     advance(); // else 
        //     advance();
        //     advance(); // left brace
        //     // std::cout << current << std::endl;
        //     Expression * else_exp = expression();
        //     advance(); // right brace
        //     advance();
        //     return new IfExpression(conditional, if_exp, else_exp);
        // }

    private:
        size_t current;
        const std::vector<Token> tokens;

        bool match(int count, ...) {
            // std::cout << "match - " << current << std::endl;
            va_list args;
            va_start(args, count);

            for (int i = 0; i < count; i++) {
                int raw_type = va_arg(args, int);
                TokenType type = static_cast<TokenType>(raw_type);
                if (check(type)) {
                    va_end(args);
                    advance();
                    return true;
                }
            }

            va_end(args);
            return false;
        }
};
