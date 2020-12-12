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

    if (!check_input(&env, argc, argv))
        return ERR;

    // do-while(0) for easy cleanup
    do
    {
        if (!init_factori(&env))
            break;

        if (!fill_factori_queue(&env))
            break;

        if (!create_factori_threads(&env))
            break;

        if (!wait_for_factori_threads(&env))
            break;

        ret_val = OK;

    } while (0);

    if (!cleanup_factori(&env))
        ret_val = ERR;

    DBG_PRINT("ret_val=%d\n", ret_val);
    return ret_val;
}
