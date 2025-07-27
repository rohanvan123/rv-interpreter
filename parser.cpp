// #include "lexer.cpp"
#include "expression.h"
#include "utils.cpp"
#include <stack>

using enum TokenType;

TokenBlock parse_block(std::vector<Token>& tokens, size_t &index) {
    TokenBlock block;

    while (index < tokens.size()) {
        Token& tok = tokens[index];

        if (tok.get_type() == LBRACE) {
            // Start of nested block
            ++index;  // consume '{'
            TokenBlock nested = parse_block(tokens, index);
            block.contents.push_back(nested);
        }
        else if (tok.get_type() == RBRACE) {
            ++index;  // consume '}'
            return block;  // end current block
        }
        else {
            block.contents.push_back(tok);
            ++index;
        }
    }

    return block;
}


void print_blocks(const TokenBlock& block, int indent) {
    std::string indent_str = "";
    for (int i = 0; i < indent; i++) {
        indent_str += " ";
    }

    for (int i = 0; i < block.contents.size(); i++) {
        TokenOrBlock item = block.contents[i];
        if (std::holds_alternative<Token>(item)) {
            const Token& tok = std::get<Token>(item);
            std::cout << indent_str << token_to_string(tok) << std::endl;
        } else {
            const TokenBlock& sub_block = std::get<TokenBlock>(item);
            print_blocks(sub_block, indent + 2);
        }
    }
}


class Parser {
    public:
        Parser(const std::vector<Token>& tokens): _tokens(tokens) {
            size_t idx = 0;
            // _root_block = parse_block(tokens, idx);
            // _current_block = _root_block;
        }

        // TokenBlock& get_root() {
        //     return root_block;
        // }

        std::vector<Token> tokens_from_idx(int &idx) {
            // std::cout << "tokens_from_idx" << std::endl;
            std::vector<Token> tokens;
            while (!match(idx, SEMI) && !match(idx, LBRACE)) {
                tokens.push_back(_tokens[idx]);
                idx += 1;
            }

            return tokens;
        }

        std::vector<Token> parse_condition(int &idx) {
            std::vector<Token> tokens;
            std::stack<std::string> stack;
            stack.push("(");

            idx += 1; // left init parenthesis
            while (!stack.empty()) {
                if (match(idx, LEFT_PAREN)) {
                    stack.push("(");
                } else if (match(idx, RIGHT_PAREN)) {
                    stack.pop();
                    if (stack.empty()) {
                        // final closing parenthesis reached
                        continue;
                    }
                }
                tokens.push_back(_tokens[idx]);
                idx += 1;
            } 

            return tokens;
        }

        std::vector<Expression*> parse_top_level_expressions() {
            std::vector<Expression*> top_level_expressions;
            int idx = 0;
            while (idx < _tokens.size() && !match(idx, END_OF_FILE)) {
                Expression* exp = parse_expression(idx, false);
                top_level_expressions.push_back(exp);
            }

            return top_level_expressions;
        }

        Expression* parse_expression(int &idx, bool parsing_condition) {
            // std::cout << "parse_expression" << " " << idx << std::endl;
            if (match(idx, LET)) {
                return parse_let_expression(idx);
            } else if (match(idx, IDENTIFIER) && match(idx + 1, EQUALS)) {
                return parse_reassign_expression(idx);
            } else if (match(idx, IF)) {
                return parse_if_expression(idx);
            } else if (match(idx, WHILE)) {
                return parse_while_expression(idx);
            } else {
                // for simple expressions, delegate directly to old parser
                std::vector<Token> tokens;
                tokens = tokens_from_idx(idx);
                // std::cout << "after tokens " << idx << std::endl; 
                // print_tokens(tokens);

                if (parsing_condition) {
                    tokens.erase(tokens.begin());
                    tokens.pop_back();
                }
                ArithmeticParser expr_parser(tokens);
                Expression *exp = expr_parser.parse();
                idx += 1; // SEMI or LBRACE
                return exp;
            }
        }

        AssignmentExpression* parse_let_expression(int &idx) {
            // std::cout << "parse_let_expression" << std::endl;
            idx += 1; // LET
            const std::string ident_name = _tokens[idx].get_string(); // IDENT name
            idx += 1;
            idx += 1; // EQUALS
            Expression * inner_exp = parse_expression(idx, false);

            return new AssignmentExpression(ident_name, inner_exp, false);
        }

        AssignmentExpression* parse_reassign_expression(int &idx) {
            const std::string ident_name = _tokens[idx].get_string(); // IDENT name
            idx += 1;
            idx += 1; // EQUALS
            Expression * inner_exp = parse_expression(idx, false);

            return new AssignmentExpression(ident_name, inner_exp, true);
        }

        WhileExpression* parse_while_expression(int &idx) {
            // std::cout << "parse_while_expression" << std::endl;
            idx += 1; // WHILE
            Expression * cond = parse_expression(idx, true);
            std::vector<Expression *> body_expressions;
            while (!match(idx, RBRACE)) {
                body_expressions.push_back(parse_expression(idx, false));
            }

            idx += 1; // RBRACE

            WhileExpression * exp = new WhileExpression(cond, body_expressions);
            return exp;
        }
        
        IfExpression* parse_if_expression(int &idx) {
            // std::cout << "parse_if_expression " << idx << std::endl;
            idx += 1; // IF
            // need to perform valid parenthesis to get string
            Expression * cond = parse_expression(idx, true);
            

            std::vector<Expression *> if_expressions;
            while (!match(idx, RBRACE)) {
                if_expressions.push_back(parse_expression(idx, false));
            }

            idx += 1; // RBRACE
            idx += 1; // ELSE
            idx += 1; // LBRACE

            std::vector<Expression *> else_expressions;

            while (!match(idx, RBRACE)) {
                else_expressions.push_back(parse_expression(idx, false));
            }

            idx += 1; // RBRACE

            return new IfExpression(cond, if_expressions, else_expressions);
        }



