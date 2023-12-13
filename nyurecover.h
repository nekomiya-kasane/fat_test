#ifndef __NYU_RECOVER__
#define __NYU_RECOVER__

#include "nyudisk.h"

#include <openssl/sha.h>
#define SHA_DIGEST_LENGTH 20

extern unsigned char* SHA1(const unsigned char* d, size_t n, unsigned char* md);

typedef enum RecoverResult {
  RR_NOT_FOUND,
  RR_AMBIGUOUS,
  RR_SUCCESS
} RecoverResult;

typedef struct RecoverCommand {
  DiskHandler* disk;

  const char* filename;
  const char* sha1;
  int isContiguous;
} RecoverCommand;

RecoverCommand* rc_create(const char* filename, const char* sha1, int isContiguous);

void rc_set_disk(RecoverCommand* rc, DiskHandler* handler);

int rc_find_entry(RecoverCommand* rc, DirEntry*** out, long* count, long maxSize);

enum RecoverResult rc_recover(RecoverCommand* rc);

void rc_destroy(RecoverCommand* rc);

#endif