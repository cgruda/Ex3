/**
 * main.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "tasks.h"

/*
 ******************************************************************************
 * MAIN
 ******************************************************************************
 */

int main(int argc, char **argv)
{
    // argv[0] - "Factori.exe"
    // argv[1] - "tasks input file - and also output file"
    // argv[2] - "priority input file"
    // argv[3] - "num" (number of lines in files)
    // argv[4] - "num" (number of threads)
    int ret_val;
    struct args args;
    struct Queue *q;

    ret_val = init(&args, argc, argv);
    if (ret_val == ERR)
    {
        return ERR;
    }

    // fill queue

    // call threads

    // end

    // this is a test commit 2
    return 0;
}
