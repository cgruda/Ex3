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
#include "Lock.h"

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */
    // argv[0] - "Factori.exe"
    // argv[1] - "tasks input file - and also output file"
    // argv[2] - "priority input file"
    // argv[3] - "num" (number of lines in files)
    // argv[4] - "num" (number of threads)
void print_usage()
{
    printf("\nusage:\n\tfactori.exe [file-1] [file-2] [n] [m]\n\n"
           "\t[file-1] - path to tasks file         \n"
           "\t[file-2] - path to priorities file    \n"
           "\t[n]      - number of lines in file    \n"
           "\t[m]      - number of threads to use   \n\n");
}

void print_error(int err_val, char *err_msg)
{
    switch (err_val)
    {
        case E_STDLIB:
            printf("%s\n", strerror(errno));
            break;
        case E_WINAPI:
            printf("WinAPI Error 0x%X\n", GetLastError());
            break;
        case E_INTERNAL:
            if (err_msg)
            {
                printf("Error: %s\n", err_msg);
                break;
            }
        default:
            printf("Unknown Error\n");
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

int init(struct args* args, int argc, char** argv)
{
    int ret_val = OK;

    // check number of arguments
    if (argc != ARGC)
    {
        print_usage();
        return ERR;
    }

    memset(args, 0, sizeof(*args));

    // input file 1 path check
    args->path1 = argv[1];
    if (!PathFileExistsA(args->path1))
    {
        printf("\n%s not found (WinAPI Error 0x%X)\n\n", args->path1, GetLastError());
        ret_val = ERR;
    }

    // input file 2 path check
    args->path2 = argv[2];
    if (!PathFileExistsA(args->path2))
    {
        printf("\n%s not found (WinAPI Error 0x%X)\n\n", args->path2, GetLastError());
        ret_val = ERR;
    }
    
    // check n_lines
    if (!my_atoi(argv[3], &args->n_lines))
    {
        printf("\ninvalid input. [n] must be an integer.\n");
        ret_val = ERR;
    }

    // check n_threads
    int rc = my_atoi(argv[4], &args->n_threads);
    if ((rc == ERR) || (args->n_threads < 1) || (args->n_threads > MAXIMUM_WAIT_OBJECTS))
    {
        printf("\ninvalid inupt, [n] must satisfy: MAXIMUM_WAIT_OBJECTS >= n > 0.\n");
        printf("MAXIMUM_WAIT_OBJECTS = %d\n", MAXIMUM_WAIT_OBJECTS);
        ret_val = ERR;
    }

    if (ret_val != OK)
    {
        print_usage();
    }

    return ret_val;
}


struct Queue *fill_queue(char *path)
{
    
}

int* factori(int num) {
    int i;
    int pos = 0;
    int* ptr, * new_ptr;
    ptr = (int*)malloc(1 * sizeof(int)); // FIXME: error handling, use calloc

    while (num % 2 == 0) { //num is even
        *(ptr + pos) = 2;
        num = num / 2;
        pos++;
        new_ptr = (int*)realloc(ptr, (pos+1)*sizeof(int)); // FIXME: error handling
        ptr = new_ptr; // FIXME: mem leak (must free ptr before re-assigning)
    }
    for (i = 3; i < (num / 2); i = i + 2) { //num is odd
        while (num % i == 0) {
            *(ptr + pos) = i;
            num = num / i;
            pos++;
            new_ptr = (int*)realloc(ptr, (pos+1)*sizeof(int)); // FIXME: error handling
            ptr = new_ptr;// FIXME: mem leak (must free ptr before re-assigning)
        }
    }
    if (num > 2) { // num remaind is prime
        *(ptr + pos) = num;
        num = 1;
        pos++;
        new_ptr = (int*)realloc(ptr, (pos+1)*sizeof(int)); // FIXME: error handling
        ptr = new_ptr;// FIXME: mem leak (must free ptr before re-assigning)
    }
    *(ptr+pos) = 0; // null terminated array
    return ptr;
}

int create_n_threads(LPTHREAD_START_ROUTINE thread_func, HANDLE *p_h_threads,
    int n_threads, LPVOID args)
{
    int threads_created = 0;

    for (int i = 0; i < n_threads; ++i)
    {
        p_h_threads[i] = CreateThread(NULL,
                                      0,
                                      thread_func,
                                      args,
                                      0,
                                      NULL);
        ASSERT_RETURN_VAL(p_h_threads[i], ERR);

        threads_created++;
    }

    return threads_created;
}


int wait_for_n_threads(HANDLE* p_h_threads, int n_threads)
{
    DWORD wait_code;
    DWORD exit_code;
    int rc;

    // trivial case
    if (n_threads == 0)
    {
        return OK;
    }

    // wait for all threads to end
    wait_code = WaitForMultipleObjects(n_threads, p_h_threads, TRUE, MAX_WAIT_TIME_ALL_MS);
    ASSERT_RETURN_VAL((wait_code != WAIT_FAILED), ERR);

    // not all ended on time
    if (wait_code != WAIT_OBJECT_0)
    {
        // set abort event

        // wait some more

        // if not all done then error for exit
    }

    // all threads ended on time, get exit codes
    for (int i = 0; i < n_threads; ++i)
    {
        rc = GetExitCodeThread(p_h_threads[i], &exit_code);
        ASSERT_RETURN_VAL(rc, ERR);

        if (exit_code == ERR)
        {
            // thread failed, dont care about others
            return ERR;
        }
    }

    return OK;
}