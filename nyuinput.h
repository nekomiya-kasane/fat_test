#ifndef __NYU_INPUT_H__
#define __NYU_INPUT_H__

typedef enum
{
  OP_NONE,
  OP_PRINT_INFO,
  OP_LIST_DIR,
  OP_RECOVER_CONTIGUOUS,
  OP_RECOVER_NON_CONTIGUOUS
} OperationCode;

typedef struct
{
  char* disk;
  OperationCode op;

  char* filename;
  char* sha1;
  char* sha1de;
} Input;

Input* parse_input(int argc, char* argv[]);

void destroy_input(Input** input);

#endif