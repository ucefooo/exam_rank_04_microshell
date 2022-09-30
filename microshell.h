#ifndef MICROSHELL_H
#define MICROSHELL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct token{
    char **cmds;
    int std_in;
    int std_out;
    struct token *next;
}tokens;

#endif