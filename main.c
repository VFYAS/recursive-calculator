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
    READ_ERROR = 0x01
};

int
check_scanf(int scanf_ret, int items_amount)
{
    if (((scanf_ret != items_amount) && (scanf_ret >= 0)) || ferror(stdin)) {
        if (ferror(stdin)) {
            fprintf(stderr, "Failed to read: %s\n", strerror(errno));
        } else {
            fprintf(stderr, "Incorrect format!\n");
        }
        return READ_ERROR;
    } else if (scanf_ret == EOF) {
        return EOF;
    } else {
        return 0;
    }
}

int
main(void)
{
    char *str = calloc(INIT_STR_SIZE, sizeof(*str));
    int c;
    long long idx = 0, str_len = INIT_STR_SIZE;
    printf("Input string with expression:\n");
    c = fgetc(stdin);
    while (c != EOF) {
        do {
            if (idx == str_len) {
                str_len <<= 1;
                str = realloc(str, str_len * sizeof(*str));
            }
            str[idx++] = (char) c;
        } while ((c = fgetc(stdin)) != '\n');
        str[idx] = '\0';
        idx = 0;

        ExpressionTree *tree = syntax_analyse(str);
        if (tree != NULL) {
            int continue_computing;
            do {
                long long len;
                long double value;
                const char **var_names = get_var_names(&len);
                for (long long i = 0; i < len; ++i) {
                    int ret;
                    printf("Input value for variable %s:\n", var_names[i]);
                    if ((ret = check_scanf(scanf("%Lf", &value), 1)) != 0) {
                        delete_expression_tree(tree);
                        delete_vars();
                        free(var_names);
                        free(str);
                        if (ret == EOF) {
                            fprintf(stderr, "Unexpected end of line\n");
                        }
                        exit(READ_ERROR);
                    }
                    reset_var(var_names[i], value);
                }
                int success;
                long double result = calculate(tree, &success);
                free(var_names);
                if (success == 0) {
                    continue_computing = 0;
                    continue;
                }
                printf("%Lf\n", result);

                if (len == 0) {
                    continue_computing = 0;
                } else {
                    do {
                        printf("Continue with the same expression?\n[y/n]\n");
                        while ((c = fgetc(stdin)) == '\n') {}
                        if (c == EOF) {
                            delete_expression_tree(tree);
                            delete_vars();
                            free(str);
                            exit(READ_ERROR);
                        }
                        continue_computing = (c == 'y') ? 1 : 0;
                    } while (c != 'y' && c != 'n');
                }
            } while (continue_computing);
        }
        delete_expression_tree(tree);
        delete_vars();
        printf("Input string with expression:\n");
        while (c == '\n') {
            c = fgetc(stdin);
        }
    }
    free(str);
    return 0;
}
