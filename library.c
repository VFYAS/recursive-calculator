#include "library.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


static ExpressionTree *
parse_term(const char *parse_string, long long *parse_pos);

static ExpressionTree *
parse_factor(const char *parse_string, long long *parse_pos);

static ExpressionTree *
parse_number(const char *parse_string, long long *parse_pos);

static long double
parse_double(const char *parse_string, long long *pos, int *success);

static enum Operation
parse_op(const char *parse_string, long long *parse_pos);

static _Noreturn void
raise_error(const char *parse_string, enum ErrorCode ErrorCode);

static void
skip_spaces(const char *parse_string, long long *pos);


static ExpressionTree *parsing_tree = NULL;

static ExpressionTree *sep_tree = NULL;


static _Noreturn void
raise_error(const char *parse_string, enum ErrorCode ErrorCode)
{
    fprintf(stderr, "Error while parsing: ");
    delete_expression_tree(parsing_tree);
    delete_expression_tree(sep_tree);
    switch (ErrorCode) {
    case BRACKETS_BALANCE:
        fprintf(stderr, "The balance of brackets is broken at: %s\n", parse_string);
        break;
    case NO_OPERAND:
        fprintf(stderr, "No operand spotted at: %s\n", parse_string);
        break;
    case NO_OPERATION:
        fprintf(stderr, "No operation between operands at: %s\n", parse_string);
        break;
    case INVALID_OPERATION:
        fprintf(stderr, "Invalid operation %c\n", *parse_string);
        break;
    case INVALID_OPERAND:
        fprintf(stderr, "Invalid operand at: %s\n", parse_string);
        break;
    case DIVISION_BY_ZERO:
        fprintf(stderr, "Division by zero!\n");
        break;
    case MEMORY_ERROR:
        fprintf(stderr, "Out of memory\n");
        break;
    default:
        break;
    }
    exit(ErrorCode);
}

static void
skip_spaces(const char *parse_string, long long *pos)
{
    long long auto_pos = *pos;
    while (isspace(*(parse_string + auto_pos))) {
        ++auto_pos;
    }
    *pos = auto_pos;
}

static long double
parse_double(const char *parse_string, long long *pos, int *success)
{
    skip_spaces(parse_string, pos);
    long double res;
    char *end;
    res = strtold(parse_string + *pos, &end);
    if ((*parse_string != '\0') && (end != parse_string) && (errno != ERANGE)) {
        *pos = end - parse_string;
        *success = 1;
        return res;
    } else if (end == parse_string) {
        *success = 0;
        return 0;
    } else {
        raise_error(parse_string + *pos, INVALID_OPERAND);
    }
}

static enum Operation
parse_op(const char *parse_string, long long *parse_pos)
{
    skip_spaces(parse_string, parse_pos);
    if (!parse_string[*parse_pos]) {
        return OP_EOF;
    }
    enum Operation opcode;
    switch (parse_string[*parse_pos]) {
    case '+':
        opcode = OP_PLUS;
        break;
    case '*':
        opcode = OP_MULT;
        break;
    case '-':
        opcode = OP_MINUS;
        break;
    case '/':
        opcode = OP_DIV;
        break;
    case '%':
        opcode = OP_MOD;
        break;
    case ')':
        opcode = OP_RBR;
        break;
    default:
        opcode = INV_OP;
        break;
    }
    *parse_pos += 1;
    return opcode;
}

static ExpressionTree *
parse_factor(const char *parse_string, long long *parse_pos)
{
    skip_spaces(parse_string, parse_pos);
    ExpressionTree *tree1 = parse_number(parse_string, parse_pos);
    long long prev_pos = *parse_pos;
    while (1) {
        enum Operation op;
        if ((op = parse_op(parse_string, parse_pos)) == OP_EOF) {
            return tree1;
        } else if (op == INV_OP) {
            delete_expression_tree(tree1);
            raise_error(parse_string + *parse_pos - 1, INVALID_OPERATION);
        }
        switch (op) {
        case OP_MULT:
        case OP_DIV:
        case OP_MOD:
            break;
        default:
            *parse_pos = prev_pos;
            return tree1;
        }
        sep_tree = tree1;
        ExpressionTree *tree2 = parse_number(parse_string, parse_pos);
        ExpressionTree *parent = calloc(1, sizeof *parent);
        if (parent == NULL) {
            delete_expression_tree(tree2);
            raise_error(NULL, MEMORY_ERROR);
        }
        parent->left = tree1;
        parent->right = tree2;
        parent->opcode = op;
        tree1 = parent;
        parsing_tree = tree1;
        prev_pos = *parse_pos;
    }
}

