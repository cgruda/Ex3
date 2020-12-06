/**
 * Lock.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * this is the header file of tasks module
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

// debug prints enable
#define DBG_ENABLE     1

// input arguments count
#define ARGC           5

// define return values
#define ERR 0
#define OK  1

// for debug use
#define __FILENAME__   (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// wating times
#define MAX_WAIT_TIME_ALL_MS (10 * MIN2SEC * SEC2MS)
#define WAIT_0_MS       0
#define SEC2MS          1000
#define MIN2SEC         60

// for printing
#define CR 0xD
#define LF 0xA

// error messages
#define E_MSG_NULL_PTR "Null Pointer\n"
#define E_MSG_MAX_WAIT "Max Wait-time passed\n"
#define E_MSG_BUF_FULL "Buffer is full\n"


/*
 ==============================================================================
 * ENUMERATIONS
 ==============================================================================
 */

enum err_value
{
    E_INTERNAL,
    E_WINAPI,
    E_STDLIB
};


/*
 ==============================================================================
 * MACROS
 ==============================================================================
 */

// debug stamp [file,function,line]
#define DBG_STAMP()     printf("[%s; %s; %d] ", __FILENAME__, __func__, __LINE__)

// for debuging
#if DBG_ENABLE
#define DBG_PRINT(...)  do {DBG_STAMP(); printf(__VA_ARGS__);} while (0)
#else
#define DBG_PRINT(...)
#endif

// print error message
#define PRINT_ERROR(err_val, err_msg)   do {DBG_STAMP(); print_error((err_val), (err_msg));} while (0)


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
    HANDLE *p_h_queue_mtx;
};

struct enviroment
{
    // main params
    struct args args;
    HANDLE *p_h_threads;
    int threads_created;

    // copy of thread args
    struct Queue *p_queue;
    struct Lock  *p_lock;
    HANDLE h_queue_mtx;
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
 * @brief convert string to integer
 * @param str to be converted
 * @param p_result pointer to integer that will get the result
 * @return OK or ERR
 ******************************************************************************
 */
int my_atoi(char *str, int *p_result);

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
 * @param p_env pointer to enviroment struct
 * @param argc from main
 * @param argv from main
 ******************************************************************************
 */
int check_input(struct enviroment *p_env, int argc, char **argv);

/**
 ******************************************************************************
 * @brief intialize all object needed for program (lock, queue, abort event)
 * @param p_env pointer to enviroment struct
 * @return OK or ERR
 ******************************************************************************
 */
int init_factori(struct enviroment *p_env);

/**
 ******************************************************************************
 * @brief fill queue with task ofssets from priorities input file
 * @param p_env pointer to enviroment struct
 * @return OK or ERR
 ******************************************************************************
 */
int fill_factori_queue(struct enviroment *p_env);

/**
 ******************************************************************************
 * @brief create all factori threads
 * @param p_env pointer to enviroment struct
 * @return OK or ERR
 ******************************************************************************
 */
int create_factori_threads(struct enviroment *p_env);

/**
 ******************************************************************************
 * @brief wait for all factori threads to end, and check exit codes
 * @param p_env pointer to enviroment struct
 * @return OK or ERR
 ******************************************************************************
 */
int wait_for_factori_threads(struct enviroment *p_env);

/**
 ******************************************************************************
 * @brief free all program resources
 * @param p_env pointer to enviroment struct
 * @return OK or ERR
 ******************************************************************************
 */
int cleanup_factori(struct enviroment *p_env);


#endif // __TASKS_H__
