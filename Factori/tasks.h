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

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

#define __FILENAME__  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)



enum status
{
    ERR,
    ERR_TIMOUT,
    OK,
};

#define SEC2MS          1000
#define MIN2SEC         60

/*
 ******************************************************************************
 * MACROS
 ******************************************************************************
 */
// debug stamp [file,function,line]
#define DBG_STAMP()                 printf("[%s,%s,%d]", __FILENAME__, __func__, __LINE__)
// debug print
#define DBG_PRINT(...)              do {DBG_STAMP(); printf(__VA_ARGS__);} while (0);
// print error message
#define ASSERT_PRINT(ok)            do {if (!(ok)) {DBG_STAMP(); print_error();}} while (0);
// print error and return
#define ASSERT_RETURN(ok)           do {if (!(ok)) {ASSERT_PRINT(0); return;}} while (0);
// print error and return value
#define ASSERT_RETURN_VAL(ok, val)  do {if (!(ok)) {ASSERT_PRINT(0); return (val);}} while (0);
// print error and break
#define ASSERT_BREAK(ok)            do {if (!(ok)) {ASSERT_PRINT(0); break;}} while (0);
// print error and continue
#define ASSERT_CONTINUE(ok)         do {if (!(ok)) {ASSERT_PRINT(0); continue;}} while (0);
// print error and exit
#define ASSERT_EXIT_FAIL(ok)        do {if (!(ok)) {ASSERT_PRINT(0); exit(EXIT_FAILURE);}} while (0);


 /*
 ******************************************************************************
 * STRUCTURES
 ******************************************************************************
 */


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

void print_error();

/**
 ******************************************************************************
 * @brief
 * @param str
 * @param p_result
 * @return OK or ERR
 ******************************************************************************
 */
int my_atoi(char* str, int* p_result);

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

#endif // __TASKS_H__