static ExpressionTree *
parse_term(const char *parse_string, long long *parse_pos)
{
    long long prev_pos;
    skip_spaces(parse_string, parse_pos);
    ExpressionTree *tree1 = parse_factor(parse_string, parse_pos);
    prev_pos = *parse_pos;
    while (1) {
        enum Operation op;
        if ((op = parse_op(parse_string, parse_pos)) == OP_EOF) {
            return tree1;
        } else if (op == INV_OP) {
            delete_expression_tree(tree1);
            raise_error(parse_string + *parse_pos - 1, INVALID_OPERATION);
        }
        switch (op) {
        case OP_PLUS:
        case OP_MINUS:
            break;
        case OP_RBR:
            *parse_pos = prev_pos;
        default:
            return tree1;
        }
        sep_tree = tree1;
        ExpressionTree *tree2 = parse_factor(parse_string, parse_pos);
        ExpressionTree *parent = calloc(1, sizeof *parent);
        if (parent == NULL) {
            delete_expression_tree(tree2);
            raise_error(NULL, MEMORY_ERROR);
        }
        parent->left = tree1;
        parent->right = tree2;
        parent->opcode = op;
        tree1 = parent;
        parsing_tree = parent;
        prev_pos = *parse_pos;
    }
}

static ExpressionTree *
parse_number(const char *parse_string, long long int *parse_pos)
{
    int success;
    ExpressionTree *res;
    skip_spaces(parse_string, parse_pos);
    if (parse_string[*parse_pos] == '(') {
        *parse_pos += 1;
        ExpressionTree *t = parse_term(parse_string, parse_pos);
        skip_spaces(parse_string, parse_pos);
        if (parse_string[*parse_pos] != ')') {
            delete_expression_tree(t);
            raise_error(parse_string + *parse_pos, BRACKETS_BALANCE);
        }
        res = calloc(1, sizeof(*res));
        if (res == NULL) {
            delete_expression_tree(t);
            raise_error(NULL, MEMORY_ERROR);
        }
        res->opcode = OP_LBR;
        res->left = t;
        res->right = calloc(1, sizeof(*res->right));
        res->right->opcode = OP_RBR;
        *parse_pos += 1;
    } else if (isdigit(parse_string[*parse_pos])) {
        long double number = parse_double(parse_string, parse_pos, &success);
        res = calloc(1, sizeof *res);
        if (res == NULL) {
            raise_error(NULL, MEMORY_ERROR);
        }
        res->opcode = OP_NUM;
        res->num = number;
    } else {
        raise_error(parse_string + *parse_pos, INVALID_OPERAND);
    }
    return res;
}

ExpressionTree *
syntax_analyse(const char *str)
{
    long long pos = 0;
    parsing_tree = parse_term(str, &pos);
    skip_spaces(str, &pos);
    if (str[pos] != '\0') {
        if (str[pos] == ')') {
            raise_error(str + pos, BRACKETS_BALANCE);
        } else {
            raise_error(str + pos, INVALID_OPERATION);
        }
    }
    return parsing_tree;
}

long double
calculate(ExpressionTree *tree)
{
    if (tree == NULL) {
        errno = EINVAL;
        return 0;
    }
    if (tree->opcode == OP_NUM) {
        return tree->num;
    }
    calculate(tree->left);
    calculate(tree->right);

    switch (tree->opcode) {
    case OP_PLUS:
        tree->num = tree->left->num + tree->right->num;
        break;
    case OP_MINUS:
        tree->num = tree->left->num - tree->right->num;
        break;
    case OP_MULT:
        tree->num = tree->left->num * tree->right->num;
        break;
    case OP_DIV:
        if (tree->right->num == 0) {
            raise_error(NULL, DIVISION_BY_ZERO);
        }
        tree->num = tree->left->num / tree->right->num;
        break;
    case OP_MOD:
        if (tree->right->num == 0) {
            raise_error(NULL, DIVISION_BY_ZERO);
        }
        tree->num = (long long) tree->left->num % (long long) tree->right->num;
        break;
    case OP_RBR:
        return 0;
    case OP_LBR:
        tree->num = tree->left->num;
        break;
    default:
        delete_expression_tree(parsing_tree);
        fprintf(stderr, "Internal error\n");
        _exit(INTERNAL_ERROR);
    }
    return tree->num;
}

void
delete_expression_tree(ExpressionTree *parse_tree)
{
    if (parse_tree == NULL) {
        return;
    }
    delete_expression_tree(parse_tree->left);
    delete_expression_tree(parse_tree->right);
    if (parse_tree == parsing_tree) {
        parsing_tree = NULL;
    }
    if (parse_tree == sep_tree) {
        sep_tree = NULL;
    }

    free(parse_tree);
}