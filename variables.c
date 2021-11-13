#include "variables.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

static VariableArray variable_array;

static int
cmpstring(const void *a, const void *b);

int
put_var(char *name, double value)
{
    Variable *found;
    if ((found = find_var(name)) == NULL) {
        if (variable_array.last == variable_array.length) {
            variable_array.length <<= 1;
            variable_array.variables = realloc(variable_array.variables, variable_array.length
                                                                         * sizeof(*variable_array.variables));
            if (variable_array.variables == NULL) {
                delete_vars();
                return ERR_IN_VAR;
            }
        }
        variable_array.variables[variable_array.last].value = value;
        variable_array.variables[variable_array.last].name = name;
        variable_array.last += 1;
        if (errno == ENOMEM) {
            delete_vars();
            return ERR_IN_VAR;
        }
    } else {
        found->value = value;
    }
    return 0;
}

void
delete_vars(void)
{
    for (long long i = 0; i < variable_array.last; ++i) {
        free(variable_array.variables[i].name);
        variable_array.variables[i].name = NULL;
    }
    variable_array.last = variable_array.length = 0;
    free(variable_array.variables);
    variable_array.variables = NULL;
}

double
get_var(const char *name)
{
    Variable *variable;
    if ((variable = find_var(name)) != NULL) {
        return variable->value;
    }
    errno = EINVAL;
    return 0;
}

int
init_vars(void)
{
    variable_array.last = 0;
    variable_array.length = INIT_AMOUNT;
    variable_array.variables = calloc(INIT_AMOUNT, sizeof(*variable_array.variables));
    if (errno == ENOMEM) {
        return ERR_IN_VAR;
    }
    return 0;
}

Variable *
find_var(const char *name)
{
    for (long long i = 0; i < variable_array.last; ++i) {
        if (!strcmp(variable_array.variables[i].name, name)) {
            return &variable_array.variables[i];
        }
    }
    return NULL;
}

static int
cmpstring(const void *a, const void *b)
{
    return strcmp((const char *) ((const Variable *) a)->name, (const char *) ((const Variable *) b)->name);
}

long long
request_vars(void)
{
    qsort(variable_array.variables, variable_array.last, sizeof(*(variable_array.variables)),
          cmpstring);
    for (long long i = 0; i < variable_array.last; ++i) {
        char *value_str = calloc(INIT_AMOUNT, sizeof *value_str), *end;
        if (value_str == NULL) {
            fprintf(stderr, "Not enough memory!\n");
            return ERR_IN_VAR;
        }
        long long idx = 0, len = INIT_AMOUNT;
        double value;
        int c;
        while ((c = fgetc(stdin)) != '\n' && c != EOF) {
            if (idx == len) {
                len <<= 1;
                value_str = realloc(value_str, len * sizeof *value_str);
            }
            value_str[idx++] = (char) c;
        }
        value_str[idx] = '\0';
        value = strtod(value_str, &end);
        if (value_str[0] == '\0' || *end != '\0' || errno == ERANGE) {
            fprintf(stderr, "Error while entering a variable value!\n");
            free(value_str);
            return ERR_IN_VAR;
        } else {
            free(value_str);
            variable_array.variables[i].value = value;
        }
    }
    return variable_array.last;
}
