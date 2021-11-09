#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "variables.h"

static VariableArray variable_array;

int
put_new_var(char *name, long double value)
{
    if (!reset_var(name, value)) {
        if (variable_array.last == variable_array.length) {
            variable_array.length <<= 1;
            variable_array.variables = realloc(variable_array.variables, variable_array.length
                                                                         * sizeof(*variable_array.variables));
            if (variable_array.variables == NULL) {
                delete_vars();
                return MEMORY_ERR;
            }
        }
        variable_array.variables[variable_array.last].value = value;
        variable_array.variables[variable_array.last].name = strdup(name);
        if (errno == ENOMEM) {
            delete_vars();
            return MEMORY_ERR;
        }
    }
    return 0;
}

int
reset_var(char *name, long double value)
{
    for (long long i = 0; i < variable_array.last; ++i) {
        if (!strcmp(variable_array.variables[i].name, name)) {
            variable_array.variables[i].value = value;
            return 0;
        }
    }
    return INVALID_NAME;
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

long double
get_var(char *name, int *success)
{
    for (long long i = 0; i < variable_array.last; ++i) {
        if (!strcmp(variable_array.variables[i].name, name)) {
            *success = 1;
            return variable_array.variables[i].value;
        }
    }
    errno = EINVAL;
    *success = 0;
    return INVALID_NAME;
}

int
init_vars(void)
{
    variable_array.last = variable_array.length = 0;
    variable_array.variables = calloc(INIT_AMOUNT, sizeof(*variable_array.variables));
    if (errno == ENOMEM) {
        return MEMORY_ERR;
    }
    return 0;
}
