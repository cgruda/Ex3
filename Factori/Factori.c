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

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */

DWORD WINAPI factori_thread(LPVOID param)
{
    HANDLE file    = NULL;
    DWORD rc = ERR;
    DWORD wait_code;
    struct thread_args* args;
    int status = 0;

    args = (struct thread_args*)(param);

    while(args->p_queue->cnt)
    {
        status = 0;

        // check abort thread event
        wait_code = WaitForSingleObject(*(args->abort_evt), 0);
        ASSERT_BREAK(wait_code != WAIT_FAILED);
        if (wait_code == WAIT_OBJECT_0)
        {
            printf("ABORT evnt was set\n");
            status = 1; // use this to handle free
            // free stuff
            break;
            ExitThread(ERR);
        }

        // lock queue?
        // pop a task
        // unlock queue?
        
        // SHARE FILE (read_lock)
        // read from file
        // DONT NEED FILE (read_release)

        // calc factori

        // OWN_FILE (write_lock)
        // print to file
        // FREE_FILE (write_release)

        //


    }
    

    
    // wait for start event from main thread
    

    int start = args->section->start;
    int length = args->section->end - start;

    // do-while(0) used for easier error cleanup
    do {
        // sanity
        if (start < 0 || length < 0)
            break;

        // sanity
        if (!args->inpath || !args->outpath)
            break;

        // input file handle
        h_infile = CreateFileA(args->inpath,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (h_infile == INVALID_HANDLE_VALUE)
            break;

        // output file handle
        h_outfile = CreateFileA(args->outpath,
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (h_outfile == INVALID_HANDLE_VALUE)
            break;

        // seek positions
        if ((SetFilePointer(h_infile, start, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ||
            (SetFilePointer(h_outfile, start, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER))
            break;

        // buffer alocation
        if (!(buffer = (char*)calloc(length, sizeof(char))))
            break;

        // read input file
        if (!ReadFile(h_infile, buffer, length, NULL, NULL))
            break;

        // encode/decode buffer
        encode_decode_buffer(buffer, length, args->key);

        // write output file
        if (!WriteFile(h_outfile, buffer, length, NULL, NULL))
            break;

        rc = OK;

    } while (0);

    if (rc == ERR)
        printf("encode_decode_thread: WinAPI Error 0x%X\n", GetLastError());

    // free thread resources
    if (h_infile)
        if (!CloseHandle(h_infile))
            rc = ERR;
    if (h_outfile)
        if (!CloseHandle(h_outfile))
            rc = ERR;
    if (buffer)
        free(buffer);

    ExitThread((DWORD)rc);
}
