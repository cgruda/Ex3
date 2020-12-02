/**
 * Lock.c
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
#include "Lock.h"
#include "tasks.h"

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */
struct Lock *InitializeLock()
{
    struct Lock *p_lock = NULL;
    int status = OK;

    // allocate lock
    p_lock = calloc(1, sizeof(*p_lock));
    ASSERT_RETURN_VAL(p_lock, NULL);

    // mutex handle
    p_lock->h_write_mtx = CreateMutex(NULL, FALSE, MUTX_NAME);
    if (!p_lock->h_write_mtx)
    {
        status = ERR;
    }

    // semaphore handle
    p_lock->h_read_smpr = CreateSemaphore(NULL, 0, MAXIMUM_WAIT_OBJECTS, SMPR_NAME);
    if (!p_lock->h_read_smpr)
    {
        status = ERR;
    }

    // incase create handle failed
    if (status != OK)
    {
        ASSERT_PRINT(0);
        DestroyLock(p_lock);
    }

    return p_lock;
}

// aloow other read. dont allow write
int read_lock(struct Lock *p_lock)
{
    ASSERT_RETURN_VAL(p_lock, ERR);

    int ret_val = OK;
    DWORD wait_code;

    // make sure no write_lock
    wait_code = WaitForSingleObject(p_lock->h_write_mtx, MAX_WAIT_MS_R);
    if (wait_code != WAIT_OBJECT_0)
    {
        ASSERT_RETURN_VAL(0, ERR_TIMOUT);
    }

    // increase semaphore by 1, indicating caller IS reading
    if (!ReleaseSemaphore(p_lock->h_read_smpr, 1, &p_lock->smpr_cnt))
    {
        ret_val = ERR;
    }

    // semaphore disables write, allow other threads increase semaphore
    if(!ReleaseMutex(p_lock->h_write_mtx))
    {
        ret_val = ERR;
    }
    // if not locked for WRITE - SUCCESS
    // if locked due to WRITE - WAIT
    // if timeout - FAIL, else SUCCESS
    
    // each reader increase smpr by 1 (by wait)
    // if mutex is not locked (i.e. no writing is in progress) - increase smpr (by calling release smpr)
    //        (also increase smpr counter)
    // if mutex is locked lock for reading fails
    ASSERT_PRINT(ret_val == OK);

    return ret_val;
}

int read_release(struct Lock *p_lock)
{
    ASSERT_RETURN_VAL(p_lock, ERR);

    // use a h_read_smpr. when seam=0: no one is reading
    // 
    // if mutex is on then we shouldent be here - sanity
    // decrease the seamaphore by one by calling wait(0). update smpr_count
}

int write_lock(struct Lock *p_lock)
{
    ASSERT_RETURN_VAL(p_lock, ERR);

    DWORD wait_code;
    // no one else locked for write - SUCCESS 
    // no one else locked for read  - SUCCESS
    // else - WAIT
    // return only when managed to lock. so there is no FAIL really. only wait enough time

    // check no read_lock
    while (p_lock->smpr_cnt != 0);

    // check no write_lock
    wait_code = WaitForSingleObject(p_lock->h_write_mtx, MAX_WAIT_MS_W);

    // if smpr_count > 0 then somone is reading - FAIL
    // else lock the mtx thus not alowing write - OK
}

int write_release(struct Lock *p_lock)
{
    ASSERT_RETURN_VAL(p_lock, ERR);

    int ret_val = OK;

    if (!ReleaseMutex(p_lock->h_write_mtx))
    {
        ret_val = ERR;
    }

    ASSERT_PRINT(ret_val == OK);

    return ret_val;
}

int DestroyLock(struct Lock *p_lock)
{
    ASSERT_RETURN_VAL(p_lock, ERR);

    int ret_val = OK;

    if (p_lock->h_write_mtx)
    {
        if (!CloseHandle(p_lock->h_write_mtx))
        {
            ret_val = ERR;
        }
    }
    
    if (p_lock->h_read_smpr)
    {
        if (!CloseHandle(p_lock->h_read_smpr))
        {
            ret_val = ERR;
        }
    }

    ASSERT_PRINT(ret_val == OK);

    free(p_lock);
    p_lock = NULL;

    return ret_val;
}