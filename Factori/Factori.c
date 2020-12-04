/**
 * Factori.c
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
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tasks.h"
#include "Queue.h"
#include "Factori.h"
#include "Lock.h"

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */

int wait_for_abort_evt(HANDLE h_abort_evt)
{
    int status;
    DWORD wait_code;

    wait_code = WaitForSingleObject(h_abort_evt, 0);
    switch (wait_code)
    {
    case WAIT_TIMEOUT:
        status = THREAD_STATUS_CONTINUE;
        break;
    case WAIT_OBJECT_0:
        status = THREAD_STATUS_ABORT;
        break;
    case WAIT_FAILED:
        PRINT_ERROR(E_WINAPI, 0);
        // FALL THROUGH
    case WAIT_ABANDONED:
        status = THREAD_STATUS_ERR;
        break;
    }

    return status;
}

int wait_for_queue_mtx(HANDLE h_queue_mtx)
{
    int status;
    DWORD wait_code;

    wait_code = WaitForSingleObject(h_queue_mtx, QUEUE_POP_MAX_WAIT_MS);
    switch (wait_code)
    {
    case WAIT_OBJECT_0:
        status = THREAD_STATUS_CONTINUE;
        break;
    case WAIT_FAILED:
        PRINT_ERROR(E_WINAPI, 0);
        // FALL THROUGH
    case WAIT_ABANDONED:
        // FALL THROUGH
    case WAIT_TIMEOUT:
        status = THREAD_STATUS_ERR;
        break;
    }

    return status;
}

int read_line_from_file(HANDLE h_file, int offset, char *buffer)
{
    char c;
    int nc = 0;
    int status;

    // set start position
    status = SetFilePointer(h_file, offset, NULL, FILE_BEGIN);
    if (status == INVALID_SET_FILE_POINTER)
    {
        PRINT_ERROR(E_WINAPI, 0);
        return THREAD_STATUS_ERR;
    }

    // read 1 char at a time until EOL squence
    while (nc < READ_BUFFER_LEN)
    {
        status = ReadFile(h_file, &c, 1, NULL, NULL);
        if(!status)
        {
            PRINT_ERROR(E_WINAPI, 0);
            return THREAD_STATUS_ERR;
        }

        if(c == '\n')
        {
            buffer[nc - 1] = '\0'; // remove trailing CR
            break;
        }
        else
        {
            buffer[nc] = c;
            nc++;
        }
    }

    if (nc == READ_BUFFER_LEN)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_BUF_FULL);
        return THREAD_STATUS_ERR;
    }

    // number of chars NOT including EOL sequence
    return THREAD_STATUS_CONTINUE;
}

DWORD WINAPI factori_thread(LPVOID param)
{
    HANDLE h_file             = NULL;
    HANDLE h_queue_mtx        = NULL;
    HANDLE h_abort_evt        = NULL;
    struct thread_args *args  = NULL;
    struct Queue *p_queue     = NULL;
    struct Task  *p_task      = NULL;
    struct Lock  *p_lock      = NULL;
    char *write_buffer        = NULL;
    char read_buffer[READ_BUFFER_LEN];
    int status    = THREAD_STATUS_CONTINUE;
    DWORD wait_code;

    args = (struct thread_args*)(param);
    h_queue_mtx = *(args->p_h_queue_mtx); // FIXME: make part of queue struct
    h_abort_evt = *(args->p_h_abort_evt);
    p_queue     =   args->p_queue;
    p_lock      =   args->p_lock;

    // open file for read and write
    h_file = CreateFileA(args->path,                         // File Name
                         GENERIC_READ    | GENERIC_WRITE,    // Desired Access
                         FILE_SHARE_READ | FILE_SHARE_WRITE, // Share Mode
                         NULL,                               // Security Attributes
                         OPEN_EXISTING,                      // Creation Disposition
                         FILE_ATTRIBUTE_NORMAL,              // Flags And Attributes
                         NULL);                              // Template File
    if (h_file == INVALID_HANDLE_VALUE)
    {
        PRINT_ERROR(E_WINAPI, 0);
        status = THREAD_STATUS_ERR;
    }

    // thread execution loop
    while(status == THREAD_STATUS_CONTINUE)
    {
        // check abort event
        status = wait_for_abort_evt(h_abort_evt);
        if (status != THREAD_STATUS_CONTINUE)
        {
            break;
        }

        // acquire exclusive pop access
        status = wait_for_queue_pop(h_queue_mtx);
        if (status != THREAD_STATUS_CONTINUE)
        {
            break;
        }

        // pop task from queue
        p_task = Pop(p_queue);

        // release pop queue mutex
        if (!ReleaseMutex(h_queue_mtx))
        {
            PRINT_ERROR(E_WINAPI, 0);
            status = THREAD_STATUS_ERR;
            break;
        }

        // if queue is empty, we are done
        if (!p_task)
        {
            status = THREAD_STATUS_SUCCESS;
            break;
        }

        // acquire read permmision
        if (read_lock(p_lock) != OK)
        {
            status = THREAD_STATUS_ERR;
            break;
        }

        // read line from file
        status = read_line_from_file(h_file, p_task->offset, &read_buffer);

        // release read
        if (read_release(p_lock) != OK)
        {
            status = THREAD_STATUS_ERR;
            break;
        }

        // check read was successful
        if (status != THREAD_STATUS_CONTINUE)
        {
            break;
        }

        // calc factori

        if (write_lock(p_lock))
        {
            status = THREAD_STATUS_ERR;
            break;
        }

        status = print_line_to_file(h_file, p_task->offset, &read_buffer);

        // release read
        if (write_release(p_lock) != OK)
        {
            status = THREAD_STATUS_ERR;
            break;
        }

        status = THREAD_STATUS_CONTINUE;
    }


    //     // write output file
    //     if (!WriteFile(h_outfile, buffer, length, NULL, NULL))
    //         break;

    //     rc = OK;

    // } while (0);

    // if (rc == ERR)
    //     printf("encode_decode_thread: WinAPI Error 0x%X\n", GetLastError());

    // // free thread resources
    // if (h_infile)
    //     if (!CloseHandle(h_infile))
    //         rc = ERR;
    // if (h_outfile)
    //     if (!CloseHandle(h_outfile))
    //         rc = ERR;
    // if (buffer)
    //     free(buffer);

    ExitThread((DWORD)0);
}
