/**
 * main.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include "tasks.h"
#include "factori.h"

/*
 ==============================================================================
 * MAIN
 ==============================================================================
 */

int main(int argc, char **argv)
{
    struct enviroment env = {0};
    int ret_val = ERR;

    // check input args
    if (!check_input(&env, argc, argv))
        return ERR;

    // do-while(0) for easy cleanup
    do
    {
        if (!init_factori(&env))
            break;

        // fill queue
        if (!fill_factori_queue(&env)) // TODO:
            break;

        // start factori threads
        if (!create_factori_threads(&env))
            break;

        // wait for factori threads to end
        if (!wait_for_factori_threads(&env))
            break;
        
        // if got here then success
        ret_val = OK;

    } while (0);

    // free resources
    if (!cleanup_factori(&env))
        ret_val = ERR;

    return ret_val;
}
