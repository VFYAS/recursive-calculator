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
//frees up the memory used for variables storage

int
put_var(char *name, double value);
// sets the value for the specified variable or creates
// new variable with given name and value

double
get_var(const char *name);
// returns the value of the variable with the given name

int
init_vars(void);
// initializes the variable module with default values

Variable *
find_var(const char *name);
// searches for the variable with the given name
// returns pointer to the variable if exists and NULL otherwise

long long
request_vars(void);
// asks for the values of every variable in lexicographic order

#endif
