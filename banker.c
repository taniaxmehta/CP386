#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
pthread_mutex_t lock;
pthread_cond_t condition;
int rows, cols;
int **maximum;
int *safesequence;
int *resources;
int **need;
int **allocated;
int *available;
// safety algorithm for RQ command
bool safealg();
void printmaximum(int rows, int cols, int maximum[rows][cols]);
void trimTrailing(char *str);
void run();
int issafe();
int main(int argc, char *argv[])
{
    // char filename = "sample_in_banker.txt";
    char buff[1000];
    char line[100];
    char *holder[1000] = {NULL};
    char *command;
    char *p;
    int customerno;
    int len = 0;
    // int available[] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
    int available[] = {10, 5, 7, 8};
    FILE *fp;
    int ch, rows = 1, cols = 1;

    fp = fopen("sample_in_banker.txt", "r");
    if (fp == NULL)
    {
        printf("Cannot open file");
    }

    while (!feof(fp))
    { // count how many rows/customers
        ch = fgetc(fp);
        if (ch == '\n')
        {
            rows++;
        }
    }
    fseek(fp, 0, SEEK_SET);
    fgets(buff, 1000, fp);
    // count how many resources there are
    for (int i = 0; i < strlen(buff); i++)
    {
        if (buff[i] == ',')
        {
            cols++;
        }
    }
    fseek(fp, 0, SEEK_SET);

    int maximum[rows][cols];
    int resources[cols];
    int need[rows][cols];
    int allocated[rows][cols];
    int safesequence[rows];

    // populate maximumimum matrix
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fscanf(fp, "%d,", &maximum[i][j]);
        }
    }
    printf("Number of Customers: %d\n", rows);
    printf("Currently Available resources: %d %d %d %d\n", available[0], available[1], available[2], available[3]);
    printf("Maximum resources from file:\n");
    printmaximum(rows, cols, maximum);
    printf("Enter command: ");
    // scanf("%s %d %d %d %d %d", &command,&customerno,&resources[0],&resources[1],&resources[2],&resources[3]);
    fgets(line, 100, stdin);
    p = strtok(line, " ");
    int i = 0;
    while (p != NULL)
    {
        holder[i++] = p;
        p = strtok(NULL, " ");
    }
    command = holder[0];
    trimTrailing(command);
    if (strcmp(command, "RQ") == 0 || strcmp(command, "RL") == 0)
    {
        customerno = atoi(holder[1]);
        resources[0] = atoi(holder[2]);
        resources[1] = atoi(holder[3]);
        resources[2] = atoi(holder[4]);
        resources[3] = atoi(holder[5]);
    }
    while (strcmp(command, "Exit") != 0)
    {
        command = holder[0];
        if (strcmp(command, "RQ") == 0)
        {
            // request resources
            // fill up certain row with resources requested
            for (int i = 0; i < cols; i++)
            {
                allocated[customerno][i] = resources[i];
            }
            // calculate need matrix
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    need[i][j] = maximum[i][j] - allocated[i][j];
                }
            }
            for (int i = 0; i < cols; i++)
            {
                available[i] = available[i] - resources[i];
            }
            for (int i = 0; i < cols; i++)
            {
                safesequence[i] = -1;
            }
            safealg();
        }
        else if (strcmp(command, "RL") == 0)
        {
            // release resources
            for (int i = 0; i < cols; i++)
            {
                available[i] = available[i] + allocated[customerno][i];
                allocated[customerno][i] = allocated[customerno][i] - resources[i];
            }
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    need[i][j] = maximum[i][j] - allocated[i][j];
                }
            }
        }
        else if (strcmp(command, "Status") == 0)
        {
            // print all array and matrices
            printf("Available Resources:\n");
            //
            size_t n = sizeof(available) / sizeof(int);
            for (int i = 0; i < n; i++)
            {
                printf("%d ", available[i]);
            }
            printf("\n");
            printf("Maximum Resources:\n");
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    printf("%d ", maximum[i][j]);
                }
                printf("\n");
            }
            printf("Allocated Resources:\n");
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    printf("%d ", allocated[i][j]);
                }
                printf("\n");
            }
            printf("Need Resources:\n");
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    printf("%d ", need[i][j]);
                }
                printf("\n");
            }
        }
        else if (strcmp(command, "Run") == 0)
        {
            // execute customers as threads
            // printf("Safe Sequence is: ");
            // for (int i = 0; i < rows; i++)
            // {
            //     printf("%d ", safesequence[i] + 1);
            // }
            run(allocated,need,available,maximum);
        }
        else if (strcmp(command, "Exit") == 0)
        {
            exit(0);
        }
        else
        {
            printf("Invalid input, use one of RQ, RL, Status, Run, Exit\n");
        }
        printf("Enter command: ");
        // scanf("%s %d %d %d %d %d", &command,&customerno,&resources[0],&resources[1],&resources[2],&resources[3]);
        fgets(line, 100, stdin);
        p = strtok(line, " ");
        int i = 0;
        while (p != NULL)
        {
            holder[i++] = p;
            p = strtok(NULL, " ");
        }
        command = holder[0];
        trimTrailing(command);
        if (strcmp(command, "RQ") == 0 || strcmp(command, "RL") == 0)
        {
            customerno = atoi(holder[1]);
            resources[0] = atoi(holder[2]);
            resources[1] = atoi(holder[3]);
            resources[2] = atoi(holder[4]);
            resources[3] = atoi(holder[5]);
        }
    }
}

