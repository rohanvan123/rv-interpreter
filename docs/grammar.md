**Ambiguous Grammar**

Expression ::= ConstExp | MonadicExp | BinaryExp | VarExp | IfExp | GroupExp

Const ::= IntConst | BoolConst | StringConst

MonadicExp ::= MonOp Expression

MonOp ::= NotOp | IntNegOp | PrintOp

BinaryExp ::= Expression BinOp Expression

BinOp ::= IntTimesOp | IntPlusOp | EqualsOp | IntMinusOp | IntDivOp | ModOp

IfExp ::= IF LBRACE Expression RBRACE ELSE Expression LBRACE RBRACE

**Unambiguous Grammar**

Expression ::= ComparisonExp | Declaration

Declaration ::= LET VarExp EQ ComparisonExp

EqualityExp ::= ComparisonExp ( (EQ NEQ) ComparisonExp )\*

ComparisonExp ::= PlusExp ( (LT LTE GT GTE ) ComparisonExp )\*

PlusExp ::= FactorExp ( (IntPlusOp | IntMinusOp) FactorExp)\*

FactorExp ::= MonadicExpression ( (IntTimesOp | IntDivOp | ModOp) MonadicExpression )\*

MonadicExpression ::= MonOp MonadicExpression | AtomicExpression

AtomicExpression ::= ConstExp | VarExp | LPAREN Expression RPAREN
