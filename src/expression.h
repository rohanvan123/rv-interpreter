#pragma once

#include <string> 
#include <iostream>

class Value;

using ValueData = std::variant<int, bool, std::string, std::vector<Value>>;

class Value {
    public:
        ValueData data;
        Value() = default;
        Value(ValueData vd): data(vd) {}
};

enum class ExpressionType {
    CONST_EXP,
    VAR_EXP,
    IF_EXP,
    BIN_EXP,
    MON_EXP,
    LET_EXP,
    WHILE_EXP,
    LIST_EXP,
    LIST_ACCESS_EXP,
    LIST_MODIFY_EXP
};
class Expression {
    protected:
        ExpressionType type;

    public:
        Expression(ExpressionType t) : type(t) {}
        virtual ~Expression() {}
        virtual Expression* clone() const = 0;

        ExpressionType get_signature() const {
            return type;
        }
};

std::vector<Expression*> clone_exp_vector(std::vector<Expression*> vecs) {
    std::vector<Expression*> res;
    for (Expression* e : vecs) {
        res.push_back(e->clone());
    }
    return res;
}

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
            value.data = c;
        }
        ConstExp(bool c) : Expression(ExpressionType::CONST_EXP), const_type(ConstType::BoolConst) {
            value.data = c;
        }
        ConstExp(std::string c) : Expression(ExpressionType::CONST_EXP), const_type(ConstType::StringConst) {
            value.data = c;
        }

        ConstType get_type() const {
            return const_type;
        }

        Expression* clone() const override {
            switch (const_type) {
                case ConstType::IntConst: return new ConstExp(std::get<int>(value.data));
                case ConstType::StringConst: return new ConstExp(std::get<string>(value.data));
                case ConstType::BoolConst: return new ConstExp(std::get<bool>(value.data));
            }
            throw std::runtime_error("Udentified type for const clone");
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
        Expression* clone() const override {
            return new VarExp(var_name);
        }
        std::string get_var_name() { return var_name; }
};

enum class MonadicOperator {
    NotOp,
    IntNegOp,
    PrintOp,
    SizeOp    
};

class MonadicExpression : public Expression {
    private:
        Expression* exp;
        MonadicOperator mon_op;
    public:
        MonadicExpression(MonadicOperator op, Expression * e) : Expression(ExpressionType::MON_EXP), exp(e), mon_op(op) {}
        MonadicOperator get_type() { return mon_op; }
        Expression * get_right() { return exp; }
        Expression* clone() const override {
            return new MonadicExpression(mon_op, exp->clone());
        }
        ~MonadicExpression() {
            delete exp;
            exp = nullptr;
        }
        
};

enum class BinaryOperator {
    IntPlusOp,
    IntMinusOp,
    IntDivOp,
    IntTimesOp,
    IntPowOp,
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
        Expression* clone() const override {
            return new BinaryExpression(bin_op, e1->clone(), e2->clone());
        }
        ~BinaryExpression() {
            delete e1;
            e1 = nullptr;

            delete e2;
            e2 = nullptr;
        }
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
        Expression* clone() const override {
            return new AssignmentExpression(ident, exp->clone(), reassignment);
        }
        ~AssignmentExpression() {
            if (exp != nullptr) {
                delete exp;
                exp = nullptr;
            }
        }
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
        Expression* clone() const override {
            return new IfExpression(conditional->clone(), clone_exp_vector(if_expressions), clone_exp_vector(else_expressions));
        }
        ~IfExpression() {
            delete conditional;
            conditional = nullptr;

            for (Expression* expr : if_expressions) {
                delete expr;
            }
            if_expressions.clear();

            for (Expression* expr : else_expressions) {
                delete expr;
            }
            else_expressions.clear();
        }

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
        Expression* clone() const override {
            return new WhileExpression(conditional->clone(), clone_exp_vector(body_expressions));
        }
        ~WhileExpression() {
            delete conditional;
            conditional = nullptr;

            for (Expression* expr : body_expressions) {
                delete expr;
            }
            body_expressions.clear();
        }

};

class ListExpression : public Expression {
    private:
        std::vector<Expression*> elements;

    public:
        ListExpression(std::vector<Expression*> exps):
            Expression(ExpressionType::LIST_EXP), elements(exps) {}
        std::vector<Expression*> get_elements() { return elements; }
        Expression * access_element(int idx) { return elements.at(idx); }
        Expression* clone() const override {
            return new ListExpression(clone_exp_vector(elements));
        }
        ~ListExpression() {
            for (Expression* expr : elements) {
                delete expr;
            }
            elements.clear();
        }
};

class ListAccessExpression : public Expression {
    private:
        Expression * ident_exp;
        Expression * idx_exp;

    public:
        ListAccessExpression(Expression * exp1, Expression * exp2) : Expression(ExpressionType::LIST_ACCESS_EXP), ident_exp(exp1), idx_exp(exp2) {
        } 
        Expression * get_arr_exp() { return ident_exp; }
        Expression * get_idx_exp() { return idx_exp; }
        Expression* clone() const override {
            return new ListAccessExpression(ident_exp->clone(), idx_exp->clone());
        }
        ~ListAccessExpression() {
            if (ident_exp != nullptr) {
                delete ident_exp;
                ident_exp = nullptr;
            }
            
            if (idx_exp != nullptr) {
                delete idx_exp;
                idx_exp = nullptr;
            }
        }
};

class ListModifyExpression : public Expression {
    private:
        Expression * ident_exp;
        Expression * idx_exp;
        Expression * exp;

    public:
        ListModifyExpression(Expression * exp1, Expression * exp2, Expression * exp3) 
            : Expression(ExpressionType::LIST_MODIFY_EXP), ident_exp(exp1), idx_exp(exp2), exp(exp3) {} 
        Expression * get_ident_exp() { return ident_exp; }
        Expression * get_idx_exp() { return idx_exp; }
        Expression * get_exp() { return exp; }
        Expression* clone() const override {
            return new ListModifyExpression(ident_exp->clone(), idx_exp->clone(), exp->clone());
        }
        ~ListModifyExpression() {
            if (ident_exp != nullptr) {
                delete ident_exp;
                ident_exp = nullptr;
            }

            if (idx_exp != nullptr) {
                delete idx_exp;
                idx_exp = nullptr;
            }

            if (exp != nullptr) {
                delete exp;
                exp = nullptr;
            }
        }
};