#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
pthread_mutex_t lock;
pthread_cond_t condition;
int rows,cols;
int *safesequence;
int *resources;
int **need;
int **allocated;
//safety algorithm for RQ command
bool safealg();

int main() {
    char filename = "sample_in_banker.txt";
    char buff[1000];
    char command[10];
    int customerno;
    FILE *fp;
    int ch, rows = 1,cols = 1;
    int resources[cols];
    int safesequence[cols];
    fp = fopen("sample_in_banker.txt","r");
    if (fp == NULL) {
        printf("Cannot open file");
    }
    
    printf("Enter command: ");
    scanf("%s %d %d %d %d %d", &command,&customerno,&resources[0],&resources[1],&resources[2],&resources[3]);
    
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
    //count how many resources there are
    for (int i = 0; i < strlen(buff); i++)
    {
        if (buff[i] == ',')
        {
            cols++;
        }
    }
    fseek(fp,0,SEEK_SET);
    
    int maximum[rows][cols];
    int allocated[rows][cols];
    //populate maximumimum matrix
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fscanf(fp, "%d,", &maximum[i][j]);
        }
    }
    printmaximum(rows,cols,maximum);
    if (strcmp(command,"RQ")) {
        //request resources

    }
    else if (strcmp(command,"RL")) {
        //release resources
    }
    else if (strcmp(command, "Status")) {
        //print all array and matrices
    }
    else if (strcmp(command, "Run")) {
        //execute customers as threads
    }
    else if (strcmp(command, "Exit")) {
        exit(0);
    }
    else {
        printf("Invalid input, use one of RQ, RL, Status, Run, Exit");
        printf("Enter command: ");
        scanf("%s %d %d %d %d %d", &command,&customerno,&resources[0],&resources[1],&resources[2],&resources[3]);
    }
}

void printmaximum(int rows,int cols,int maximum[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }
}

bool safealg() {
    int temp[cols];
    for (int i = 0; i < cols; i++) {
        temp[i] = resources[i];
    }
    bool finished[rows];
    for (int i = 0; i < rows; i++) {
        finished[i] = false;
    }
    int nfinished = 0;
    while (nfinished < cols) {
        bool safe = false;
        for (int i = 0; i < cols; i++) {
            //if process is not finished then it might be safe
            if (finished[i] == false) {
                bool possible = true;
                for (int j = 0; j < cols; j++) {
                    //if amount of resources needed greater than present resources
                    //then it being safe is not possible
                    if (need[i][j] > temp[j]) {
                        possible = false;
                        break;
                    }
                }
                //if all needs are less than present resources
                if (possible == true) {
                    for (int j = 0; j < cols; j++) {
                        temp[j] += allocated[i][j];
                    }
                    safesequence[nfinished] = i;
                    finished[i] = true;
                    ++nfinished;
                    safe = true;
                }
            }
        }
        if (safe == false) {
            for (int j = 0; j < cols; j++) {
                safesequence[j] = -1;
            }
            return false; //no safe sequence of tasks is found
        }
    }
    return true; //safe sequence of tasks is found
}
