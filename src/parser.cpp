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

    private:
        const std::vector<Token>& _tokens;

        bool match(int idx, TokenType type) {
            return _tokens[idx].get_type() == type;
        }
};
