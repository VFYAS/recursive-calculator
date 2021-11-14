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
    OP_LBR, //opening bracket
    OP_RBR, //closing bracket
    OP_VAR, //variable
    INV_OP //invalid operation
};

typedef struct ExpressionTree
{
    enum Operation opcode;
    struct ExpressionTree *left;
    struct ExpressionTree *right;
    char *name_var;
    double num;
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
    MEMORY_ERROR = 0x08,
    INVALID_VAR = 0x09
};

enum
{
    MAX_VAR_NAME_LEN = 6
};

void
delete_expression_tree(ExpressionTree *parse_tree);
// frees up the memory used by calculator

double
calculate(ExpressionTree *tree, int *success);
// calculates the value by the specified expression tree

ExpressionTree *
syntax_analyse(const char *str);
// analyses the given expression, converting it into tree

#endif
