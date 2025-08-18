#include "arithmetic_parser.hpp"

ArithmeticParser::ArithmeticParser(const std::vector<Token>& tok): tokens(tok) {}

Token ArithmeticParser::peek() {
    return tokens.at(current);
}

bool ArithmeticParser::is_at_end() {
    return current == tokens.size();
}

bool ArithmeticParser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().get_type() == type;
}

Token ArithmeticParser::previous() {
    return tokens.at(current - 1);
}

Token ArithmeticParser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

bool ArithmeticParser::match(int count, ...) {
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

Expression * ArithmeticParser::parse() {
    return expression();
}

Expression * ArithmeticParser::expression() {
    return disjunction();
}

Expression * ArithmeticParser::disjunction() {
    Expression * left = conjunction();
    while (match(1, OR)) {
        Expression * right = conjunction();
        left = new BinaryExpression(BinaryOperator::OrOp, left, right);
    }

    return left;
}

Expression * ArithmeticParser::conjunction() {
    Expression * left = comparison();
    while (match(1, AND)) {
        Expression * right = comparison();
        left = new BinaryExpression(BinaryOperator::AndOp, left, right);
    }

    return left;
}

Expression * ArithmeticParser::comparison() {
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

Expression * ArithmeticParser::term() {
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

Expression * ArithmeticParser::factor() {
    // std::cout << "factor - " << current << std::endl;
    Expression * left = exponent();
    while (match(3, TIMES, DIVIDES, MOD)) {
        Token op = previous();
        Expression * right = exponent();

        switch (op.get_type()) {
            case TIMES: left = new BinaryExpression(BinaryOperator::IntTimesOp, left, right); break;
            case DIVIDES: left =  new BinaryExpression(BinaryOperator::IntDivOp, left, right); break;
            case MOD: left = new BinaryExpression(BinaryOperator::ModOp, left, right); break;
            default: return nullptr;
        };
        
    }

    return left;
}

Expression * ArithmeticParser::exponent() {
    Expression * left = unary();
    while (match(1, POW)) {
        Token op = previous();
        Expression * right = unary();

        switch (op.get_type()) {
            case POW: left = new BinaryExpression(BinaryOperator::IntPowOp, left, right); break;
            default: return nullptr;
        };
        
    }

    return left;
}

Expression * ArithmeticParser::unary() {
    // std::cout << "unary - " << current << std::endl;
    if (match(3, PRINT, MINUS, SIZE)) {
        Token op = previous();
        Expression * right = unary();

        switch (op.get_type()) {
            case PRINT: return new MonadicExpression(MonadicOperator::PrintOp, right);
            case MINUS: return new MonadicExpression(MonadicOperator::IntNegOp, right);
            case SIZE: return new MonadicExpression(MonadicOperator::SizeOp, right);
            default: return nullptr;
        };
    }
    
    return atomic();
}

Expression * ArithmeticParser::atomic() {
    // std::cout << "atomic - " << current << std::endl;
    if (match(1, FALSE)) return new ConstExp(false);
    if (match(1, TRUE)) return new ConstExp(true);
    if (match(1, INTEGER)) return new ConstExp(stoi(previous().get_string()));
    if (match(1, STRING)) return new ConstExp(previous().get_string());

    if (match(1, IDENTIFIER)) {
        std::string ident_name = previous().get_string();
        if (match(1, LEFT_PAREN)) {
            std::vector<Expression*> arg_exps;
            // Function Call

            if (!match(1, RIGHT_PAREN)) {
                do {
                    Expression * arg_exp = expression();
                    arg_exps.push_back(arg_exp);
                } while(match(1, COMMA));

                if (!match(1, RIGHT_PAREN)) {
                    throw std::runtime_error("Expected a ')' for function call");
                }
            }

            return new FunctionCallExpression(ident_name, arg_exps);
        }

        Expression* ident_exp = new VarExp(ident_name);
        
        if (check(LBRACKET)) {
            // LIST ACCESS
            std::vector<Expression*> idx_exps;

            while (match(1, LBRACKET)) {
                Expression * idx_exp = expression();
                if (!match(1, RBRACKET)) {
                    throw std::runtime_error("Expected a ']'");
                }
                idx_exps.push_back(idx_exp);
            }

            Expression* curr = new ListAccessExpression(ident_exp, idx_exps[0]);
            for (int i = 1; static_cast<size_t>(i) < idx_exps.size(); i++) {
                curr = new ListAccessExpression(curr, idx_exps[i]);
            }

            return curr;
        } else {
            // VARIABLE
            return ident_exp;
        }
    }

    if (match(1, LBRACKET)) {
        // LISTING of objects
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

    // Ensures parenthesis are given highest priority
    if (match(1, LEFT_PAREN)) {
        Expression * inner_exp = expression();
        if (match(1, RIGHT_PAREN)) {
            return inner_exp;
        }
    }

    return nullptr;
}