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
  if (row < 0 || row > map->row - 1 || col < 0 || col > map->col - 1)
  {
    return 0; /* wall or out of range */
  }

  map->buf_back[map->col * row + col] = ele;
  return 1;
}

CellElement get_cell(MapBuffer *map, int row, int col, int back)
{
  if (row < 0 || row > map->row - 1 || col < 0 || col > map->col - 1)
  {
    return BAD_INDEX; /* out of range */
  }
  return (back ? map->buf_back : map->buf_front)[map->col * row + col];
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
}

int print_buffer(MapBuffer *map)
{
  int i, j;

  for (j = 0; j < map->col + 2; j++)
    printf("*");

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