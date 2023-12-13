#include "nyuinput.h"
#include "nyuutils.h"

#include <stdlib.h>
#include <string.h>

Input* parse_input(int argc, char* argv[])
{
  int succeed = 0;

  Input* input = malloc(sizeof(Input));
  input->disk = input->filename = input->sha1 = input->sha1de = 0;
  input->op = OP_NONE;

  while (1)
  {
    if (argc < 2 || argc > 5)
      break;

    input->disk = argv[1];
    if (argc == 2)
    {
      succeed = 1;
      break;
    }

    const char* op = argv[2];
    if (strlen(op) != 2 || op[0] != '-' || (op[1] != 'i' && op[1] != 'l' && op[1] != 'r' && op[1] != 'R'))
    {
      break;
    }

    if (op[1] == 'i')
    {
      input->op = OP_PRINT_INFO;
      if (argc == 3)
      {
        succeed = 1;
      }
      break;
    }
    if (op[1] == 'l')
    {
      input->op = OP_LIST_DIR;
      if (argc == 3)
      {
        succeed = 1;
      }
      break;
    }
    if (op[1] == 'r')
    {
      input->op = OP_RECOVER_CONTIGUOUS;
      if (argc == 3)
        break;

      input->filename = argv[3];
      if (argc == 4)
      {
        succeed = 1;
        break;
      }

      const char* op2 = argv[4];
      if (argc == 5 || op2[0] != '-' || op2[1] != 's')
        break;

      input->sha1 = argv[5];
      if (argc == 6 || (strlen(argv[5]) == 40))
      {
        input->sha1de = hexToBytes(input->sha1, 40);
        if (input->sha1de)
          succeed = 1;
      }
      break;
    }
    if (op[1] == 'R')
    {
      input->op = OP_RECOVER_NON_CONTIGUOUS;
      if (argc != 6)
        break;

      input->filename = argv[3];
      input->sha1 = argv[5];
      succeed = 1;
      break;
    }
    break;
  }

  if (!succeed)
  {
    free(input->sha1de);
    free(input);
    input = 0;
  }
  return input;
}

void destroy_input(Input** input)
{
  if (!input)
    return;

  free((*input)->sha1de);
  free(*input);
  *input = 0;
}