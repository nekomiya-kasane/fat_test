#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include "nyuutils.h"
#include "nyudisk.h"
#include "nyuinput.h"

void print_usage()
{
  printf("Usage: ./nyufile disk <options>\n");
  printf("  -i                     Print the file system information.\n");
  printf("  -l                     List the root directory.\n");
  printf("  -r filename [-s sha1]  Recover a contiguous file.\n");
  printf("  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
}

void print_file_system_info(DiskHandler* handler)
{
  if (!handler)
    return;

  BootEntry* bootEntry = handler->bootEntry;

  printf("Number of FATs = %d", bootEntry->BPB_NumFATs);
  printf("Number of bytes per sector = %d", bootEntry->BPB_BytsPerSec);
  printf("Number of sectors per cluster = %d", bootEntry->BPB_SecPerClus);
  printf("Number of reserved sectors = %d", bootEntry->BPB_RsvdSecCnt);
}

void print_root_dir(DiskHandler* handler)
{
  if (!handler)
    return;

  DirEntityNode* rootEntity = dh_get_entries(handler, handler->bootEntry->BPB_RootClus);
  DirEntityNode* curNode = rootEntity;

  while (curNode) {
    char ch = curNode->entry->DIR_Name[0];
    if (ch == 0x00) { 
      break;
    }
    if (ch == 0xE5 || ch == ' ') { /* deleted */
      curNode = curNode->next;
      continue;
    }


  }
}

int main(int argc, char* argv[])
{
  Input* input = parse_input(argc, argv);
  if (!input)
  {
    print_usage();
    return 1;
  }

  DiskHandler* disk = open_disk(input->disk);
  if (!disk)
  {
    free(input->sha1de);
    free(input);
    return 1;
  }

  if (input->op == OP_PRINT_INFO)
  {
    print_file_system_info(disk);
    return 0;
  }
  if (input->op == OP_LIST_DIR)
  {
    print_root_dir(disk);
  }
  return 0;
}