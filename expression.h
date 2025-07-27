#pragma once

#include <string> 
#include <iostream>

enum class ExpressionType {
    CONST_EXP,
    VAR_EXP,
    IF_EXP,
    BIN_EXP,
    MON_EXP,
    LET_EXP,
    WHILE_EXP,
};

class Expression {
    protected:
        ExpressionType type;

    public:
        Expression(ExpressionType t) : type(t) {}
        virtual ~Expression() {}

        ExpressionType get_signature() const {
            return type;
        }
};

union Value {
            int int_val;
            bool bool_val;
            std::string* string_val;

            ~Value() {}
        };

enum class ConstType {
    IntConst,
    BoolConst,
    StringConst
};

class ConstExp : public Expression {
    private:
        ConstType const_type;
        ExpressionType exp_type;
        
        
    public:
        Value value;

        ConstExp(int c) : Expression(ExpressionType::CONST_EXP), const_type(ConstType::IntConst) {
            value.int_val = c;
        }
        ConstExp(bool c) : Expression(ExpressionType::CONST_EXP), const_type(ConstType::BoolConst) {
            value.bool_val = c;
        }
        ConstExp(const std::string& c) : Expression(ExpressionType::CONST_EXP), const_type(ConstType::StringConst) {
            value.string_val = new std::string(c);
        }

        ConstType get_type() const {
            return const_type;
        }

        Value get_val() {
            return value;
        }
};

class VarExp : public Expression {
    private:
        std::string var_name;
    public:
        VarExp(std::string v) : Expression(ExpressionType::VAR_EXP), var_name(v) {}
        std::string get_var_name() { return var_name; }
};

enum class MonadicOperator {
    NotOp,
    IntNegOp,
    PrintOp
};

class MonadicExpression : public Expression {
    private:
        Expression* exp;
        MonadicOperator mon_op;
    public:
        MonadicExpression(MonadicOperator op, Expression * e) : Expression(ExpressionType::MON_EXP), exp(e), mon_op(op) {}
        MonadicOperator get_type() { return mon_op; }
        Expression * get_right() { return exp; }
};

enum class BinaryOperator {
    IntPlusOp,
    IntMinusOp,
    IntDivOp,
    IntTimesOp,
    ModOp,  
    EqualityOp,
    GtOp,
    GteOp,
    LtOp,
    LteOp,
    EqualsOp,
    NotEqualsOp,
    AndOp,
    OrOp
};

class BinaryExpression : public Expression {
    private:
        Expression * e1;
        Expression * e2;
        BinaryOperator bin_op;
    public:
        BinaryExpression(BinaryOperator op, Expression * exp1, Expression * exp2) : Expression(ExpressionType::BIN_EXP), bin_op(op), e1(exp1), e2(exp2) {} 
        BinaryOperator get_type() { return bin_op; }
        Expression * get_left() { return e1; }
        Expression * get_right() { return e2; }

};

class AssignmentExpression : public Expression {
    private:
        std::string ident;
        Expression * exp;
        bool reassignment;
    public:
        AssignmentExpression(std::string identifier, Expression * e, bool reassignment) 
            : Expression(ExpressionType::LET_EXP), ident(identifier), exp(e), reassignment(reassignment) {}
        std::string get_id() { return ident; }
        Expression * get_right() { return exp; }
        bool is_reassign() { return reassignment; }
};

class IfExpression : public Expression {
    private:
        Expression * conditional;
        std::vector<Expression *> if_expressions;
        std::vector<Expression *> else_expressions;
    public: 
        IfExpression(Expression * e1, std::vector<Expression *> e2, std::vector<Expression *> e3): 
            Expression(ExpressionType::IF_EXP), conditional(e1), if_expressions(e2), else_expressions(e3) {}
        Expression * get_conditional() {return conditional; }
        std::vector<Expression *> get_if_exps() {return if_expressions; }
        std::vector<Expression *> get_else_exps() {return else_expressions; }

};

class WhileExpression : public Expression {
    private:
        Expression * conditional;
        std::vector<Expression *> body_expressions;
    public: 
        WhileExpression(Expression * e1, std::vector<Expression *> e2): 
            Expression(ExpressionType::WHILE_EXP), conditional(e1), body_expressions(e2) {}
        Expression * get_conditional() {return conditional; }
        std::vector<Expression *> get_body_exps() {return body_expressions; }

};
