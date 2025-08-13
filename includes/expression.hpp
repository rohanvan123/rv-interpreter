#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string> 
#include <variant>
#include <iostream>
#include <vector>

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
    LIST_MODIFY_EXP,
    FUNC_ASSIGN_EXP,
    FUNC_CALL_EXP,
    EMPTY_EXP,
};

template<typename T>
std::vector<T> clone_vector(const std::vector<T>& vecs) {
    std::vector<T> res;
    res.reserve(vecs.size());  // optional, improves performance
    for (T e : vecs) {
        res.push_back(e->clone());
    }
    return res;
}

class Expression {
    public:
        Expression(const ExpressionType& t) : type(t) {}
        virtual ~Expression() {}
        
        virtual Expression* clone() const = 0;

        ExpressionType get_signature() const { return type; }

        bool is_returnable() const { return returnable; }
        void set_returnable(bool b) { returnable = b; }
    
    protected:
        ExpressionType type;
        bool returnable = false;
};

// Empty Expressions for empty return statemnts
class EmptyExpression : public Expression {
    public:
        EmptyExpression(): Expression(ExpressionType::EMPTY_EXP) {}
        Expression *clone() const override { return new EmptyExpression(); }
};

enum class ConstType {
    IntConst,
    BoolConst,
    StringConst
};

class ConstExp : public Expression {
    private:
        ConstType const_type;
        
    public:
        Value value;

        ConstExp(const int& c): Expression(ExpressionType::CONST_EXP), const_type(ConstType::IntConst) {
            value.data = c;
        }
        ConstExp(const bool& c): Expression(ExpressionType::CONST_EXP), const_type(ConstType::BoolConst) {
            value.data = c;
        }
        ConstExp(const std::string& c): Expression(ExpressionType::CONST_EXP), const_type(ConstType::StringConst) {
            value.data = c;
        }

        ConstType get_type() const { return const_type; }
        Value get_val() const { return value; }

        Expression* clone() const override {
            switch (const_type) {
                case ConstType::IntConst: return new ConstExp(std::get<int>(value.data));
                case ConstType::StringConst: return new ConstExp(std::get<std::string>(value.data));
                case ConstType::BoolConst: return new ConstExp(std::get<bool>(value.data));
                default: throw std::runtime_error("Unknown ConstType in clone");
            }
        }

        
};

class VarExp : public Expression {
    private:
        std::string var_name;

    public:
        VarExp(const std::string& v): Expression(ExpressionType::VAR_EXP), var_name(v) {}
        std::string get_var_name() const { return var_name; }
        Expression* clone() const override { return new VarExp(var_name); }
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
        MonadicExpression(MonadicOperator op, Expression* e): 
            Expression(ExpressionType::MON_EXP), 
            exp(e), 
            mon_op(op) {}

        ~MonadicExpression() override { delete exp; }

        MonadicOperator get_type() const { return mon_op; }
        Expression* get_right() const { return exp; }

        Expression* clone() const override { return new MonadicExpression(mon_op, exp->clone()); }
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
        Expression* e1;
        Expression* e2;
        BinaryOperator bin_op;

    public:
        BinaryExpression(BinaryOperator op, Expression* exp1, Expression* exp2): 
            Expression(ExpressionType::BIN_EXP), 
            e1(exp1), 
            e2(exp2), 
            bin_op(op) {} 

        ~BinaryExpression() override {
            delete e1;
            delete e2;
        }

        BinaryOperator get_type() const { return bin_op; }
        Expression* get_left() const { return e1; }
        Expression* get_right() const { return e2; }

        Expression* clone() const override {
            return new BinaryExpression(bin_op, e1->clone(), e2->clone());
        }
};

class AssignmentExpression : public Expression {
    private:
        std::string ident;
        Expression* exp;
        bool reassignment;

    public:
        AssignmentExpression(const std::string& identifier, Expression* e, bool reassignment): 
            Expression(ExpressionType::LET_EXP), 
            ident(identifier), exp(e), 
            reassignment(reassignment) {}

        ~AssignmentExpression() override { delete exp; }

        const std::string& get_id() const { return ident; }
        Expression* get_right() const { return exp; }
        bool is_reassign() const { return reassignment; }

        Expression* clone() const override {
            return new AssignmentExpression(ident, exp->clone(), reassignment);
        }
};

class IfExpression : public Expression {
    private:
        Expression* conditional;
        std::vector<Expression*> if_expressions;
        std::vector<Expression*> else_expressions;

    public: 
        IfExpression(Expression* e1, const std::vector<Expression*>& if_body, const std::vector<Expression*>& else_body): 
            Expression(ExpressionType::IF_EXP), 
            conditional(e1), 
            if_expressions(if_body), 
            else_expressions(else_body) {}

        ~IfExpression() override {
            for (auto expr : if_expressions) delete expr;
            for (auto expr : else_expressions) delete expr;
            delete conditional;
        }

        Expression* get_conditional() const {return conditional; }
        const std::vector<Expression*>& get_if_exps() const {return if_expressions; }
        const std::vector<Expression*>& get_else_exps() const {return else_expressions; }

