#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define ROW 20
#define COLUMN 20

// thread locks
pthread_mutex_t lock_create_matrix;
pthread_mutex_t lock_sum_matrix;
pthread_mutex_t lock_main;
pthread_mutex_t lock_main_create;

// threads
pthread_t thread_matrix_creator;
pthread_t thread_matrix_sum;

int matrix[ROW][COLUMN]; // matrix
int counter = 0;    // counter for count steps
int number;         // step number

// print the 2 dimensional array
void print_matrix(int matrix[ROW][COLUMN]);

// check whether string is a number
// if it is number, return 1. Otherwise, return 0
int isNumber(char *str);

// ctrl+c handler
// if user enter ctrl+c, this function execute
void ctrl_c_handler(int dummy);

// ctrl+z handler
void ctrl_z_handler(int dummy);

// create 2d matrix
// it has random numbers in all iterations
void *create_matrix();

// sum elements of the created matrix
void *sum_matrix();

// terminate threads and main process
void terminateAll();

int main()
{
    // ctr+c and ctrl+z handlers
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTSTP, ctrl_z_handler);

    // initialize mutex
    pthread_mutex_init(&lock_main, NULL);
    pthread_mutex_init(&lock_create_matrix, NULL);
    pthread_mutex_init(&lock_sum_matrix, NULL);
    pthread_mutex_init(&lock_main_create, NULL);

    // lock lock_sum_matrix and loc_main_crate
    pthread_mutex_lock(&lock_sum_matrix);
    pthread_mutex_lock(&lock_main_create);

    // create threads
    pthread_create(&thread_matrix_creator, NULL, create_matrix, NULL);
    pthread_create(&thread_matrix_sum, NULL, sum_matrix, NULL);

    char input[100] = {'\0'};   // input string

    // infinite loop
    while (1)
    {
        pthread_mutex_lock(&lock_main); // lock main

        while (1)   // continue to be entered correct input
        {
            sleep(0.01);    // wait a few time
            //because after print operation is done, string is written to a buffer
            // and then unlock main. But, in main section, it also prints
            // and sometimes strings can not be printed in corret order
            // so we must wait a few time
            printf("\nplease write number: ");
            fgets(input, 100, stdin);   // want input from user

            if (input[0] == '\n')   // if input is enter character
            {
                continue;
            }

            input[strlen(input) - 1] = '\0';    // the last enter character assign to null character

            if (!isNumber(input))   // if input is not a number
            {
                printf("it is not a number\n");
                continue;
            }
            break;
        }

        number = atoi(input);   // convert string input to integer

        if (number <= 0)    // if number is zero or negative number
        {
            printf("\nentered zero or negative number\n");
            printf("exit program\n");
            terminateAll(); // termşnate all
            break;
        }

        printf("entered number: %d\n", number);
        counter = 0;    // reset counter
        pthread_mutex_unlock(&lock_main_create);    // unlock main create
    }

    return 0;
}

void *create_matrix()
{
    int i, j;
    while (1)   // infinite loop
    {
        pthread_mutex_lock(&lock_main_create);  // lock main crate
        while (counter < number)    // while counter is lower than number
        {
            pthread_mutex_lock(&lock_create_matrix);    // lock create matrix
            counter++;  // increase counter

            for (i = 0; i < ROW; i++)
            {
                for (j = 0; j < COLUMN; j++)
                {
                    matrix[i][j] = 1 + rand() % 99;     // random number assign to matrix
                }
            }

            printf("\ncreated matrix: %d\n", counter);
            print_matrix(matrix);   // print the created matrix

            pthread_mutex_unlock(&lock_sum_matrix); // unlock sum matrix
        }
        pthread_mutex_unlock(&lock_main);   // lock main
    }

    return NULL;
}

void *sum_matrix()
{
    int sum = 0;    // sum of matrix
    int i, j;
    while (1)   // infinite loop
    {
        pthread_mutex_lock(&lock_sum_matrix);   // lock sum matrix
        sum = 0;    // reset sum
        for (i = 0; i < ROW; i++)
        {
            for (j = 0; j < COLUMN; j++)
            {
                sum += matrix[i][j];    // sum matrix elements
            }
        }

        printf("\nmatrix: %d\tsum: %d\n", counter, sum);    // print sum of matrix
        pthread_mutex_unlock(&lock_create_matrix);          // unlock crate matrix
    }

    return NULL;
}

void ctrl_c_handler(int dummy)
{
    signal(SIGINT, ctrl_c_handler);
    printf("\nCTRL+C entered\n");
    printf("exit program\n");
    terminateAll();
}

void ctrl_z_handler(int dummy)
{
    signal(SIGTSTP, ctrl_z_handler);
    printf("\nCTRL+Z entered\n");
    printf("exit program\n");
    terminateAll();
}

void print_matrix(int matrix[20][20])
{
    int i, j;
    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COLUMN; j++)
        {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void terminateAll()
{
    // destroy locks
    pthread_mutex_destroy(&lock_create_matrix);
    pthread_mutex_destroy(&lock_sum_matrix);
    pthread_mutex_destroy(&lock_main);
    pthread_mutex_destroy(&lock_main_create);
    // kill threads
    pthread_kill(thread_matrix_creator, SIGKILL);
    pthread_kill(thread_matrix_sum, SIGKILL);
    exit(0);    // exit program
}

int isNumber(char *str)
{
    int i = 0;
    while (*str != '\0')
    {
        if (i == 0)
        {
            if (str[0] == '-')
            {
                str++;
            }
            i++;
        }
        if (*str < '0' || *str > '9')
        {
            return 0; // false
        }
        str++;
    }

    return 1; // true
}