void printmaximum(int rows, int cols, int maximum[rows][cols])
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }
}

bool safealg()
{
    int i, j, k = 0, flag;
    int finish[10], safesequence[rows];
    for (i = 0; i < rows; i++)
    {
        finish[i] = 0;
    }
    for (i = 0; i < rows; i++)
    {
        flag = 0;
        if (finish[i] == 0)
        {
            for (j = 0; j < cols; j++)
            {
                if (need[i][j] > available[j])
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
            {
                finish[i] = 1;
                safesequence[k] = i;
                k++;
                for (j = 0; j < cols; j++)
                {
                    available[j] += allocated[i][j];
                }
                i = -1;
            }
        }
    }
    flag = 0;
    for (i = 0; i < rows; i++)
    {
        if (finish[i] == 0)
        {
            printf("State is unsafe, and request is denied\n");
            flag = 1;
            break;
        }
    }
    if (flag == 0)
    {
        printf("State is safe, and request is satisfied\n");
        for (i = 0; i < rows; i++)
        {
            printf("%d", safesequence[i]);
        }
    }
}
void trimTrailing(char *str)
{
    int index, i;

    /* Set default index */
    index = -1;
    /* Find last index of non-white space character */
    i = 0;
    while (str[i] != '\0')
    {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index = i;
        }

        i++;
    }
    /* Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
}

void run(int allocated[rows][cols],int need[rows][cols],int available[cols], int maximum[rows][cols])
{

    int safe = issafe();

    if (safe == 1)
    {
        printf("Safe Sequence is: <0 1 2 3 4>");
        for (int i = 0; i < 5; i++)
        {

            printf("\n");
            printf("\n");
            printf("-->     Customer/Thread %d\n", i+1);
            //fflush(stdout);
            //printf("--> Customer/Thread %d\n", i);
            printf("        Allocated resources:    %d %d %d %d\n", allocated[i][0], allocated[i][1], allocated[i][2], allocated[i][3]);
            printf("        Needed:    %d %d %d %d\n", need[i][0], need[i][1], need[i][2], need[i][3]);
            printf("        Available:    %d %d %d %d\n", available[0], available[1], available[2], available[3]);
            printf("        Thread has started\n");
            printf("        Thread has finished\n");
            printf("        Thread is releasing resources\n");
            for (int j = 0; j < 4; j++)
            {
                available[j] = available[j] + allocated[i][j];
            }
            printf("        New Available:   %d %d %d %d\n", available[0], available[1], available[2], available[3]);
            for (int j = 0; j < 4; j++)
            {
                //need[i][j] = 0;
                //allocated[i][j] = maximum[i][j];
            }
        }
    }
    else
    {
        printf("Not safe\n");
    }
}

int issafe()
{

    int i, j = 0;
    int safe = 1;
    int num;
    while (i < rows && safe == 1)
    {
        while (j < cols && safe == 1)
        {
            num = 0;
            for (int a = i - 1; a > 0; a--)
            {
                num = num + allocated[i - a][j];
            }
            if (need[i][j] > (available[j] + num))
            {
                safe = 0;
            }
            j++;
        }
        i++;
    }

    return safe;
}