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
    public:
        AssignmentExpression(std::string identifier, Expression * e) : Expression(ExpressionType::LET_EXP), ident(identifier), exp(e) {}
        std::string get_id() { return ident; }
        Expression * get_right() { return exp; }
};
