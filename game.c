#include "game.h"
#include "buffer.h"
#include "utils.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <memory.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define ABS(A) ((A) < 0 ? (-(A)) : (A))

typedef struct
{
  float elapse;

  int row, col;
  int crow;

  int* cars;

  int prow, pcol;
  int grow, gcol;

  int frame;

  State state;

  MapBuffer* map;
} GameData;

#define INDEX(DATA, XPOS, YPOS) (DATA->trow * YPOS + XPOS)

/**
 * @brief Get a random boolean-like int, 1 for true and -1 for false
 */
int get_rand_bool()
{
  return randomUCP(0, 1) * 2 - 1;
}

/**
 * @brief Get a random car position (column index), negative for left-oriented and positive for right-oriented
 *
 * @param data Game context
 * @return int Car position
 */
int get_rand_car(GameData* data)
{
  return get_rand_bool() * (rand() % data->col);
}

/**
 * @brief Get the row index of a car given its index
 *
 * @param id Car index
 * @return int Car's row index
 */
int get_car_row(int id)
{
  return 2 * id + 1;
}

/**
 * @brief Initialize first frame's cells
 *
 * @param data Game context
 */
void init_cells(GameData* data)
{
  int i, j, rand_car;

  /* set player and goal */
  set_cell(data->map, data->prow, data->pcol, PLAYER);
  set_cell(data->map, data->grow, data->gcol, GOAL);

  /* set traffic lines */
  for (i = 0; i < data->crow; i++)
  {
    for (j = 0; j < data->col; j++)
    {
      set_cell(data->map, get_car_row(i), j, TRAFFIC_LINE);
    }
  }

  /* init cars */
  for (i = 0; i < data->crow; i++)
  {
    rand_car = get_rand_car(data);
    /* if the car's at the right edge, it should be oriented left */
    if (rand_car == data->col - 1 && rand_car > 0)
      rand_car = -rand_car;

    data->cars[i] = rand_car;
    set_cell(data->map, get_car_row(i),
      rand_car >= 0 ? rand_car : -rand_car,
      rand_car >= 0 ? CAR_RIGHT : CAR_LEFT);
  }

  /* now, the map has been initialized */
}

/**
 * @brief Initialize game context
 *
 * @param row Row count, should be odd
 * @param col Column count
 * @param fps Speed
 * @return GameHandle Game context
 */
GameHandle init_game(int row, int col, int fps)
{
  /* set random seed */
  initRandom();

  /* initialize game data */
  GameData* data = (GameData*)malloc(sizeof(GameData));
  if (!data)
  {
    return 0;
  }

  /* initialize sizes */
  data->row = row;
  data->col = col;
  data->crow = (row - 1) / 2;

  /* initialize cars */
  data->cars = (int*)malloc(data->crow * sizeof(int));
  if (!data->cars)
  {
    free(data);
    return 0;
  }

  /* initialize player and goal's position */
  data->prow = data->pcol = 0;
  data->grow = data->row - 1;
  data->gcol = data->col - 1;

  /* initialize frame buffer and maps */
  data->map = init_buffers(data->row, data->col);
  if (!data->map)
  {
    free(data->cars);
    free(data);
    return 0;
  }

  /* initialize cells */
  init_cells(data);

  /* calculate elapse time per frame */
  data->elapse = 1.0f / fps;

  /* initialize state */
  data->frame = 0;
  data->state = STATE_IDLE;

  return data;
}

/**
 * @brief Process input
 *
 * @param handle Game context
 * @return int Succeed
 */
int process_input(GameHandle handle)
{
  GameData* data = (GameData*)handle;
  char ch;
  disableBuffer();
  scanf(" %c", &ch); /* get input */
  enableBuffer();
  switch (ch)
  {
  case 'a': /* left */
    data->pcol = MAX(data->pcol - 1, 0);
    break;
  case 'd': /* right */
    data->pcol = MIN(data->pcol + 1, data->col - 1);
    break;
  case 'w': /* up */
    data->prow = MAX(data->prow - 1, 0);
    break;
  case 's': /* down */
    data->prow = MIN(data->prow + 1, data->row - 1);
    break;
  default:
    return 0;
  }
  return 1;
}

/**
 * @brief Next frame
 *
 * @param handle Game context
 * @return int Succeed
 */
int step(GameHandle handle)
{
  int i, j;
  GameData* data = (GameData*)handle;

  /* update cars */
  for (i = 0; i < data->crow; i++)
  {
    /* if the car's oriented right, go one column ahead */
    data->cars[i]++;
    if (data->cars[i] > 0)
    {
      /* if the car's at the right edge, turn back */
      if (data->cars[i] == data->col - 1)
        data->cars[i] = -data->cars[i];
    }
    /* if the car's oriented left, go one column back */
    else
    {
      /* if the car's at the left edge, turn back */
      if (data->cars[i] == 0)
        data->cars[i] = -data->cars[i];
    }
    /* if the car's at the player's position (the player's got crashed), game over */
    if (get_car_row(i) == data->prow && data->cars[i] == data->pcol)
    {
      data->state = STATE_GAMEOVER;
    }
  }
  /* get to the goal */
  if (data->prow == data->grow && data->pcol == data->gcol)
  {
    data->state = STATE_WON;
  }
  data->frame++;
  return 1;
}

/**
 * @brief Draw the map to the back frame buffer
 */
int draw(GameHandle handle)
{
  int i, j;
  GameData* data = (GameData*)handle;

  /* draw cars and the traffic line */
  for (i = 0; i < data->crow; i++)
  {
    for (j = 0; j < data->col; j++)
      set_cell(data->map, get_car_row(i), j, TRAFFIC_LINE);
    set_cell(data->map, get_car_row(i), ABS(data->cars[i]), data->cars[i] >= 0 ? CAR_RIGHT : CAR_LEFT);
  }

  /* draw the player and goal */
  set_cell(data->map, data->prow, data->pcol, PLAYER);
  set_cell(data->map, data->grow, data->gcol, GOAL);
}

/**
 * @brief Run the game
 */
State run(GameHandle handle)
{
  GameData* data = (GameData*)handle;
  time_t start_time = time(0), now;
  data->state = STATE_GAMING;

  int last = 0;
  while (1)
  {
    clear_buffer(data->map);
    draw(handle);

    swap_buffer(data->map);
    print_buffer(data->map);

    printf("Press w to move up\n");
    printf("Press s to move down\n");
    printf("Press a to move left\n");
    printf("Press d to move right\n");

    if (last)
      break;

    now = time(0);
    if (!process_input(handle))
      continue;

    //newSleep(data->frame * data->elapse - (now - start_time) / 1000);
    step(handle);

    if (data->state != STATE_GAMING)
    {
      last = 1;
    }
  }
  return data->state;
}

/**
 * @brief Clear memory
 */
int destroy_game(GameHandle handle)
{
  GameData* data = (GameData*)handle;
  destroy_buffer(&data->map);
  free(data->cars);
  free(data);
  return 1;
}