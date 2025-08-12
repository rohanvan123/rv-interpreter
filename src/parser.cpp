#include "parser.hpp"
#include "token.hpp"
#include "expression.hpp"
#include "arithmetic_parser.hpp"

#include <stack>

using enum TokenType;

const std::set<TokenType> Parser::assignment_op_tokens = {PLUS_EQUALS, TIMES_EQUALS, MINUS_EQUALS, DIVIDES_EQUALS, MOD_EQUALS};

const std::map<TokenType, BinaryOperator> Parser::token_to_binop = {
    {PLUS_EQUALS, BinaryOperator::IntPlusOp},
    {MINUS_EQUALS, BinaryOperator::IntMinusOp},
    {TIMES_EQUALS, BinaryOperator::IntTimesOp},
    {DIVIDES_EQUALS, BinaryOperator::IntDivOp},
    {MOD_EQUALS, BinaryOperator::ModOp},
};

std::vector<Token> Parser::tokens_from_idx(int &idx) {
    // std::cout << "tokens_from_idx" << std::endl;
    std::vector<Token> tokens;
    while (!match(idx, SEMI) && !match(idx, LBRACE)) {
        tokens.push_back(_tokens[idx]);
        idx += 1;
    }

    return tokens;
}

std::vector<Token> Parser::arr_idx_tokens(int &idx) {
    std::vector<Token> tokens;
    idx += 1; // LBRACKET
    while (!match(idx, RBRACKET)) {
        tokens.push_back(_tokens[idx]);
        idx += 1;
    }
    return tokens;
}

std::vector<Token> Parser::parse_condition(int &idx) {
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

std::vector<Expression*> Parser::parse_top_level_expressions() {
    std::vector<Expression*> top_level_expressions;
    int idx = 0;
    while (idx < _tokens.size() && !match(idx, END_OF_FILE)) {
        Expression* exp = parse_expression(idx, false);
        top_level_expressions.push_back(exp);
    }

    return top_level_expressions;
}

Expression* Parser::parse_expression(int &idx, bool parsing_condition) {
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
        Expression *exp;

        if (returnable && tokens.size() == 0) {
            exp = new EmptyExpression();
            exp->set_returnable(true);
        } else {
            if (parsing_condition) {
                tokens.erase(tokens.begin());
                tokens.pop_back();
            }

            ArithmeticParser expr_parser(tokens);
            exp = expr_parser.parse();
            exp->set_returnable(returnable);
        }

        idx += 1; // SEMI or LBRACE
        return exp;
    }
}

AssignmentExpression* Parser::parse_let_expression(int &idx) {
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

AssignmentExpression* Parser::parse_reassign_expression(int &idx) {
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

AssignmentExpression* Parser::parse_assign_op_expression(int &idx) {
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
    Expression* final_exp = new BinaryExpression(token_to_binop.at(op_token), var_exp, inner_exp);

    return new AssignmentExpression(ident_name, final_exp, true);
}


AssignmentExpression* Parser::parse_list_assign_expression(int &idx) {
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

WhileExpression* Parser::parse_while_expression(int &idx) {
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

IfExpression* Parser::parse_if_expression(int &idx) {
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

FunctionAssignmentExpression* Parser::parse_function_expression(int &idx) {
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

    idx += 1; // LBRACE

    std::vector<Expression*> body_expressions;
    while (!match(idx, RBRACE)) {
        body_expressions.push_back(parse_expression(idx, false));
    }

    idx += 1; // RBRACE
    return new FunctionAssignmentExpression(func_name, args, body_expressions);
}
