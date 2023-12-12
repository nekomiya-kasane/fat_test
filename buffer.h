#ifndef __CHAR_BUFFER__
#define __CHAR_BUFFER__

/** @brief Cell elements */
typedef enum
{
  BLANK = ' ',
  TRAFFIC_LINE = '.',
  CAR_LEFT = '<',
  CAR_RIGHT = '>',
  GOAL = 'G',
  PLAYER = 'P',
  BAD_INDEX = 0
} CellElementType;

typedef char CellElement;

/** @brief The frame buffer */
typedef struct {
  int row, col;
  CellElement* buf_back, * buf_front;
} MapBuffer;

/** @brief Initialize a pair of buffers */
MapBuffer* init_buffers(int row, int col);

/** @brief Set a cell in the buffer with ele */
int set_cell(MapBuffer* map, int row, int col, CellElement ele);

/** @brief Get a cell content in the buffer */
CellElement get_cell(MapBuffer* map, int row, int col, int back);

/** @brief Swap the front and back buffer */
int swap_buffer(MapBuffer* map);

/** @brief Clear the buffer to BLANK */
int clear_buffer(MapBuffer* map);

/** @brief Print the buffer with * surrounded */
int print_buffer(MapBuffer* map);

/** @brief Destroy the buffer */
int destroy_buffer(MapBuffer** map);

#endif