#ifndef __NYU_DISK__
#define __NYU_DISK__

#include <bits/stat.h>

#include "nyufat32.h"

typedef struct stat DiskStat;

typedef struct DiskHandler
{
    int fd;

    char *data;
    int size;

    DiskStat stat;

    /* entries */
    BootEntry *bootEntry;
    DirEntry *rootEntry;

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
    DirEntityNode* next;
} DirEntityNode;

DiskHandler *dh_open_disk();

BootEntry *dh_get_disk_head(DiskHandler *handler);

long dh_get_cluster_offset(DiskHandler *handler, long cluster);

long dh_get_next_cluster(DiskHandler* handler, long cluster);

DirEntityNode* dh_get_entries(DiskHandler* handler, long cluster);

#define CLUSTER_END 0x0FFFFFFF
#define CLUSTER_BAD 0x0FFFFFF7
#define CLUSTER_MAX 0x0FFFFFEF
#define CLUSTER_MIN 0x00000002
#define CLUSTER_RESERVED 0x00000001
#define CLUSTER_IDLE 0x00000000

#endif