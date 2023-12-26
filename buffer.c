#include "buffer.h"

#include <stdlib.h>
#include <stdio.h>

MapBuffer *init_buffers(int row, int col)
{
  MapBuffer *map = (MapBuffer *)malloc(sizeof(MapBuffer));

  map->row = row;
  map->col = col;

  map->buf_back = (CellElement *)malloc(map->row * map->col * sizeof(CellElement));
  map->buf_front = (CellElement *)malloc(map->row * map->col * sizeof(CellElement));

  return map;
}

int set_cell(MapBuffer *map, int row, int col, CellElement ele)
{
  int res = 0;
  if (!(row < 0 || row > map->row - 1 || col < 0 || col > map->col - 1))
  {
    res = 1; /* not wall or out of range */
	map->buf_back[map->col * row + col] = ele;
  }
  return res;
}

CellElement get_cell(MapBuffer *map, int row, int col, int back)
{
  CellElement res = BAD_INDEX;
  if (!(row < 0 || row > map->row - 1 || col < 0 || col > map->col - 1))
  {
    res = (back ? map->buf_back : map->buf_front)[map->col * row + col];/* not out of range */
  }
  return res;
}

int clear_buffer(MapBuffer *map)
{
  int i, j;
  /* clear the map */
  for (i = 0; i < map->row; i++)
  {
    for (j = 0; j < map->col; j++)
    {
      set_cell(map, i, j, BLANK);
    }
  }
  return 1;
}

int swap_buffer(MapBuffer *map)
{
  CellElement *tmp = map->buf_back;
  map->buf_back = map->buf_front;
  map->buf_front = tmp;

  return 1;
}

int print_buffer(MapBuffer *map)
{
  int i, j;

  system("clear");
  system("tput cup 0 0");

  for (j = 0; j < map->col + 2; j++)
    printf("*");
  printf("\n");

  for (i = 0; i < map->row; i++)
  {
    printf("*");
    for (j = 0; j < map->col; j++)
    {
      printf("%c", get_cell(map, i, j, 0));
    }
    printf("*");
    printf("\n");
  }

  for (j = 0; j < map->col + 2; j++)
    printf("*");
  printf("\n");
  return 1;
}

int destroy_buffer(MapBuffer **map)
{
  MapBuffer *pmap = *map;
  free(pmap->buf_back);
  free(pmap->buf_front);
  free(pmap);
  *map = 0;
  return 1;
}