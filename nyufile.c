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
#include "nyurecover.h"

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

  printf("Number of FATs = %d\n", bootEntry->BPB_NumFATs);
  printf("Number of bytes per sector = %d\n", bootEntry->BPB_BytsPerSec);
  printf("Number of sectors per cluster = %d\n", bootEntry->BPB_SecPerClus);
  printf("Number of reserved sectors = %d\n", bootEntry->BPB_RsvdSecCnt);
}

void print_root_dir(DiskHandler* handler)
{
  if (!handler)
    return;

  DirEntityNode* rootEntity = dh_get_entries(handler, handler->bootEntry->BPB_RootClus);
  DirEntityNode* curNode = rootEntity;

  long total = 0;
  while (curNode) {
    for (int i = 0; i < handler->clusterBytes / sizeof(DirEntry); ++i) {
      DirEntry* curDir = curNode->entry + i;

      char ch = curDir->DIR_Name[0];
      if (ch == SLOT_EMPTY) {
        break;
      }
      if (ch == SLOT_DELETED || ch == SLOT_NEXT_VALID || ch == ' ') {
        continue;
      }
      if (curDir->DIR_Attr == SLOT_LONG_ENTRY) { // long entry
        continue;
      }
      else {
        char buf[12];
        format_entry_name(curDir, buf);

        total += 1;
        if (curDir->DIR_Attr == ATTR_SUBDIRECTORY) {
          printf("%s/ (starting cluster = %d)\n", buf, de_get_file_start_cluster(curDir));
        }
        else {
          printf("%s (size = %d, starting cluster = %d)\n", buf, curDir->DIR_FileSize, de_get_file_start_cluster(curDir));
        }
      }
    }

    curNode = curNode->next;
  }
  printf("Total number of entries = %d\n", total);
}

int main(int argc, char* argv[])
{
  Input* input = parse_input(argc, argv);
  if (!input)
  {
    print_usage();
    return 1;
  }

  DiskHandler* disk = dh_create(input->disk);
  if (!disk)
  {
    print_usage();
    destroy_input(&input);
    return 1;
  }

  if (input->op == OP_PRINT_INFO)
  {
    print_file_system_info(disk);
  }
  else if (input->op == OP_LIST_DIR)
  {
    print_root_dir(disk);
  }
  else if (input->op == OP_RECOVER_CONTIGUOUS || input->op == OP_RECOVER_NON_CONTIGUOUS)
  {
    RecoverCommand* rc = rc_create(input->filename, input->sha1de, input->op == OP_RECOVER_CONTIGUOUS);
    rc_set_disk(rc, disk);

    RecoverResult res = rc_recover(rc);

    switch (res)
    {
    case RR_AMBIGUOUS:
    {
      printf("%s: multiple candidates found\n", rc->filename);
      break;
    }
    case RR_NOT_FOUND: {
      printf("%s: file not found\n", rc->filename);
      break;
    }
    case RR_SUCCESS: {
      if (rc->sha1)
        printf("%s: successfully recovered with SHA-1\n", rc->filename);
      else
        printf("%s: successfully recovered\n", rc->filename);
      break;
    }
    default:
      printf("???");
    }

    rc_destroy(rc);
  }

  dh_destroy_disk(disk);
  destroy_input(&input);

  return 0;
}