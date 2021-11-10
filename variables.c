#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "variables.h"

static VariableArray variable_array;

int
put_new_var(char *name, long double value)
{
    if (find_var(name) == NULL) {
        if (variable_array.last == variable_array.length) {
            variable_array.length <<= 1;
            variable_array.variables = realloc(variable_array.variables, variable_array.length
                                                                         * sizeof(*variable_array.variables));
            if (variable_array.variables == NULL) {
                delete_vars();
                return MEMORY_ERR_VARS;
            }
        }
        variable_array.variables[variable_array.last].value = value;
        variable_array.variables[variable_array.last].name = name;
        variable_array.last += 1;
        if (errno == ENOMEM) {
            delete_vars();
            return MEMORY_ERR_VARS;
        }
    } else {
        reset_var(name, value);
    }
    return 0;
}

int
reset_var(const char *name, long double value)
{
    Variable *variable;
    if ((variable = find_var(name)) != NULL) {
        variable->value = value;
        return 0;
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
get_var(const char *name)
{
    Variable *variable;
    if ((variable = find_var(name)) != NULL) {
        return variable->value;
    }
    errno = EINVAL;
    return INVALID_NAME;
}

int
init_vars(void)
{
    variable_array.last = 0;
    variable_array.length = INIT_AMOUNT;
    variable_array.variables = calloc(INIT_AMOUNT, sizeof(*variable_array.variables));
    if (errno == ENOMEM) {
        return MEMORY_ERR_VARS;
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

const char **
get_var_names(long long *length)
{
    const char **var_names = calloc(variable_array.last, sizeof(*var_names));
    if (length != NULL) {
        *length = variable_array.last;
    }
    for (long long i = 0; i < variable_array.last; ++i) {
        var_names[i] = variable_array.variables[i].name;
    }
    return var_names;
}