        Expression* clone() const override {
            return new IfExpression(
                conditional->clone(), 
                clone_vector<Expression*>(if_expressions), 
                clone_vector<Expression*>(else_expressions)
            );
        }
};

class WhileExpression : public Expression {
    private:
        Expression* conditional;
        std::vector<Expression*> body_expressions;

    public: 
        WhileExpression(Expression* e1, const std::vector<Expression*>& while_body): 
            Expression(ExpressionType::WHILE_EXP), 
            conditional(e1), 
            body_expressions(while_body) {}
        
        ~WhileExpression() override {
            for (auto expr : body_expressions) delete expr;
            delete conditional;
        }

        Expression* get_conditional() const { return conditional; }
        const std::vector<Expression*>& get_body_exps() const { return body_expressions; }

        Expression* clone() const override {
            return new WhileExpression(
                conditional->clone(), 
                clone_vector<Expression*>(body_expressions)
            );
        }
};

class ListExpression : public Expression {
    private:
        std::vector<Expression*> elements;

    public:
        ListExpression(const std::vector<Expression*>& exps):
            Expression(ExpressionType::LIST_EXP), 
            elements(exps) {}

        ~ListExpression() override {
            for (auto expr : elements) delete expr;
        }

        const std::vector<Expression*>& get_elements() const { return elements; }
        Expression* access_element(int idx) const { return elements.at(idx); }

        Expression* clone() const override {
            return new ListExpression(
                clone_vector<Expression*>(elements)
            );
        }
};

class ListAccessExpression : public Expression {
    private:
        Expression* ident_exp;
        Expression* idx_exp;

    public:
        ListAccessExpression(Expression* identifer, Expression* idx): 
            Expression(ExpressionType::LIST_ACCESS_EXP), 
            ident_exp(identifer), 
            idx_exp(idx) {} 
        
        ~ListAccessExpression() override {
            if (ident_exp != nullptr) { delete ident_exp; }
            if (idx_exp != nullptr) { delete idx_exp; }
        }

        Expression* get_arr_exp() const { return ident_exp; }
        Expression* get_idx_exp() const { return idx_exp; }

        Expression* clone() const override {
            return new ListAccessExpression(ident_exp->clone(), idx_exp->clone());
        }
};

class ListModifyExpression : public Expression {
    private:
        Expression* ident_exp;
        Expression* idx_exp;
        Expression* exp;

    public:
        ListModifyExpression(Expression* identifier, Expression* index, Expression* new_val): 
            Expression(ExpressionType::LIST_MODIFY_EXP),
            ident_exp(identifier),
            idx_exp(index),
            exp(new_val) {} 

        ~ListModifyExpression() override {
            delete ident_exp;
            delete idx_exp;
            delete exp;
        }

        Expression* get_ident_exp() const { return ident_exp; }
        Expression* get_idx_exp() const { return idx_exp; }
        Expression* get_exp() const { return exp; }

        Expression* clone() const override {
            return new ListModifyExpression(
                ident_exp->clone(),
                idx_exp->clone(), 
                exp->clone()
            );
        }
};

class FunctionAssignmentExpression : public Expression {
    private:
        std::string func_name;
        std::vector<std::string> arg_names;
        std::vector<Expression*> body_expressions;

    public: 
        FunctionAssignmentExpression(
            const std::string& name, 
            const std::vector<std::string>& aliases, 
            const std::vector<Expression*>& function_body
        ): 
            Expression(ExpressionType::FUNC_ASSIGN_EXP), 
            func_name(name), 
            arg_names(aliases), 
            body_expressions(function_body) {}
        
        ~FunctionAssignmentExpression() override {
            for (auto expr : body_expressions) delete expr;
            arg_names.clear();
        }

        const std::string& get_name() const { return func_name; }
        const std::vector<Expression*>& get_body_exps() const {return body_expressions; }
        const std::vector<std::string>& get_arg_names() const { return arg_names; }
        size_t get_args_length() const { return arg_names.size(); }

        Expression* clone() const override {
            return new FunctionAssignmentExpression(
                func_name, 
                arg_names, 
                clone_vector<Expression*>(body_expressions)
            );
        }
};

class FunctionCallExpression : public Expression {
    private:
        std::string func_name;
        std::vector<Expression*> arg_expressions;

    public: 
        FunctionCallExpression(const std::string& name, const std::vector<Expression*>& args): 
            Expression(ExpressionType::FUNC_CALL_EXP), 
            func_name(name), 
            arg_expressions(args) {}
        
        ~FunctionCallExpression() override {
            for (auto expr : arg_expressions) delete expr;
        }

        const std::string& get_name() const { return func_name; }
        const std::vector<Expression*>& get_arg_exps() const {return arg_expressions; }
        size_t get_args_length() const { return arg_expressions.size(); }

        Expression* clone() const override {
            return new FunctionCallExpression(
                func_name, 
                clone_vector<Expression*>(arg_expressions)
            );
        }
        
};

#endif // EXPRESSION_HPP