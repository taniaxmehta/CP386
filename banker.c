#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
pthread_mutex_t lock;
pthread_cond_t condition;
int rows,cols;
int **maximum;
int *safesequence;
int *resources;
int **need;
int **allocated;
//safety algorithm for RQ command
bool safealg();

int main(int argc, char *argv[]) {
    char filename = "sample_in_banker.txt";
    char buff[1000];
    char line[100];
    char *holder[1000];
    char *command;
    char * p;
    int customerno;
    //int available[] = {atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4])};
    int available[] = {10,5,7,8};
    FILE *fp;
    int ch, rows = 1,cols = 1;
    
    fp = fopen("sample_in_banker.txt","r");
    if (fp == NULL) {
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
    int resources[cols];
    int need[rows][cols];
    int allocated[rows][cols];
    int safesequence[rows];

    //populate maximumimum matrix
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fscanf(fp, "%d,", &maximum[i][j]);
        }
    }
    printf("Number of Customers: %d\n",rows);
    printf("Currently Available resources: %d %d %d %d\n",available[0],available[1],available[2],available[3]);
    printf("Maximum resources from file:\n");
    printmaximum(rows,cols,maximum);
    printf("Enter command: ");
    scanf("%s %d %d %d %d %d", &command,&customerno,&resources[0],&resources[1],&resources[2],&resources[3]);
    /*    fgets(line,100,stdin);
        p = strtok(line," ");
    while (p != NULL) {
        int i = 0;
        holder[i++] = p;
        p = strtok(NULL," ");
        p++;
    }
    for (int i = 0; i < 6; i++) {
        printf(holder[i]);
    }
    command = holder[0];
    customerno = atoi(holder[1]);
    resources[0] = atoi(holder[2]);
    resources[1] = atoi(holder[3]);
    resources[2] = atoi(holder[4]);
    resources[3] = atoi(holder[5]);
    */
    while (strcmp(command,"Exit") != 0) {
        
        if (strcmp(command,"RQ") == 0) {
            //request resources
            //fill up certain row with resources requested
            for (int i = 0; i < cols; i++) {
                allocated[customerno][i] = resources[i];
            }
            //calculate need matrix
            for (int i = 0; i < cols; i++) {
                for (int j = 0; j < cols; j++) {
                    need[i][j] = maximum[i][j] - allocated[i][j];
                }
            }
            for (int i = 0; i < cols; i++) {
                safesequence[i] = -1;
            }
            if (!safealg()) {
                printf("State is unsafe, and request is denied\n");
                exit(-1);
            }
            printf("State is safe, and request is satisfied\n");

        }
        else if (strcmp(command,"RL") == 0) {
            //release resources
            for (int i = 0; i < cols; i++) {
                allocated[customerno][i] = allocated[customerno][i] - resources[i];
            }
        }
        else if (strcmp(command, "Status") == 0) {
            //print all array and matrices
            printf("Available Resources:\n");
            //
            size_t n = sizeof(available) / sizeof(int);
            for (int i = 0; i < n; i++) {
                printf("%d ",available[i]);
            }
            printf("\n");
            printf("Maximum Resources:\n");
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    printf("%d ", maximum[i][j]);
                }   
            printf("\n");
            }
            printf("Allocated Resources:\n");
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    printf("%d ", allocated[i][j]);
                }   
            printf("\n");
            }
            printf("Need Resources:\n");
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    printf("%d ", need[i][j]);
                }   
            printf("\n");
            }
        }
        else if (strcmp(command, "Run") == 0) {
            //execute customers as threads
        }
        else if (strcmp(command, "Exit") == 0) {
            exit(0);
        }
        else {
            printf("Invalid input, use one of RQ, RL, Status, Run, Exit");
            
        }
        printf("Enter command: ");
        scanf("%s %d %d %d %d %d", &command,&customerno,&resources[0],&resources[1],&resources[2],&resources[3]);
        /*
        fgets(line,100,stdin);
        p = strtok(line," ");
        while (p != NULL) {
            int i = 0;
            holder[i++] = p;
            p = strtok(NULL," ");
        }
        command = holder[0];
        customerno = holder[1];
        resources[0] = holder[2];
        resources[1] = holder[3];
        resources[2] = holder[4];
        resources[3] = holder[5];*/
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
