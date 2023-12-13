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
  const char* disk;
  OperationCode op;
  const char* filename;
  const char* sha1;
  const char* sha1de;
} Input;

Input* parse_input(int argc, char* argv[]);

void destory_input(Input** input);

#endif