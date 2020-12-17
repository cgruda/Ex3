/**
 * Factori.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * This module implements the Factori thread
 * and all its subroutines
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"
#include "Factori.h"
#include "Lock.h"
#include "tasks.h"

/*
 ==============================================================================
 * FUNCTION DEFENITIONS
 ==============================================================================
 */
int create_file_handle(HANDLE *h_file, char *path)
{
    *h_file = CreateFileA(path,                               // File Name
                          GENERIC_READ    | GENERIC_WRITE,    // Desired Access
                          FILE_SHARE_READ | FILE_SHARE_WRITE, // Share Mode
                          NULL,                               // Security Attributes
                          OPEN_EXISTING,                      // Creation Disposition
                          FILE_ATTRIBUTE_NORMAL,              // Flags And Attributes
                          NULL);                              // Template File
    if (h_file == INVALID_HANDLE_VALUE)
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        return ERR;
    }

    return OK;
}

//==============================================================================

int check_abort_evt(HANDLE h_abort_evt)
{
    int status = THREAD_STATUS_CONTINUE;
    DWORD wait_code;

    // wait 0 ms to check if need to abort
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
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        // FALL THROUGH
    case WAIT_ABANDONED:
        status = THREAD_STATUS_ERR;
        break;
    }

    return status;
}

//==============================================================================

int wait_for_queue_mtx(HANDLE h_queue_mtx)
{
    int status = ERR;
    DWORD wait_code;

    wait_code = WaitForSingleObject(h_queue_mtx, QUEUE_POP_MAX_WAIT_MS);
    switch (wait_code)
    {
    case WAIT_OBJECT_0:
        status = OK;
        break;
    case WAIT_FAILED:
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        // FALL THROUGH
    case WAIT_ABANDONED:
        // FALL THROUGH
    case WAIT_TIMEOUT:
        status = ERR;
        break;
    }

    return status;
}

//==============================================================================

int release_queue_mtx(HANDLE h_queue_mtx)
{
    if (!ReleaseMutex(h_queue_mtx))
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        return ERR;
    }

    return OK;
}

//==============================================================================

int read_number_from_file(HANDLE h_file, struct Task **p_task, int *number)
{
    char c;
    int status = OK;
    *number = 0;

    // save task offset and free task
    int offset = (*p_task)->offset;
    free(*p_task);
    *p_task = NULL;

    // set start position
    if (SetFilePointer(h_file, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        return ERR;
    }

    while (1)
    {
        // read 1 char at a time
        if(!ReadFile(h_file, &c, 1, NULL, NULL))
        {
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
            status = ERR;
            break;
        }

        // CR indicates end of line
        if (c == CR)
            break;

        // check for validity
        if(!isdigit(c))
        {
            PRINT_ERROR(E_INTERNAL, E_MSG_INPT_ERR);
            status = ERR;
            break;
        }

        *number *= 10;
        *number += (c - '0');

        // integer overflow
        if (*number < 0)
        {
            PRINT_ERROR(E_INTERNAL, E_MSG_INPT_ERR);
            status = ERR;
            break;
        }
    }

    return status;
}

//==============================================================================

int *factori(int num) 
{
    int  i, pos = 0;
    int *ptr, *new_ptr;

    if (num == 0)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_INPT_ERR);
        return NULL;
    }

    ptr = (int*)calloc(1, sizeof(*ptr));
    if (!ptr)
    {
        PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
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
            PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
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
                PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
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
        pos++;
        new_ptr = (int*)realloc(ptr, (pos + 1) * sizeof(int));
        if (!new_ptr)
        {
            PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
            free(ptr);
            return NULL;
        }
        ptr = new_ptr;
    }
    
    // Null terminated array
    *(ptr + pos) = 0;

    return ptr;
}

//==============================================================================

int num_strlen(int num)
{
    int count = 1;

    while(num /= 10)
        count++;

    return count;
}

//==============================================================================

