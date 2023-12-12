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

DiskHandler *dh_open_disk(char *path)
{
    int fd = open(path, O_RDWR);
    if (fd == -1)
    {
        printf("Failed to open disk.\n");
        return 0;
    }

    DiskHandler *handler = malloc(sizeof(DiskHandler));

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

    BootEntry *bootEntry = get_disk_head(handler);
    handler->bootEntry = bootEntry;
    handler->sectorBytes = bootEntry->BPB_BytsPerSec;
    handler->clusterBytes = handler->sectorBytes * bootEntry->BPB_SecPerClus;
    handler->fatAreaByteOffset = handler->sectorBytes * bootEntry->BPB_RsvdSecCnt;
    handler->dataAreaByteOffset = handler->fatAreaByteOffset + bootEntry->BPB_NumFATs * bootEntry->BPB_FATSz32 * handler->sectorBytes;
    handler->rootByteOffset = handler->dataAreaByteOffset + handler->clusterBytes * (bootEntry->BPB_RootClus - 2);

    handler->rootEntry = (DirEntry*)(handler->data + handler->rootByteOffset);

    return handler;
}

BootEntry *dh_get_disk_head(DiskHandler *handler)
{
    return (BootEntry *)(handler->data);
}

long dh_get_cluster_offset(DiskHandler *handler, long cluster)
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
    rootNode->entry = handler->data + dh_get_offset(handler, cluster);

    DirEntityNode* node = rootNode;
    long nextCluster = dh_get_next_cluster(handler, cluster);
    while (nextCluster != CLUSTER_END) {
        node->next = malloc(sizeof(DirEntityNode));
        node = node->next;

        node->next = 0;
        node->entry = handler->data + dh_get_offset(handler, nextCluster);

        nextCluster = dh_get_next_cluster(handler, cluster);
    }

    return rootNode;
}

