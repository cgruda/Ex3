/**
 * tasks.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * this module contains functions that are called from main.c
 * as well as my_atoi which is called also by factori.c
 * these function implement the main flow in portions.
 * also global defines and macros are contained in this module.
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
    printf("\nusage:\n\tfactori.exe [file-1] [file-2] [n] [m]\n\n"
           "\t[file1] - path to tasks file         \n"
           "\t[file2] - path to priorities file    \n"
           "\t[n]     - number of lines in file    \n"
           "\t[m]     - number of threads to use   \n\n");
}

//==============================================================================

void print_error(int err_val, char *err_msg)
{
    switch (err_val)
    {
        printf("Error: ");
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
    DBG_PRINT("test\n");

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
    {
        print_usage();
    }

    return ret_val;
}

//==============================================================================
// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:// TODO:
int fill_factori_queue(struct enviroment *p_env)
{
    /*
    FILE* file;
    Queue* q;
    Task* tsk;
    char buffer[MAX_BUFF];
    int i, cnt;
    errno_t err;
    q = InitializeQueue();

    if ((err = fopen_s(&file, path, "r+")) || file == NULL)
    {
        printf("File was not opened\n");
    }
    else
    {
        for (cnt = 0; cnt < length; ++cnt) {
            i = strtol(fgets(buffer, MAX_BUFF, file), NULL, 10);
            tsk = InitializeTask(i);
            Push(q, tsk);
        }
        return q;
    }
    */
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

    if (!(p_env->h_queue_mtx = CreateMutex(NULL, FALSE, NULL)))
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
    for (int i = 0; i < p_env->args.n_threads; p_env->p_h_threads[i++] = 0);

    // fill thread args struct
    p_env->thread_args.p_h_abort_evt = &p_env->h_abort_evt;
    p_env->thread_args.p_h_queue_mtx = &p_env->h_queue_mtx;
    p_env->thread_args.p_queue       = p_env->p_queue;
    p_env->thread_args.p_lock        = p_env->p_lock;
    p_env->thread_args.path          = p_env->args.path1;

    return OK;
}

//==============================================================================

int create_factori_threads(struct enviroment *p_env)
{
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
            PRINT_ERROR(E_WINAPI, 0);
            break;
        }

        p_env->threads_created++;
    }

    // if not all were created set abort event
    if (p_env->threads_created < p_env->args.n_threads)
    {
        if (!SetEvent(p_env->h_abort_evt))
        {
            PRINT_ERROR(E_WINAPI, 0);
            return ERR;
        }
    }

    return OK;
}

//==============================================================================

int wait_for_factori_threads(struct enviroment *p_env)
{
    int n_threads = p_env->threads_created;
    DWORD wait_code;
    DWORD exit_code;
    int status = ERR;

    // return ERR so that main will return ERR
    if (p_env->threads_created == 0)
        return ERR;

    // wait for created threads to end
    wait_code = WaitForMultipleObjects(n_threads, p_env->p_h_threads, TRUE, MAX_WAIT_TIME_ALL_MS);
    switch (wait_code)
    {
    case WAIT_OBJECT_0:
        status = OK;
        break;
    case WAIT_ABANDONED:
        // FALL THROUGH
    case WAIT_TIMEOUT:
        status = ERR;
        break;
    case WAIT_FAILED:
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    //threads must be done since already had abort event
    if (p_env->threads_created < p_env->args.n_threads)
        return ERR;

    // not all threads ended on time - try abort
    if (status == ERR)
    {
        if (!SetEvent(p_env->h_abort_evt))
        {
            PRINT_ERROR(E_WINAPI, 0);
            return ERR;
        }
        
        if (WaitForMultipleObjects(n_threads, p_env->p_h_threads, TRUE, 20) == WAIT_FAILED)
            PRINT_ERROR(E_WINAPI, 0);

        // return ERR in any case
        return ERR;
    }

    // all threads ended on time, get exit codes
    for (int i = 0; i < n_threads; ++i)
    {
        if(!GetExitCodeThread(p_env->p_h_threads[i], &exit_code))
        {
            PRINT_ERROR(E_WINAPI, 0);
            return ERR;
        }

        // thread failed, dont care about others
        if (exit_code != THREAD_STATUS_SUCCESS)
            return ERR;
    }

    return OK;
}

//==============================================================================

int cleanup_factori(struct enviroment *p_env)
{
    int status = OK;

    if (p_env->p_h_threads)
    {
        for (int i = 0; i < p_env->args.n_threads; ++i)
        {
            if (p_env->p_h_threads[i])
            {
                if (!CloseHandle(p_env->p_h_threads[i]))
                {
                    PRINT_ERROR(E_WINAPI, 0);
                    status = ERR;
                }
            }
        }

        free(p_env->p_h_threads);
    }

    if (!CloseHandle(p_env->h_abort_evt))
    {
        PRINT_ERROR(E_WINAPI, 0);
        status = ERR;
    }
    
    if (!CloseHandle(p_env->h_queue_mtx))
    {
        PRINT_ERROR(E_WINAPI, 0);
        status = ERR;
    }

    if (!DestroyQueue(p_env->p_queue))
    {
        status = ERR;
    }

    if (!DestroyLock(p_env->p_lock))
    {
        status = ERR;
    }

    return status;
}
