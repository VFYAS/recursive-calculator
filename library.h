#ifndef CALCULATOR_LIBRARY_H
#define CALCULATOR_LIBRARY_H

enum Operation
{
    OP_EOF,
    OP_NUM,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_LBR,
    OP_RBR,
    OP_MOD,
    INV_OP,
    OP_VAR
};

typedef struct ExpressionTree
{
    enum Operation opcode;
    struct ExpressionTree *left;
    struct ExpressionTree *right;
    char *name_var;
    long double num;
} ExpressionTree;

enum ErrorCode
{
    BRACKETS_BALANCE = 0x01,
    NO_OPERAND = 0x02,
    NO_OPERATION = 0x03,
    INVALID_OPERATION = 0x04,
    INVALID_OPERAND = 0x05,
    DIVISION_BY_ZERO = 0x06,
    INTERNAL_ERROR = 0x07,
    MEMORY_ERROR_CALCULATOR = 0x08
};

void
delete_expression_tree(ExpressionTree *parse_tree);

long double
calculate(ExpressionTree *tree);

ExpressionTree *
syntax_analyse(const char *str);

#endif
