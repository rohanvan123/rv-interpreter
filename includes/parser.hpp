#ifndef PARSER_HPP
#define PARSER_HPP

#include "expression.hpp"
#include "utils.hpp"

#include <set>
#include <map>

class Parser {
    private:
        static const std::set<TokenType> assignment_op_tokens;
        static const std::map<TokenType, BinaryOperator> token_to_binop;

        bool match(int idx, TokenType type) {
            return _tokens[idx].get_type() == type;
        }

        bool match(int idx, std::set<TokenType> types) {
            return types.find(_tokens[idx].get_type()) != types.end();
        }

        std::vector<Token> tokens_from_idx(int &idx);
        std::vector<Token> arr_idx_tokens(int &idx);
        std::vector<Token> parse_condition(int &idx);

        Expression* parse_expression(int &idx, bool parsing_condition);

        AssignmentExpression* parse_let_expression(int &idx);
        AssignmentExpression* parse_reassign_expression(int &idx);
        AssignmentExpression* parse_assign_op_expression(int &idx);
        AssignmentExpression* parse_list_assign_expression(int &idx);
        
        WhileExpression* parse_while_expression(int &idx);
        IfExpression* parse_if_expression(int &idx);
        FunctionAssignmentExpression* parse_function_expression(int &idx);

        const std::vector<Token>& _tokens;

    public:
        Parser(const std::vector<Token>& tokens): _tokens(tokens) {}
        std::vector<Expression*> parse_top_level_expressions();
    
};

#endif // PARSER_HPP