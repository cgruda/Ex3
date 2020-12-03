/**
 * tasks.c
 * Factori Project
 * ISP_HW_3_2020
 *
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "tasks.h"

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */

void print_usage()
{

}

void print_error()
{
    if (errno)
    {
        printf("%s\n", strerror(errno));
    }
    else if (GetLastError())
    {
        printf("WinAPI Error 0x%X\n", GetLastError());
    }
    else
    {
        printf("Unknown Error\n", GetLastError());
    }
}

int my_atoi(char *str, int *p_result)
{
    if (!str || !p_result)
        return ERR;

    for (int i = 0; i < (int)strlen(str); ++i)
    {
        if (!isdigit(str[i]))
        {
            if (i == 0)
            {
                if ((str[i] == '-') || (str[i] == '+'))
                    continue;
            }

            return ERR;
        }
    }

    *p_result = strtol(str, NULL, 10);

    if (errno == ERANGE)
        return ERR;

    return OK;
}

struct Queue *fill_queue(char *path)
{
    
}

int* factori(int num) {
    int i;
    int pos = 0;
    int* ptr, * new_ptr;
    ptr = (int*)malloc(1 * sizeof(int));

    while (num % 2 == 0) { //num is even
        *(ptr + pos) = 2;
        num = num / 2;
        pos++;
        new_ptr = (int*)realloc(ptr, (pos+1)*sizeof(int));
        ptr = new_ptr;
    }
    for (i = 3; i < (num / 2); i = i + 2) { //num is odd
        while (num % i == 0) {
            *(ptr + pos) = i;
            num = num / i;
            pos++;
            new_ptr = (int*)realloc(ptr, (pos+1)*sizeof(int));
            ptr = new_ptr;
        }
    }
    if (num > 2) { // num remaind is prime
        *(ptr + pos) = num;
        num = 1;
        pos++;
        new_ptr = (int*)realloc(ptr, (pos+1)*sizeof(int));
        ptr = new_ptr;
    }
    *(ptr+pos) = 0; // null terminated array
    return ptr;
}