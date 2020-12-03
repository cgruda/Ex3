/**
 * Lock.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * 
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __TASKS_H__
#define __TASKS_H__


// for using PathFileExistsA()
#pragma comment(lib, "Shlwapi.lib")

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Shlwapi.h"


/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

#define __FILENAME__  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define ARGC 5
#define SEC2MS          1000
#define MIN2SEC         60
#define MAX_WAIT_TIME_ALL_MS (10 * MIN2SEC * SEC2MS)
#define WAIT_0_MS        0

#define ERR 0
#define OK  1


/*
 ******************************************************************************
 * MACROS
 ******************************************************************************
 */
// debug stamp [file,function,line]
#define DBG_STAMP()                 printf("[%s,%s,%d]", __FILENAME__, __func__, __LINE__)
// debug print
#define DBG_PRINT(...)              do {DBG_STAMP(); printf(__VA_ARGS__);} while (0)
// print error message
#define ASSERT_PRINT(ok)            do {if (!(ok)) {DBG_STAMP(); print_error();}} while (0)
// print error and return
#define ASSERT_RETURN(ok)           do {if (!(ok)) {ASSERT_PRINT(0); return;}} while (0)
// print error and return value
#define ASSERT_RETURN_VAL(ok, val)  do {if (!(ok)) {ASSERT_PRINT(0); return (val);}} while (0)
// print error and break
#define ASSERT_BREAK(ok)            do {if (!(ok)) {ASSERT_PRINT(0); break;}} while (0)
// print error and continue
#define ASSERT_CONTINUE(ok)         do {if (!(ok)) {ASSERT_PRINT(0); continue;}} while (0)
// print error and exit
#define ASSERT_EXIT_FAIL(ok)        do {if (!(ok)) {ASSERT_PRINT(0); exit(EXIT_FAILURE);}} while (0)


enum e_value
{
    E_INTERNAL,
    E_WINAPI,
    E_STDLIB
};



#define PRINT_ERROR(err_val, err_msg)   do {DBG_STAMP(); print_error((err_val), (err_msg));} while (0)
#define E_MSG_NULL_PTR "Null Pointer\n"
#define E_MSG_MAX_WAIT "Max Wait-time passed\n"

#define WAIT_CODE_CHECK(wait_code)               \
{                                                \
    switch (wait_code)                           \
    {                                            \
    case WAIT_OBJECT_0:                          \
        break;                                   \
    case WAIT_FAILED:                            \
        PRINT_ERROR(E_WINAPI, 0);                \
        return ERR;                              \
    default:                                     \
        PRINT_ERROR(E_INTERNAL, E_MSG_MAX_WAIT); \
        return ERR;                              \
    }                                            \
}


/*
 ******************************************************************************
 * STRUCTURES
 ******************************************************************************
 */
struct args
{
    char* path1;
    char* path2;
    int   n_lines;
    int   n_threads;
};

struct thread_args
{
    char *path;
    struct Queue *p_queue;
    struct Lock  *p_lock;
    HANDLE *abort_evt;
};

/*
 ******************************************************************************
 * DECLARATIONS
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief
 * @param
 * @param
 * @return
 ******************************************************************************
 */
void print_usage();

int init(struct args *args, int argc, char **argv);


void print_error(int err_val, char *err_msg);

/**
 ******************************************************************************
 * @brief
 * @param str
 * @param p_result
 * @return OK or ERR
 ******************************************************************************
 */
int my_atoi(char *str, int *p_result);

/**
 ******************************************************************************
 * @brief
 * @param path to priority input file 
 * @return pointer to Queue struct
 ******************************************************************************
 */
struct Queue *fill_queue(char *path);

/**
 ******************************************************************************
 * @brief 
 * @param num
 * @param
 * @return null termianted array
 ******************************************************************************
 */
int *factori(int num);

/**
 ******************************************************************************
 * @brief ... and free array
 * @param path to priority input file 
 * @param array null terminated
 * @return pointer to Queue struct
 ******************************************************************************
 */
void print_output(int *array, char *path);


int create_n_threads(LPTHREAD_START_ROUTINE thread_func, HANDLE *p_h_threads,
    int n_threads, LPVOID args);


int wait_for_n_threads(HANDLE *p_h_threads, int n_threads);



#endif // __TASKS_H__
