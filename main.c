#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "library.h"

int
main(void)
{
    char str[256];
    if (!fgets(str, sizeof(str), stdin)) {
        fprintf(stderr, "Failed to read string!\n");
        exit(1);
    }
    *strchr(str, '\n') = '\0';
    ExpressionTree *tree = syntax_analyse(str);
    long double result = calculate(tree);
    delete_expression_tree(tree);
    printf("%Lf\n", result);
    return 0;
}
