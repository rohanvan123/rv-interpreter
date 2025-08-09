// #include "lexer.cpp"
#include "expression.h"
#include "utils.cpp"
#include <stack>
#include <set>

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

std::set<TokenType> assignment_op_tokens = {PLUS_EQUALS, TIMES_EQUALS, MINUS_EQUALS, DIVIDES_EQUALS, MOD_EQUALS};
std::map<TokenType, BinaryOperator> token_to_binop = {
    {PLUS_EQUALS, BinaryOperator::IntPlusOp},
    {MINUS_EQUALS, BinaryOperator::IntMinusOp},
    {TIMES_EQUALS, BinaryOperator::IntTimesOp},
    {DIVIDES_EQUALS, BinaryOperator::IntDivOp},
    {MOD_EQUALS, BinaryOperator::ModOp},
};

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

        std::vector<Token> arr_idx_tokens(int &idx) {
            std::vector<Token> tokens;
            idx += 1; // LBRACKET
            while (!match(idx, RBRACKET)) {
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
            } else if (match(idx, IDENTIFIER) && match(idx + 1, LBRACKET)) {
                return parse_list_assign_expression(idx);
            } else if (match(idx, IDENTIFIER) && match(idx + 1, assignment_op_tokens)) {
                return parse_assign_op_expression(idx);
            } else if (match(idx, IF)) {
                return parse_if_expression(idx);
            } else if (match(idx, WHILE)) {
                return parse_while_expression(idx);
            } else if (match(idx, FUNCTION)) {
                return parse_function_expression(idx);
            } else {
                // for simple expressions, delegate directly to old parser
                bool returnable = false;
                if (match(idx, RETURN)) {
                    returnable = true;
                    idx += 1;
                }

                std::vector<Token> tokens;
                tokens = tokens_from_idx(idx);

                if (returnable && tokens.size() == 0) {
                    Expression *exp = new EmptyExpression();
                    exp->set_returnable(true);
                    return exp;
                }

                if (parsing_condition) {
                    tokens.erase(tokens.begin());
                    tokens.pop_back();
                }

                ArithmeticParser expr_parser(tokens);
                Expression *exp = expr_parser.parse();
                exp->set_returnable(returnable);

                idx += 1; // SEMI or LBRACE
                return exp;
            }
        }

        AssignmentExpression* parse_let_expression(int &idx) {
            idx += 1; // LET
            const std::string ident_name = _tokens[idx].get_string(); // IDENT name
            idx += 1;
            idx += 1; // EQUALS
            
            std::vector<Token> tokens;
            tokens = tokens_from_idx(idx);
            ArithmeticParser expr_parser(tokens);
            Expression * inner_exp = expr_parser.parse();

            idx += 1;

            return new AssignmentExpression(ident_name, inner_exp, false);
        }

        AssignmentExpression* parse_reassign_expression(int &idx) {
            const std::string ident_name = _tokens[idx].get_string(); // IDENT name
            idx += 1;
            idx += 1; // EQUALS

            std::vector<Token> tokens;
            tokens = tokens_from_idx(idx);
            ArithmeticParser expr_parser(tokens);
            Expression * inner_exp = expr_parser.parse();

            idx += 1;

            return new AssignmentExpression(ident_name, inner_exp, true);
        }

        AssignmentExpression* parse_assign_op_expression(int &idx) {
            const std::string ident_name = _tokens[idx].get_string(); // IDENT name
            idx += 1;
            TokenType op_token = _tokens[idx].get_type();
            idx += 1; // OP_EQUALS

            std::vector<Token> tokens;
            tokens = tokens_from_idx(idx);
            ArithmeticParser expr_parser(tokens);
            Expression* inner_exp = expr_parser.parse();

            idx += 1; // SEMI

            // wrap in bin exp based on operator
            Expression* var_exp = new VarExp(ident_name);
            Expression* final_exp = new BinaryExpression(token_to_binop[op_token], var_exp, inner_exp);

            return new AssignmentExpression(ident_name, final_exp, true);
        }


        AssignmentExpression* parse_list_assign_expression(int &idx) {
            const std::string ident_name = _tokens[idx].get_string(); // IDENT name
            std::vector<Expression*> idx_exps;
            idx += 1;

            while (match(idx, LBRACKET)) {
                std::vector<Token> idx_tokens = arr_idx_tokens(idx);
                ArithmeticParser expr_parser(idx_tokens);
                Expression * exp = expr_parser.parse();
                idx_exps.push_back(exp);

                if (!match(idx, RBRACKET)) throw std::runtime_error("Expected a ']'");
                idx += 1; // RBRACKET
            }

            idx += 1; // EQUALS
            std::vector<Token> idx_tokens = tokens_from_idx(idx);
            ArithmeticParser expr_parser(idx_tokens);
            Expression * inner_exp = expr_parser.parse();

            idx += 1; // SEMI

            int n = idx_exps.size();
            Expression * curr = new VarExp(ident_name);
            std::vector<Expression*> list_accesses;

            list_accesses.push_back(curr);
            for (int i = 0; i < n - 1; i++) {
                curr = new ListAccessExpression(curr->clone(), idx_exps[i]);
                list_accesses.push_back(curr);
            }

            // arr[0][0] -> [arr, arr[0]]
            curr = new ListModifyExpression(list_accesses[n - 1], idx_exps[n - 1], inner_exp);
            
            for (int i = n - 2; i >= 0; i--) {
                Expression * prev = curr;
                curr = new ListModifyExpression(list_accesses[i], idx_exps[i]->clone(), curr->clone());
                delete prev;
            }

            return new AssignmentExpression(ident_name, curr, true);
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

            Expression * cond = parse_expression(idx, true);
            

            std::vector<Expression *> if_expressions;
            while (!match(idx, RBRACE)) {
                if_expressions.push_back(parse_expression(idx, false));
            }

            idx += 1; // RBRACE
            std::vector<Expression *> else_expressions;
            if (!match(idx, ELSE)) {
                return new IfExpression(cond, if_expressions, else_expressions);
                idx += 1;
            } else {
                idx += 1; // ELSE
            }
            
            idx += 1; // LBRACE

            while (!match(idx, RBRACE)) {
                else_expressions.push_back(parse_expression(idx, false));
            }

            idx += 1; // RBRACE

            return new IfExpression(cond, if_expressions, else_expressions);
        }

        FunctionAssignmentExpression* parse_function_expression(int &idx) {
            idx += 1; // Function
            const std::string func_name = _tokens[idx].get_string();
            idx += 1; // name
            idx += 1; // LEFT_PAREN

            std::vector<std::string> args;

            while (!match(idx, LBRACE)) {
                if (match(idx, IDENTIFIER)) {
                    Token tok = _tokens[idx];
                    args.push_back(tok.get_string());
                }
                idx += 1;
            }

            idx += 1; // RBRACE

            std::vector<Expression*> body_expressions;
            while (!match(idx, RBRACE)) {
                body_expressions.push_back(parse_expression(idx, false));
            }

            idx += 1; // RBRACE

            return new FunctionAssignmentExpression(func_name, args, body_expressions);
        }

    private:
        const std::vector<Token>& _tokens;

        bool match(int idx, TokenType type) {
            return _tokens[idx].get_type() == type;
        }

        bool match(int idx, std::set<TokenType> types) {
            return types.find(_tokens[idx].get_type()) != types.end();
        }
};
