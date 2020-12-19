/**
 * tasks.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * this module contains functions that are called from main.c
 * these function implement the main flow.
 * 
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include <windows.h>
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

void print_usage()
{
    printf("\nusage:\n\tfactori.exe [path1] [path2] [n] [m]\n\n"
           "\t[path1] - path to tasks file         \n"
           "\t[path2] - path to priorities file    \n"
           "\t[n]     - number of lines in file    \n"
           "\t[m]     - number of threads to use   \n\n");
}

//==============================================================================

void print_error(int err_val, char *err_msg)
{
    printf("Error: ");
    switch (err_val)
    {
        case E_STDLIB:
            printf("errno = %d\n", errno);
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

    // input file 1 path check
    args->path1 = argv[1];
    if (!PathFileExistsA(args->path1))
    {
        printf("\n%s not found (WinAPI Error 0x%X)", args->path1, GetLastError());
        ret_val = ERR;
    }

    // input file 2 path check
    args->path2 = argv[2];
    if (!PathFileExistsA(args->path2))
    {
        printf("\n%s not found (WinAPI Error 0x%X)", args->path2, GetLastError());
        ret_val = ERR;
    }
    
    // check n_lines
    if (!my_atoi(argv[3], &args->n_lines))
    {
        printf("\ninvalid input: [n] must be an integer.");
        ret_val = ERR;
    }

    // check n_threads
    int rc = my_atoi(argv[4], &args->n_threads);
    if ((rc == ERR) || (args->n_threads < 1) || (args->n_threads > MAXIMUM_WAIT_OBJECTS))
    {
        printf("\ninvalid inupt: [m] must satisfy: %d > [m] > 0.\n", MAXIMUM_WAIT_OBJECTS + 1);
        ret_val = ERR;
    }

    if (ret_val != OK)
        print_usage();

    return ret_val;
}

//==============================================================================

int init_factori(struct enviroment *p_env)
{
    int ret_val = OK;

    // init readers-writer lock
    if (!(p_env->p_lock = InitializeLock()))
        ret_val = ERR;

    // init tasks queue
    if (!(p_env->p_queue = InitializeQueue()))
        ret_val = ERR;

    // init queue mutex
    if (!(p_env->h_queue_mtx = CreateMutex(NULL, FALSE, NULL)))
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        ret_val = ERR;
    }

    // init abort event
    if (!(p_env->h_abort_evt = CreateEvent(NULL, TRUE, FALSE, NULL)))
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        ret_val = ERR;
    }

    // allocate mem for thread handles
    p_env->p_h_threads = (HANDLE*)calloc(p_env->args.n_threads, sizeof(HANDLE));
    if (!p_env->p_h_threads)
    {
        PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
        ret_val = ERR;
    }

    // fill thread args struct
    p_env->thread_args.p_h_abort_evt = &p_env->h_abort_evt;
    p_env->thread_args.p_h_queue_mtx = &p_env->h_queue_mtx;
    p_env->thread_args.p_queue       = p_env->p_queue;
    p_env->thread_args.p_lock        = p_env->p_lock;
    p_env->thread_args.path          = p_env->args.path1;

    return ret_val;
}

//==============================================================================

int fill_factori_queue(struct enviroment *p_env)
{
    FILE *fp              = NULL;
    struct Task  *p_task  = NULL;
    struct Queue *p_queue = NULL;
    int offset_val;
    int ret_val = OK;

    if (fopen_s(&fp, p_env->args.path2, "r"))
    {
        PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
        return ERR;
    }

    if (!fp)
    {
        PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
        return ERR;
    }

    p_queue = p_env->p_queue;

    for (int i = 0; i < p_env->args.n_lines; ++i)
    {
        fscanf_s(fp, "%d\n", &offset_val);

        p_task = (struct Task*)calloc(1, sizeof(*p_task));
        if (!p_task)
        {
            PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
            ret_val = ERR;
            break;
        }

        p_task->offset = offset_val;
        p_task->next = NULL;

        if (!Push(p_queue, p_task))
        {
            ret_val = ERR;
            break;
        }
    }

    fclose(fp);
    return ret_val;
}

//==============================================================================

int create_factori_threads(struct enviroment *p_env)
{
    int status = OK;
    p_env->threads_created = 0;

    for (int i = 0; i < p_env->args.n_threads; ++i)
    {
        p_env->p_h_threads[i] = CreateThread(NULL,
                                             0,
                                             factori_thread,
                                             (LPVOID)&p_env->thread_args,
                                             0,
                                             NULL);
        if (!p_env->p_h_threads[i])
        {
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
            status = ERR;
            break;
        }

        p_env->threads_created++;
    }

    // case not all threads were created -> set abort event
    if (status == ERR)
        if (!SetEvent(p_env->h_abort_evt))
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG); // status is ERR anyway

    // return OK since must get to wait function
    return OK;
}

//==============================================================================

int wait_for_factori_threads(struct enviroment *p_env)
{
    int threads_created = p_env->threads_created;
    DWORD wait_code;
    DWORD exit_code;
    int ret_val = OK;

    // return ERR so that main will return ERR
    if (threads_created == 0)
        return ERR;

    // wait for threads to end
    wait_code = WaitForMultipleObjects(threads_created, p_env->p_h_threads, TRUE, MAX_WAIT_TIME_ALL_MS);
    switch (wait_code)
    {
    case WAIT_OBJECT_0:
        // all threads ended on time, get exit codes
        for (int i = 0; i < threads_created; ++i)
        {
            if (!GetExitCodeThread(p_env->p_h_threads[i], &exit_code))
            {
                PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
                ret_val = ERR;
                break;
            }

            // thread failed - result is ERR (dont care about other threads)
            if (exit_code != THREAD_STATUS_SUCCESS)
            {
                ret_val = ERR;
                break;
            }
        }
        break;
    case WAIT_FAILED:
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        ret_val = ERR;
        break;
    case WAIT_ABANDONED:
        // FALL THROUGH
    case WAIT_TIMEOUT:
        // not all threads ended -> abort
        ret_val = ERR;

        if (!SetEvent(p_env->h_abort_evt))
        {
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
            break; // event not set -> no purpose in waiting again
        }

        // wait 20 ms after abort event 
        // wait result does not really matter since we are in ERR state anyway (so only notify on fail)
        if (WaitForMultipleObjects(threads_created, p_env->p_h_threads, TRUE, 20) == WAIT_FAILED)
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG); 

        break;
    }

    return ret_val;
}

//==============================================================================

int cleanup_factori(struct enviroment *p_env)
{
    int status = OK;

    if (p_env->p_h_threads)
    {
        for (int i = 0; i < p_env->threads_created; ++i)
        {
            if (!CloseHandle(p_env->p_h_threads[i]))
            {
                PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
                status = ERR;
            }
        }

        free(p_env->p_h_threads);
    }

    if (!CloseHandle(p_env->h_abort_evt))
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        status = ERR;
    }
    
    if (!CloseHandle(p_env->h_queue_mtx))
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        status = ERR;
    }

    if (!DestroyQueue(&p_env->p_queue))
        status = ERR;

    if (!DestroyLock(&p_env->p_lock))
        status = ERR;

    return status;
}
