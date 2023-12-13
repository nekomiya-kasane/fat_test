#ifndef __NYU_DISK__
#define __NYU_DISK__

#include <sys/stat.h>

#include "nyufat32.h"

typedef struct stat DiskStat;

typedef struct
{
  int fd;

  char* data;
  int size;

  DiskStat stat;

  /* entries */
  BootEntry* bootEntry;

  /* details */
  long sectorBytes;
  long clusterBytes;
  long fatAreaByteOffset;
  long dataAreaByteOffset;
  long rootByteOffset;
} DiskHandler;

typedef struct DirEntityNode
{
  DirEntry* entry;
  struct DirEntityNode* next;
} DirEntityNode;

DiskHandler* dh_open_disk(const char* path);

BootEntry* dh_get_disk_head(DiskHandler* handler);

long dh_get_cluster_offset(DiskHandler* handler, long cluster);

long dh_get_next_cluster(DiskHandler* handler, long cluster);

DirEntityNode* dh_get_entries(DiskHandler* handler, long cluster);

unsigned* dh_get_cluster_record(DiskHandler* handler, long cluster);

void dh_get_deleted_clusters(DiskHandler* handler, long** clusters, long* count, long except);

void node_destroy(DirEntityNode* node);

long de_get_file_start_cluster(DirEntry* entry);


#endif