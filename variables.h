#ifndef CALCULATOR_VARIABLES_H
#define CALCULATOR_VARIABLES_H

enum
{
    INIT_AMOUNT = 16
};

enum Errors
{
    MEMORY_ERR_VARS = 0x01,
    INVALID_NAME = 0x02
};

typedef struct
{
    char *name;
    long double value;
} Variable;

typedef struct
{
    Variable *variables;
    long long length;
    long long last;
} VariableArray;

int
reset_var(const char *name, long double value);

void
delete_vars(void);

int
put_new_var(char *name, long double value);

long double
get_var(const char *name);

int
init_vars(void);

Variable *
find_var(const char *name);

const char **
get_var_names(long long *length);

#endif
