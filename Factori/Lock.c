/**
 * Lock.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * this module solves the readers-writer sync problem,
 * allowing concurrent read access, but exclusive write access.
 * the solution is based on Michel Raynal (1949) solution,
 * which uses 2 mutexs and a counter. since WinAPI does not
 * allow a mutex acquired by one thread to be released by another,
 * the global mutex is implemented using a binari semaphore.
 * 
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include "Lock.h"
#include "tasks.h"

/*
 ==============================================================================
 * FUNCTION DEFENITIONS
 ==============================================================================
 */
struct Lock *InitializeLock()
{
    struct Lock *p_lock = NULL;
    int    status       = OK;

    // allocate lock mem
    p_lock = calloc(1, sizeof(*p_lock));
    if (!p_lock)
    {
        PRINT_ERROR(E_STDLIB, 0);
        return ERR;
    }

    // create readers mutex
    p_lock->h_read_mtx = CreateMutexA(NULL, FALSE, MUTX_R);
    if (!p_lock->h_read_mtx)
    {
        PRINT_ERROR(E_WINAPI, 0);
        status = ERR;
    }

    // create global (binari) semaphore handle
    p_lock->h_global_smpr = CreateSemaphoreA(NULL, 1, 1, SMPR_G);
    if (!p_lock->h_global_smpr)
    {
        PRINT_ERROR(E_WINAPI, 0);
        status = ERR;
    }

    // in case a handle creation failed
    if (status != OK)
    {
        DestroyLock(p_lock);
    }

    return p_lock;
}

//==============================================================================

int read_lock(struct Lock *p_lock)
{
    DWORD wait_code;

    // sanity
    if (!p_lock)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_NULL_PTR);
        return ERR;
    }

    // ask for readers mutex, so thread can be added to readers_count
    DBG_PRINT("wait for r lock\n");
    wait_code = WaitForSingleObject(p_lock->h_read_mtx, MAX_WAIT_MS_R);
    LOCK_WAIT_CODE_CHECK(wait_code);

    p_lock->readers_count++;

    // only the FIRST reader locks global, not alowing others to write
    if (p_lock->readers_count == 1)
    {
        DBG_PRINT("wait for g lock\n");
        wait_code = WaitForSingleObject(p_lock->h_global_smpr, MAX_WAIT_MS_R);
        LOCK_WAIT_CODE_CHECK(wait_code);
    }

    // release readers mutx, allowing others to start read
    DBG_PRINT("release r lock\n");
    if(!ReleaseMutex(p_lock->h_read_mtx))
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    return OK;
}

//==============================================================================

int read_release(struct Lock *p_lock)
{
    DWORD wait_code;

    // sanity
    if (!p_lock)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_NULL_PTR);
        return ERR;
    }

    // ask for readers mutex, so thread can decrement readers_counter
    DBG_PRINT("wait for r lock\n");
    wait_code = WaitForSingleObject(p_lock->h_read_mtx, MAX_WAIT_MS_R);
    LOCK_WAIT_CODE_CHECK(wait_code);

    p_lock->readers_count--;

    // only the LAST reader releases global, allowing others to write
    if (p_lock->readers_count == 0)
    {
        DBG_PRINT("release g lock\n");
        if(!ReleaseSemaphore(p_lock->h_global_smpr, 1, NULL))
        {
            PRINT_ERROR(E_WINAPI, 0);
            return ERR;
        }
    }

    // release readers mutex, allowing others to update readers_count
    DBG_PRINT("release r lock\n");
    if(!ReleaseMutex(p_lock->h_read_mtx))
    {
        return ERR;
    }

    return OK;
}

//==============================================================================

int write_lock(struct Lock *p_lock)
{
    DWORD wait_code;

    // sanity
    if (!p_lock)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_NULL_PTR);
        return ERR;
    }

    // wait for global semaphore, that will grant exclusive writing
    wait_code = WaitForSingleObject(p_lock->h_global_smpr, MAX_WAIT_MS_R);
    LOCK_WAIT_CODE_CHECK(wait_code);

    return OK;
}

//==============================================================================

int write_release(struct Lock *p_lock)
{
    // sanity
    if (!p_lock)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_NULL_PTR);
        return ERR;
    }

    // release global semaphore, allowing others to get write or read access
    if (!ReleaseSemaphore(p_lock->h_global_smpr, 1, NULL))
    {
        PRINT_ERROR(E_WINAPI, 0);
        return ERR;
    }

    return OK;
}

//==============================================================================

int DestroyLock(struct Lock *p_lock)
{
    int ret_val = OK;

    // sanity
    if (!p_lock)
    {
        PRINT_ERROR(E_INTERNAL, E_MSG_NULL_PTR);
        return ERR;
    }

    // close mutex handler
    if (p_lock->h_read_mtx)
    {
        if (!CloseHandle(p_lock->h_read_mtx))
        {
            PRINT_ERROR(E_WINAPI, 0);
            ret_val = ERR;
        }
    }

    // close semaphore handler
    if (p_lock->h_global_smpr)
    {
        if (!CloseHandle(p_lock->h_global_smpr))
        {
            PRINT_ERROR(E_WINAPI, 0);
            ret_val = ERR;
        }
    }

    free(p_lock);
    p_lock = NULL;

    return ret_val;
}