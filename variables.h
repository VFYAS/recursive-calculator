#ifndef CALCULATOR_VARIABLES_H
#define CALCULATOR_VARIABLES_H

enum
{
    INIT_AMOUNT = 16
};

enum
{
    ERR_IN_VAR = -1
};

typedef struct
{
    char *name;
    double value;
} Variable;

typedef struct
{
    Variable *variables;
    long long length;
    long long last;
} VariableArray;

void
delete_vars(void);

int
put_var(char *name, double value);

double
get_var(const char *name);

int
init_vars(void);

Variable *
find_var(const char *name);

long long
request_vars(void);

#endif
