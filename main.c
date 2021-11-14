#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "library.h"
#include "variables.h"

enum
{
    INIT_STR_SIZE = 256
};

enum ErrorMain
{
    EXPR_ERROR = 0x01,
    VAR_ERROR = 0x02,
    CALC_ERROR = 0x03
};

int
main(void)
{
    char *str = calloc(INIT_STR_SIZE, sizeof(*str));
    int c;
    long long idx = 0, str_len = INIT_STR_SIZE;
    c = fgetc(stdin);
    if (c == EOF) {
        // if the expression is empty
        free(str);
        fprintf(stderr, "Empty statement!\n");
        exit(EXPR_ERROR);
    }
    while (c != EOF) {
        do {
            if (idx == str_len) {
                str_len <<= 1;
                str = realloc(str, str_len * sizeof(*str));
            }
            str[idx++] = (char) c;
        } while ((c = fgetc(stdin)) != '\n' && c != EOF);
        str[idx] = '\0';
        idx = 0;

        ExpressionTree *tree = syntax_analyse(str);
        if (tree != NULL) {
            int continue_computing;
            do {
                long long len = request_vars();
                if (len == -1 || errno == ENOMEM) {
                    // if error occurred while reading variables' values
                    delete_expression_tree(tree);
                    delete_vars();
                    free(str);
                    exit(VAR_ERROR);
                }
                int success;
                double result = calculate(tree, &success);
                if (success == 0) {
                    // if error in calculations occurred
                    delete_expression_tree(tree);
                    delete_vars();
                    free(str);
                    exit(CALC_ERROR);
                }
                printf("%f\n", result);

                if (len == 0) {
                    continue_computing = 0;
                } else {
                    do {
                        while ((c = fgetc(stdin)) == '\n') {}
                        if (c == EOF) {
                            delete_expression_tree(tree);
                            delete_vars();
                            free(str);
                            exit(0);
                        }
                        continue_computing = (c == 'y') ? 1 : 0;
                    } while (c != 'y' && c != 'n');
                }
            } while (continue_computing);
        } else {
            free(str);
            exit(EXPR_ERROR);
        }
        delete_expression_tree(tree);
        delete_vars();
        do {
            c = fgetc(stdin);
        } while (c == '\n');
    }
    free(str);
    return 0;
}
