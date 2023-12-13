#include "nyudisk.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

DiskHandler* dh_create(const char* path)
{
  int fd = open(path, O_RDWR);
  if (fd == -1)
  {
    printf("Failed to open disk.\n");
    return 0;
  }

  DiskHandler* handler = malloc(sizeof(DiskHandler));

  fstat(fd, &handler->stat);
  handler->size = handler->stat.st_size;
  handler->data = mmap(NULL, handler->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (handler->data == MAP_FAILED)
  {
    printf("Failed to map disk.\n");
    free(handler);
    return 0;
  }

  handler->fd = fd;

  BootEntry* bootEntry = dh_get_disk_head(handler);
  handler->bootEntry = bootEntry;
  handler->sectorBytes = bootEntry->BPB_BytsPerSec;
  handler->clusterBytes = handler->sectorBytes * bootEntry->BPB_SecPerClus;
  handler->fatAreaByteOffset = handler->sectorBytes * bootEntry->BPB_RsvdSecCnt;
  handler->dataAreaByteOffset = handler->fatAreaByteOffset + bootEntry->BPB_NumFATs * bootEntry->BPB_FATSz32 * handler->sectorBytes;
  handler->rootByteOffset = handler->dataAreaByteOffset + handler->clusterBytes * (bootEntry->BPB_RootClus - 2);

  return handler;
}

BootEntry* dh_get_disk_head(DiskHandler* handler)
{
  return (BootEntry*)(handler->data);
}

long dh_get_cluster_offset(DiskHandler* handler, long cluster)
{
  return handler->dataAreaByteOffset + (cluster - 2) * handler->clusterBytes;
}

long dh_get_next_cluster(DiskHandler* handler, long cluster)
{
  return *(unsigned*)(handler->data + handler->fatAreaByteOffset + cluster * 4);
}

DirEntityNode* dh_get_entries(DiskHandler* handler, long cluster)
{
  char* dataAreaStart = handler->data + handler->dataAreaByteOffset;

  DirEntityNode* rootNode = malloc(sizeof(DirEntityNode));
  rootNode->next = 0;
  rootNode->entry = (DirEntry*)(handler->data + dh_get_cluster_offset(handler, cluster));

  DirEntityNode* node = rootNode;
  long nextCluster = dh_get_next_cluster(handler, cluster);
  while (nextCluster != CLUSTER_END) {
    node->next = malloc(sizeof(DirEntityNode));
    node = node->next;

    node->next = 0;
    node->entry = (DirEntry*)(handler->data + dh_get_cluster_offset(handler, nextCluster));

    nextCluster = dh_get_next_cluster(handler, nextCluster);
  }

  return rootNode;
}

void dh_get_deleted_clusters(DiskHandler* handler, long** clusters, long* count, long except)
{
  unsigned int* fatRecords = (unsigned int*)(handler->data + handler->fatAreaByteOffset);

  if (*clusters)
    free(*clusters);
  *clusters = 0;

  long cluster = 3;
  long index = 0;
  while (cluster < 23) {
    unsigned* fatEntry = fatRecords + cluster;
    if (*fatEntry == 0 && (except < 0 || *fatEntry != except)) {
      *clusters = *clusters ? malloc(sizeof(long)) : realloc(*clusters, sizeof(long) * (index + 1));
      (*clusters)[index] = cluster;
      index++;
    }
    cluster++;
  }
}

void de_node_destroy(DirEntityNode* node)
{
  if (!node)
    return;

  de_node_destroy(node->next);
  free(node);
}

long de_get_file_start_cluster(DirEntry* entry)
{
  return (entry->DIR_FstClusHI << 2) | (entry->DIR_FstClusLO);
}

unsigned* dh_get_cluster_record(DiskHandler* handler, long cluster, long fatId)
{
  return (unsigned*)(handler->data + handler->fatAreaByteOffset
    + handler->bootEntry->BPB_FATSz32 * handler->sectorBytes * fatId + cluster * 4);
}

void dh_destroy_disk(DiskHandler* disk)
{
  close(disk->fd);
  free(disk);
}