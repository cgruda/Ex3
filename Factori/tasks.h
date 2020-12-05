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
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Shlwapi.h"
#include "Queue.h"


/*
 ==============================================================================
 * DEFINES
 ==============================================================================
 */

#define __FILENAME__  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define ARGC 5
#define SEC2MS          1000
#define MIN2SEC         60
#define MAX_WAIT_TIME_ALL_MS (10 * MIN2SEC * SEC2MS)
#define WAIT_0_MS        0
#define INT_LENGTH      32

#define CR 0xD
#define LF 0xA

#define ERR 0
#define OK  1


/* // TODO:
 ==============================================================================
 * MACROS
 ==============================================================================
 */
// debug stamp [file,function,line]
#define DBG_STAMP()     printf("[%s,%s,%d]", __FILENAME__, __func__, __LINE__)
// debug print
#define DBG_PRINT(...)  do {DBG_STAMP(); printf(__VA_ARGS__);} while (0)

enum err_value // TODO:
{
    E_INTERNAL,
    E_WINAPI,
    E_STDLIB
};

#define PRINT_ERROR(err_val, err_msg)   do {DBG_STAMP(); print_error((err_val), (err_msg));} while (0)

#define E_MSG_NULL_PTR "Null Pointer\n"
#define E_MSG_MAX_WAIT "Max Wait-time passed\n"
#define E_MSG_BUF_FULL "Buffer is full\n"


/*
 ==============================================================================
 * STRUCTURES
 ==============================================================================
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
    char  *path;
    struct Queue *p_queue;
    struct Lock  *p_lock;
    HANDLE *p_h_abort_evt;
};

struct enviroment
{
    // main params
    struct args args;
    HANDLE *p_h_threads;

    // copy of thread args
    struct Queue *p_queue;
    struct Lock  *p_lock;
    HANDLE h_abort_evt;

    // to be passed to thread
    struct thread_args thread_args;
};

/*
 ==============================================================================
 * DECLARATIONS
 ==============================================================================
 */

/**
 ******************************************************************************
 * @brief print program usage
 ******************************************************************************
 */
void print_usage();

/**
 ******************************************************************************
 * @brief print error message
 * @param err_val enum err_value
 * @param err_msg E_MSG to be printed
 ******************************************************************************
 */
void print_error(int err_val, char *err_msg);

/**
 ******************************************************************************
 * @brief check validity of inputs arguments
 * @param env pointer to enviroment struct
 * @param argc from main
 * @param argv from main
 ******************************************************************************
 */
int check_input(struct enviroment *env, int argc, char **argv);

/**
 ******************************************************************************
 * @brief convert string to integer
 * @param str to be converted
 * @param p_result pointer to integer that will get the result
 * @return OK or ERR
 ******************************************************************************
 */
int my_atoi(char *str, int *p_result);

/**
 ******************************************************************************
 * @brief intialize all object needed for program (lock, queue, abort event)
 * @param p_env pointer to enviroment struct
 * @return OK or ERR
 ******************************************************************************
 */
int init_factori(struct enviroment *p_env);

/** // TODO:
 ******************************************************************************
 * @brief fill queue with task ofssets from priorities input file
 * @param 
 * @return 
 ******************************************************************************
 */
struct Queue *fill_queue(char *path);

/** // TODO:
 ******************************************************************************
 * @brief 
 * @param num
 * @param
 * @return null termianted array
 ******************************************************************************
 */
int *factori(int num);

/** // TODO:
 ******************************************************************************
 * @brief 
 * @param num
 * @param
 * @return null termianted array
 ******************************************************************************
 */
int create_n_threads(LPTHREAD_START_ROUTINE thread_func, HANDLE *p_h_threads,
    int n_threads, LPVOID args);

/** // TODO:
 ******************************************************************************
 * @brief 
 * @param num
 * @param
 * @return null termianted array
 ******************************************************************************
 */
int wait_for_n_threads(HANDLE *p_h_threads, int n_threads);


#endif // __TASKS_H__
