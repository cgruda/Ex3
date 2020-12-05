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
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "tasks.h"
#include "Lock.h"
#include "Factori.h"

/*
 ==============================================================================
 * FUNCTION DEFENITIONS
 ==============================================================================
 */

void print_usage()
{
    printf("\nusage:\n\tfactori.exe [file-1] [file-2] [n] [m]\n\n"
           "\t[file-1] - path to tasks file         \n"
           "\t[file-2] - path to priorities file    \n"
           "\t[n]      - number of lines in file    \n"
           "\t[m]      - number of threads to use   \n\n");
}

//==============================================================================

void print_error(int err_val, char *err_msg)
{
    switch (err_val)
    {
        printf("Error: ");
        case E_STDLIB:
            printf("%s\n", strerror(errno));
            break;
        case E_WINAPI:
            printf("WinAPI Error 0x%X\n", GetLastError());
            break;
        case E_INTERNAL:
            if (err_msg)
            {
                printf("%s\n", err_msg);
                break;
            }
        default:
            printf("Unknown Error\n");
    }
}

//==============================================================================

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

            *p_result = 0;
            return ERR;
        }
    }

    *p_result = strtol(str, NULL, 10);

    if (errno == ERANGE)
    {
        *p_result = 0;
        return ERR;
    }

    return OK;
}

//==============================================================================

int check_input(struct enviroment *env, int argc, char** argv)
{
    struct args *args = &env->args;
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
        printf("\ninvalid input. [n] must be an integer.\n\n");
        ret_val = ERR;
    }

    // check n_threads
    int rc = my_atoi(argv[4], &args->n_threads);
    if ((rc == ERR) || (args->n_threads < 1) || (args->n_threads > MAXIMUM_WAIT_OBJECTS))
    {
        printf("\ninvalid inupt, [n] must satisfy: MAXIMUM_WAIT_OBJECTS >= n > 0.\n");
        printf("\t(MAXIMUM_WAIT_OBJECTS = %d)\n\n", MAXIMUM_WAIT_OBJECTS);
        ret_val = ERR;
    }

    if (ret_val != OK)
    {
        print_usage();
    }

    return ret_val;
}

//==============================================================================
// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:
struct Queue *fill_queue(char *path)
{
    
}

//==============================================================================
// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:
int *factori(int num) 
{
    int  i, pos = 0;
    int *ptr, *new_ptr;

    // edge cases // FIXME:
    if (num == 0)
    {
        return NULL;
    }

    ptr = (int*)calloc(1, sizeof(*ptr));
    if (!ptr)
    {
        PRINT_ERROR(E_STDLIB, 0);
        return NULL;
    }

    // num is even
    while (num % 2 == 0)
    {
        *(ptr + pos) = 2;
        num /= 2;
        pos++;
        new_ptr = (int*)realloc(ptr, (pos + 1) * sizeof(int));
        if (!new_ptr)
        {
            PRINT_ERROR(E_STDLIB, 0);
            free(ptr);
            return NULL;
        }
        ptr = new_ptr;
    }

    //num is odd
    for (i = 3; i < (num / 2); i += 2)
    {
        while (num % i == 0)
        {
            *(ptr + pos) = i;
            num /= i;
            pos++;
            new_ptr = (int*)realloc(ptr, (pos + 1) * sizeof(int));
            if (!new_ptr)
            {
                PRINT_ERROR(E_STDLIB, 0);
                free(ptr);
                return NULL;
            }
            ptr = new_ptr;
        }
    }

    // num remaind is prime
    if (num > 2)
    {
        *(ptr + pos) = num;
        num = 1;
        pos++;
        new_ptr = (int*)realloc(ptr, (pos + 1) * sizeof(int));
        if (!new_ptr)
        {
            PRINT_ERROR(E_STDLIB, 0);
            free(ptr);
            return NULL;
        }
        ptr = new_ptr;
    }
    
    // Null terminated array
    *(ptr+pos) = 0;

    return ptr;
}

//==============================================================================

int create_factori_threads(struct locale_s *locale, struct args_s *args)
{// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:
    thread_args.path    = main_args.path1;
        thread_args.p_lock  = p_lock;
        thread_args.p_queue = p_queue;
        thread_args.p_h_abort_evt = create_abort_evt();
        if (!thread_args.p_h_abort_evt)
        {
            status = ERR;
            break;
        }
    
    int threads_created = 0;
    HANDLE *p_h_threads             = locale->p_h_threads;
    struct thread_args *thread_args = &locale->thread_args;

    // create abort event
    thread_args->p_h_abort_evt = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!thread_args->p_h_abort_evt)
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    // allocate thread handles
    p_h_threads = calloc(args->n_threads, sizeof(HANDLE));
    if (!p_h_threads)
        return ERR;



    for (int i = 0; i < args->n_threads; ++i)
    {
        p_h_threads[i] = CreateThread(NULL,
                                      0,
                                      factori_thread,
                                      thread_args,
                                      0,
                                      NULL);
        if (!p_h_threads[i])
        {
            PRINT_ERROR(E_WINAPI, 0);
            break;
        }

        threads_created++;
    }

    return threads_created;
}

//==============================================================================
// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:
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
    // wait

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

//==============================================================================

int init_factori(struct enviroment *p_env)
{
    struct Queue *p_queue = p_env->p_queue;
    struct Lock  *p_lock  = p_env->p_lock;

    // init read-write lock
    if (!(p_env->p_lock = InitializeLock()))
        return ERR;

    // init tasks queue
    if (!(p_env->p_queue = InitializeQueue()))
        return ERR;

    // init abort event
    if (!(p_env->h_abort_evt = CreateEvent(NULL, FALSE, FALSE, NULL)))
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    // allocate mem for thread handles
    p_env->p_h_threads = calloc(p_env->args.n_threads, sizeof(HANDLE));
    if (!p_env->p_h_threads)
    {
        PRINT_ERROR(E_STDLIB, 0);
        return ERR;
    }

    // fill thread args struct
    p_env->thread_args.p_h_abort_evt = p_env->h_abort_evt;
    p_env->thread_args.p_queue       = p_env->p_queue;
    p_env->thread_args.p_lock        = p_env->p_lock;
    p_env->thread_args.path          = p_env->args.path1;

    return OK;
}