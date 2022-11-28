/*Narasimhan Kovalai
Assignment-2
Operating System LAB-2
IMPLEMENTATION OF SHELL
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*Definitions */
#define HISTORY_SIZE 10
#define MAX_COMMAND_SIZE 100

#define NO_OF_BUILTIN_COMMANDS 3
char* history_arr[1000];
int last = 0;

/*Implemented a circular queue of commands for history */
int rear = -1, front = -1;
int current_top = 1;
char commands[HISTORY_SIZE][MAX_COMMAND_SIZE];

/*List of the built-in commands of the shell */
char *list_of_commands[NO_OF_BUILTIN_COMMANDS] = {"cd", "history", "exit"};

/*Function to display initialize shell */
void init()
{
    system("clear");
}

/* Function to display current working directory. */
void display_cwd()
{

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("\nDirectory: %s >> ", cwd);
    }
    else
    {
        perror("getcwd() error");
    }
}
/*Utility functions for the circular queue of commands */

// Function to Check if the queue is full.
// int isfull()
// {

//     return ((front == 0 && rear == HISTORY_SIZE - 1) || (rear == (front - 1) % (HISTORY_SIZE - 1)));
// }

/* Function to dequeue a command */
// void dequeue()
// {

//     strcpy(commands[front], "");
//     if (front == rear)
//     {
//         front = -1;
//         rear = -1;
//     }
//     else if (front == HISTORY_SIZE - 1)
//     {
//         front = 0;
//     }
//     else
//     {
//         front++;
//     }
// }

/* Function to enqueue command */
// void enqueue(char *command)
// {

//     if (front == -1)
//     {
//         front = rear = 0;
//     }
//     else if (rear == HISTORY_SIZE - 1 && front != 0)
//     {
//         rear = 0;
//     }
//     else
//     {
//         rear++;
//     }

//     strcpy(commands[rear], command);
// }

/*Function to add command to the history. */
void addcommand(char *command)
{

    history_arr[last] = command;
    last++;
    // if (isfull())
    // {

    //     dequeue();
    //     current_top++;
    // }

    // enqueue(command);
}
/* Function to check if the input is empty or not. */
int isemptyinput(const char *s)
{
    while (*s != '\0')
    {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

/* Function to take input */
int takeinput(char *str)
{
    char *buf;

    buf = readline("$ ");
    if (strlen(buf) != 0 && !isemptyinput(buf))
    {
        if (buf[0] != '!')
            addcommand(buf);
        strcpy(str, buf);
        return 0;
    }
    else
    {
        return 1;
    }
}

/*Function to Parse the command through line */
void parseSpace(char *str, char **parsed)
{
    int i;

    for (i = 0; i < MAX_COMMAND_SIZE; i++)
    {
        parsed[i] = strsep(&str, " ");
        if (parsed[i] == NULL)
        {
            break;
        }
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

/*Functions for handling the built-in commands */

// cd Command
void cdcommand(char *path)
{

    chdir(path);
}

// history command
void historycommand()
{

    // int c = current_top;
    // if (front == -1)
    // {
    //     return;
    // }
    // printf("\n");
    // if (rear >= front)
    // {
    //     for (int i = front; i <= rear; i++)
    //         printf("%d: %s\n", c++, commands[i]);
    // }
    // else
    // {
    //     for (int i = front; i < HISTORY_SIZE; i++)
    //         printf("%d: %s\n", c++, commands[i]);

    //     for (int i = 0; i <= rear; i++)
    //         printf("%d: %s\n", c++, commands[i]);
    // }
    int i = 0;
    if (last <= HISTORY_SIZE)
    {

        for (i = last - 1; i >= 0; i--)
        {

            printf("%s\n", history_arr[i]);
        }
    }
    else
    {
        for (i = last - 1; i >= last - HISTORY_SIZE; i--)
        {

            printf("%s\n", history_arr[i]);
        }
    }
}

// exit command
void exitcommand()
{
    printf("Exiting...\n");
    system("clear");
    exit(0);
}

/*Function to process built-in commands */
int isbuiltincommand(char **parsedcommand)
{

    int i = 0, command_num = 0;

    for (i = 0; i < NO_OF_BUILTIN_COMMANDS; i++)
    {

        if (strcmp(parsedcommand[0], list_of_commands[i]) == 0)
        {
            // printf("Hello, I am built-in command\n");
            command_num = i + 1;
            break;
        }
    }

    switch (command_num)
    {
    case 0:
        break;
    case 1:
        cdcommand(parsedcommand[1]);
        return 1;
    case 2:
        historycommand();
        return 1;
    case 3:
        exitcommand();
        return 1;
    default:
        break;
    }

    return 0;
}

/*Function to check for redirection commands */
int isinputoutput(char **parsed, char **str, int choice)
{

    char *s;
    if (!choice)
        s = "<";
    else
        s = ">";

    int flag = 0, i = 0;

    for (i = 0; parsed[i] != NULL; i++)
    {
        if (strcmp(parsed[i], s) == 0 && !flag && parsed[i + 1] != NULL)
        {

            flag = 1;
            *str = parsed[i + 1];
            parsed[i] = NULL;
            return 1;
        }
    }

    return 0;
}

/*Function to execute program with arguements*/
void executecommand(char **parsed)
{

    pid_t pid = fork();

    if (pid == -1)
    {
        printf("Forking child failed..");
        return;
    }
    else if (pid == 0)
    {

        char *input_file = NULL, *output_file = NULL;

        int I = isinputoutput(parsed, &input_file, 0);
        int O = isinputoutput(parsed, &output_file, 1);

        if (I)
        {
            int fd0 = open(input_file, O_RDONLY);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
            output_file = NULL;
        }
        if (O)
        {

            int fd1 = open(output_file, O_RDWR | O_CREAT, 0644);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
            output_file = NULL;
        }

        execvp(parsed[0], parsed);
        fprintf(stderr, "Failed to execute %s\n", parsed[0]);
        return;
    }
    else
    {

        waitpid(pid, NULL, 0);
        return;
    }
}

/*Function to process command */
void processcommand(char *inputString)
{
    char *parsedcommand[MAX_COMMAND_SIZE];
    int executeFlag = 1;

    parseSpace(inputString, parsedcommand);

    if (parsedcommand[0] == NULL)
        return;

    if (isbuiltincommand(parsedcommand))
    {

        executeFlag = 0;
    }

    if (executeFlag)
    {

        executecommand(parsedcommand);
    }
}

int main(int argc, char **argv)
{

    char inputString[MAX_COMMAND_SIZE];

    init();

    while (1)
    {

        display_cwd();

        if (takeinput(inputString))
        {
            continue;
        }
        processcommand(inputString);
    }

    return 0;
}