        // Expression * parse_while_expression() {
        //     return &Expression()
        // }

        

        // Token peek() {
        //     return tokens.at(current);
        // }

        // bool is_at_end() {
        //     return current == tokens.size();
        // }

        // bool check(TokenType type) {
        //     if (is_at_end()) return false;
        //     // std::cout << peek().get_type() << " " << type << std::endl;
        //     // std::cout << "checking ";
        //     return peek().get_type() == type;
        // }

        // Token previous() {
        //     return tokens.at(current - 1);
        // }

        // Token advance() {
        //     if (!is_at_end()) current++;
        //     return previous();
        // }

        // Expression * parse() {
        //     return expression();
        // }

        // Expression *  expression() {
        //     if (match(1, LET)) {
        //         // std::cout << "dec" << std::endl;
        //         Token var = advance();
        //         advance();
        //         Expression * right = comparison();
        //         return new AssignmentExpression(var.get_string(), right);
        //     } else if (match(1, IF)) {
        //         Expression * if_exp = if_expression();
        //         return if_exp;
        //     } else if (match(1, WHILE)) {
        //         Expression * while_exp = while_expression();
        //         return while_exp;
        //     }
        //     return comparison();
        // }

        // Expression * comparison() {
        //     // std::cout << "comparison - " << current << std::endl;
        //     Expression * left = term();
        //     while (match(4, GEQ, LEQ, GT, LT)) {
        //         Token op = previous();
        //         Expression * right = term();

        //         switch (op.get_type()) {
        //             case GT: left = new BinaryExpression(BinaryOperator::GtOp, left, right); break;
        //             case GEQ: left =  new BinaryExpression(BinaryOperator::GteOp, left, right); break;
        //             case LT: left =  new BinaryExpression(BinaryOperator::LtOp, left, right); break;
        //             case LEQ: left = new BinaryExpression(BinaryOperator::GteOp, left, right); break;
        //             case NEQ: left = new BinaryExpression(BinaryOperator::NotEqualsOp, left, right); break;
        //             case EQUALITY: left = new BinaryExpression(BinaryOperator::EqualityOp, left, right); break;
        //         };
                
        //     }

        //     return left;
        // };

        // Expression * term() {
        //     // std::cout << "term - " << current << std::endl;
        //     Expression * left = factor();
        //     while (match(2, PLUS, MINUS)) {
        //         Token op = previous();
        //         Expression * right = factor();

        //         switch (op.get_type()) {
        //             case PLUS: left = new BinaryExpression(BinaryOperator::IntPlusOp, left, right); break;
        //             case MINUS: left =  new BinaryExpression(BinaryOperator::IntMinusOp, left, right); break;
        //         };
                
        //     }

        //     return left;
        // }

        // Expression * factor() {
        //     // std::cout << "factor - " << current << std::endl;
        //     Expression * left = unary();
        //     while (match(3, TIMES, DIVIDES, MOD)) {
        //         Token op = previous();
        //         Expression * right = unary();

        //         switch (op.get_type()) {
        //             case TIMES: left = new BinaryExpression(BinaryOperator::IntTimesOp, left, right); break;
        //             case DIVIDES: left =  new BinaryExpression(BinaryOperator::IntDivOp, left, right); break;
        //             case MOD: left = new BinaryExpression(BinaryOperator::ModOp, left, right); break;
        //         };
                
        //     }

        //     return left;
        // }

        // Expression * unary() {
        //     // std::cout << "unary - " << current << std::endl;
        //     if (match(2, PRINT, MINUS)) {
        //         Token op = previous();
        //         Expression * right = unary();

        //         switch (op.get_type()) {
        //             case PRINT: return new MonadicExpression(MonadicOperator::PrintOp, right);
        //             case MINUS: return new MonadicExpression(MonadicOperator::IntNegOp, right);
        //         };
        //     }
            
        //     return atomic();
        // }

        // Expression * atomic() {
        //     // std::cout << "atomic - " << current << std::endl;
        //     if (match(1, FALSE)) return new ConstExp(false);
        //     // std::cout << "atomic - " << current << std::endl;
        //     if (match(1, TRUE)) return new ConstExp(true);
        //     // std::cout << "atomic - " << current << std::endl;
        //     if (match(1, INTEGER)) {
        //         return new ConstExp(stoi(previous().get_string()));
        //     }
        //     // std::cout << "atomic - " << current << std::endl;
        //     if (match(1, STRING)) return new ConstExp(previous().get_string());
        //     // std::cout << "atomic - " << current << std::endl;
        //     if (match(1, IDENTIFIER)) return new VarExp(previous().get_string());
        //     // std::cout << "atomic - " << current << std::endl;

        //     if (match(1, LEFT_PAREN)) {
        //         Expression * inner_exp = expression();
        //         if (match(1, RIGHT_PAREN)) {
        //             return inner_exp;
        //         }
        //     }
        // }

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

        // Expression * while_expression() {
        //     advance(); // left paren 
        //     Expression * conditional = expression();
        //     advance(); // right paren
        //     advance(); // left brace
        // }

    private:
        const std::vector<Token>& _tokens;
        // TokenBlock _root_block;
        // TokenBlock & _current_block;

        bool match(int idx, TokenType type) {
            return _tokens[idx].get_type() == type;
        }
};
