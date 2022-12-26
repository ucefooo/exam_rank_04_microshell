#include "microshell.h"

int ft_strlen(char *s)
{
    int i=0;
    while(s[i])
        i++;
    return (i);
}

void print_err(char *s1,char *s2,int n)
{
    write(2,s1,ft_strlen(s1));
    if (s2)
        write(2,s2,ft_strlen(s2));
    write(2,"\n",1);
    if (n)
        exit(1);
}

tokens *init_token()
{
    tokens *tmp;
    if (!(tmp = (tokens*)malloc(sizeof(tokens))))
        print_err("fatal",0,1);
    if (!(tmp->cmds = (char **)malloc(sizeof(char*)*1024)))
        print_err("err fatal",0,1);
    tmp->std_in=0;
    tmp->std_out=1;
    return (tmp);
}

int add_cmd(char **v, tokens *token, int i)
{
    int j=0;
    while(v[i] && strcmp(v[i],"|") && strcmp(v[i],";"))
    {
        token->cmds[j++]=v[i++];
    }
    i++;
    token->cmds[j]=NULL;
    return i;
}

void ft_pip(tokens *token)
{
    int pip[2];
    pipe(pip);
    token->next=init_token();
    token->next->std_in=pip[0];
    token->std_out=pip[1];
}

void ft_free(tokens *token)
{
    if (token)
    {
        if (token->cmds)
            free(token->cmds);
        free(token);
        token=NULL;
    }
}

void exec_cmd(tokens *token,int *start,int i,char **env)
{
    int pid;
    if (!strcmp(token->cmds[0],"cd"))
    {
        if (token->cmds[2] || !token->cmds[1])
            print_err("too many args cd",0,0);
        else if (chdir(token->cmds[1]))
            print_err("cnat go ",token->cmds[1],0);
        *start=i;
        return;
    }
    pid =fork();
    if (pid < 0)
        print_err("fatal",0,1);
    if (pid ==0)
    {
        if (token->std_out!=1)
        {
            close(token->next->std_in);
            if (dup2(token->std_out,1) == -1)
                print_err("fata",0,1);
        }
        if (token->std_in!=0)
        {
            if (dup2(token->std_in,0)==-1)
                print_err("fatal",0,1);
        }
        execve(token->cmds[0],token->cmds,env);
        print_err("cant exe",token->cmds[0],1);
    }
    if (token->std_out!=1)
        close(token->std_out);
    if (token->std_in!=0)
        close(token->std_in);
    if (token->std_out==1)
    {
        while(*start<i)
        {
            waitpid(0,0,0);
            (*start)++;
        }
    }
}

int main(int c, char **v, char **env)
{
    int i=1,start=i;
    tokens *token,*before;
    while(i <c)
    {
        if (token==NULL || start ==i)
            token =init_token();
        if (!strcmp(v[i],";"))
        {
            i++;
            continue;
        }
        i = add_cmd(v,token,i);
        if (v[i-1] && !strcmp(v[i-1],"|"))
            ft_pip(token);
        exec_cmd(token,&start,i,env);
        before=token;
        if (token->std_out!=1)
            token=token->next;
        ft_free(before);
    }
    return 0;
}