int generate_output_string(int num, int *factori_arr, char **write_buffer)
{
    size_t buff_len;
    char *temp_buff;
    char *buff;

    // sanity
    if (!factori_arr)
        return ERR;

    // allocate first chunk
    buff_len = 27 + num_strlen(num) + 3;
    buff     = (char*)calloc(buff_len, sizeof(*buff));
    if (!buff)
    {
        PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
        return ERR;
    }

    // print first chunk into buffer
    sprintf_s(buff, buff_len, "The prime factors of %d are: ", num);

    // empty group, allocate mem for EOL
    if (!factori_arr[0])
    {
        buff_len += 2;
        temp_buff = (char*)realloc(buff, buff_len);
        if (!temp_buff)
        {
            PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
            free(buff);
            return ERR;
        }
        sprintf_s(temp_buff + strlen(temp_buff),
                  buff_len  - strlen(temp_buff),
                  "  ");
        buff = temp_buff;
    }

    // print factori array results into buffer
    for (int i = 0; factori_arr[i]; i++)
    {
        buff_len += num_strlen(factori_arr[i]) + 2;
        temp_buff = (char*)realloc(buff, buff_len);
        if (!temp_buff)
        {
            PRINT_ERROR(E_STDLIB, E_MSG_NULL_MSG);
            free(buff);
            return ERR;
        }
        sprintf_s(temp_buff + strlen(temp_buff),
                  buff_len  - strlen(temp_buff),
                  "%d, ", factori_arr[i]);
        buff = temp_buff;
    }

    // EOL sequence overwite
    buff[strlen(buff) - 2] = CR;
    buff[strlen(buff) - 1] = LF;

    *write_buffer = buff;
    return OK;
}

//==============================================================================

int print_line_to_file(HANDLE *h_file, char **write_buffer)
{
    int num_bytes = (int)strlen(*write_buffer);

    // seek end of file
    if (SetFilePointer(h_file, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        return ERR;
    }

    // write output
    if (!WriteFile(h_file, *write_buffer, num_bytes, NULL, NULL))
    {
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
        return ERR;
    }

    free(*write_buffer);
    *write_buffer = NULL;
    return OK;
}

//==============================================================================

DWORD WINAPI factori_thread(LPVOID param)
{
    struct thread_args *args = (struct thread_args*)(param);
    struct Task *p_task = NULL;
    char *write_buffer  = NULL;
    int *factori_arr    = NULL;
    HANDLE h_file       = NULL;
    int number, status  = THREAD_STATUS_CONTINUE;

    // open file for read and write
    status = create_file_handle(&h_file, args->path);

    // thread execution loop
    while(status == THREAD_STATUS_CONTINUE)
    {
        // check if main thread triggerd abort event
        status = check_abort_evt(*(args->p_h_abort_evt));
        CHECK_STATUS();

        // acquire exclusive pop access
        status = wait_for_queue_mtx(*(args->p_h_queue_mtx));
        CHECK_STATUS();

        // pop task from queue
        p_task = Pop(args->p_queue);

        // release queue mutex
        status = release_queue_mtx(*(args->p_h_queue_mtx));
        CHECK_STATUS();

        // if queue is empty we are done
        if (!p_task)
        {
            status = THREAD_STATUS_SUCCESS;
            break;
        }

        // acquire read access
        status = read_lock(args->p_lock);
        CHECK_STATUS();

        // read line from file
        status = read_number_from_file(h_file, &p_task, &number);

        // release read
        if (read_release(args->p_lock) != OK)
            status = THREAD_STATUS_ERR;

        // check read_line & read_release
        CHECK_STATUS();

        // calc factori and generate string
        factori_arr = factori(number);
        status = generate_output_string(number, factori_arr, &write_buffer);
        CHECK_STATUS();

        // acquire exclusive write access
        status = write_lock(args->p_lock);
        CHECK_STATUS();

        // write output (and free write buffer)
        status = print_line_to_file(h_file, &write_buffer);

        // release write lock
        if (write_release(args->p_lock) != OK)
            status = THREAD_STATUS_ERR;
    }

    // in case of error abort all threads
    if (status == THREAD_STATUS_ERR)
        if (!SetEvent(*(args->p_h_abort_evt)))
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);

    // thread cleanup
    if (h_file)
    {
        if (!CloseHandle(h_file))
        {
            PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);
            status = THREAD_STATUS_ERR;
        }
    }
    if (write_buffer)
        free(write_buffer);
    if (p_task)
        free(p_task);

    ExitThread((DWORD)status);
}
