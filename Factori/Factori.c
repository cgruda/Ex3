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

#include "Queue.h"
#include "Factori.h"
#include "Lock.h"

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */

int check_abort_evt(HANDLE h_abort_evt)
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
        status = OK;
        break;
    case WAIT_FAILED:
        PRINT_ERROR(E_WINAPI, 0);
        // FALL THROUGH
    case WAIT_ABANDONED:
        // FALL THROUGH
    case WAIT_TIMEOUT:
        status = ERR;
        break;
    }

    return status;
}

int release_queue_mtx(HANDLE h_queue_mtx)
{
    if (!ReleaseMutex(h_queue_mtx))
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    return OK;
}

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
        PRINT_ERROR(E_WINAPI, 0);
        return THREAD_STATUS_ERR;
    }

    return THREAD_STATUS_CONTINUE;
}

int read_line_from_file(HANDLE h_file, int offset, char *buffer)
{
    char c;
    int nc = 0;

    // set start position
    if (SetFilePointer(h_file, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        PRINT_ERROR(E_WINAPI, 0);
        return THREAD_STATUS_ERR;
    }

    // read 1 char at a time until EOL squence
    while (nc < READ_BUFFER_LEN)
    {
        if(!ReadFile(h_file, &c, 1, NULL, NULL))
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

int print_line_to_file(HANDLE *h_file, char *write_buffer)
{
    int num_bytes = (int)strlen(write_buffer);

    // seek end of file
    if (SetFilePointer(h_file, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    // write output
    if (!WriteFile(h_file, write_buffer, num_bytes, NULL, NULL))
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    free(write_buffer);
    write_buffer = NULL;
    return OK;
}

int num_strlen(int num)
{
    int count = 1;

    while(num /= 10)
    {
        count++;
    }

    return count;
}

int generate_output_string(int num, int *factori_arr, char **write_buffer)
{
    size_t buff_len;
    char *temp_buff;
    char *buff;

    // sanity
    if (!factori_arr)
        return ERR;

    // allocate first chunk
    buff_len = 27 + num_strlen(num) + 3; // FIXME:
    buff     = calloc(buff_len, sizeof(*buff));
    if (!buff)
    {
        PRINT_ERROR(E_STDLIB, 0);
        return ERR;
    }

    // print first chunk into buffer
    sprintf_s(buff, buff_len, "The prime factors of %d are: ", num);

    // print factori array results into buffer
    for (int i = 0; factori_arr[i]; i++)
    {
        buff_len += num_strlen(factori_arr[i]) + 2;
        temp_buff = realloc(buff, buff_len);
        if (!temp_buff)
        {
            PRINT_ERROR(E_STDLIB, 0);
            free(buff); // FIXME:
            return ERR;
        }
        sprintf_s(temp_buff + strlen(temp_buff),
                  buff_len  - strlen(temp_buff),
                  "%d, ", factori_arr[i]);
        buff = temp_buff;
    }

    // EOL sequence
    buff[strlen(buff) - 2] = CR;
    buff[strlen(buff) - 1] = LF;

    *write_buffer = buff;
    return OK;
}

DWORD WINAPI factori_thread(LPVOID param)
{
    struct thread_args *args = (struct thread_args*)(param);
    struct Queue *p_queue    = args->p_queue;
    struct Lock  *p_lock     = args->p_lock;
    struct Task  *p_task     = NULL;
    HANDLE h_file            = NULL;
    char *write_buffer       = NULL;
    char read_buffer[READ_BUFFER_LEN];
    int status = THREAD_STATUS_CONTINUE;
    int number, *factori_arr;

    // open file for read and write
    status = create_file_handle(&h_file, args->path);

    // thread execution loop
    while(status == THREAD_STATUS_CONTINUE)
    {
        // check if main thread triggerd abort event
        status = check_abort_evt(*(args->p_h_abort_evt));
        CHECK_STATUS();

        // acquire exclusive pop access
        status = wait_for_queue_mtx(*(p_queue->p_h_queue_mtx));
        CHECK_STATUS();

        // pop task from queue
        p_task = Pop(p_queue);

        // release queue mutex
        status = release_queue_mtx(*(p_queue->p_h_queue_mtx));
        CHECK_STATUS();

        // if queue is empty, we are done
        if (!p_task)
        {
            status = THREAD_STATUS_SUCCESS;
            break;
        }

        // acquire read access
        status = read_lock(p_lock);
        CHECK_STATUS();

        // read line from file
        status = read_line_from_file(h_file, p_task->offset, &read_buffer);

        // release read
        if (read_release(p_lock) != OK)
        {
            status = THREAD_STATUS_ERR;
            break;
        }

        // check read_line_from_file() & read_release()
        CHECK_STATUS();

        /* convert str to int, calc factori, build output str.
         * if an error occures at my_atoi() or at factori(),
         * it will be detected by build_output_string and
         * status will be set accordingly */
        my_atoi(read_buffer, &number);
        factori_arr = factori(number);
        status = generate_output_string(number, factori_arr, &write_buffer);
        CHECK_STATUS();

        // acquire exclusive write access
        status = write_lock(p_lock);
        CHECK_STATUS();

        // write output (and free write buffer)
        status = print_line_to_file(h_file, &write_buffer);

        // release write lock
        if (write_release(p_lock) != OK)
        {
            status = THREAD_STATUS_ERR;
            break;
        }
    }

    // cleanup
    if (h_file)
    {
        if (!CloseHandle(h_file))
        {
            PRINT_ERROR(E_WINAPI, 0);
            status = THREAD_STATUS_ERR;
        }
    }
    if (write_buffer)
        free(write_buffer);

    ExitThread((DWORD)status);
}
