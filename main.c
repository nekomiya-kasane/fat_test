#include <stdlib.h>
#include <stdio.h>

#include "game.h"

int main(int argc, const char **argv)
{
    int row, col;
    GameHandle handle;
    State res;

    if (argc != 3)
    {
        printf("Wrong arguments!\n");
        printf("Usage: ./box <map_row> <map_col>");
        return 1;
    }

    row = atoi(argv[1]);
    col = atoi(argv[2]);
    if (col < 5)
    {
        printf("Error: Minimum value for <map_col> is 5.\n");
        return 1;
    }
    if (row < 3 || row % 2 == 0)
    {
        printf("Error: <map_row> must be an odd number greater than or equal to 3.\n");
        return 1;
    }

    handle = init_game(row, col, 5);
    res = run(handle);

    switch (res)
    {
    case STATE_WON:
        printf("YOU WIN!\n");
        break;
    case STATE_GAMEOVER:
        printf("YOU LOSE!\n");
        break;
    default:
        break;
    }

    return 0;
}