﻿#include "nyurecover.h"
#include "nyudisk.h"
#include "nyuutils.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

RecoverCommand* rc_create(const char* filename, const char* sha1, int isContiguous)
{
  RecoverCommand* rc = malloc(sizeof(RecoverCommand));
  rc->disk = 0;
  rc->filename = filename;
  rc->sha1 = sha1;
  rc->isContiguous = isContiguous;
  return rc;
}

void rc_destroy(RecoverCommand* rc)
{
  if (rc)
    free(rc);
}

void rc_set_disk(RecoverCommand* rc, DiskHandler* handler)
{
  if (rc)
    rc->disk = handler;
}

int rc_find_entry(RecoverCommand* rc, DirEntry*** out, long* count, long maxSize)
{
  DirEntityNode* rootEntity = dh_get_entries(rc->disk, rc->disk->bootEntry->BPB_RootClus);
  DirEntityNode* curNode = rootEntity;

  while (curNode) {
    for (int i = 0; i < rc->disk->clusterBytes / sizeof(DirEntry); ++i) {
      DirEntry* curDir = curNode->entry + i;

      char ch = curDir->DIR_Name[0];
      if (ch == SLOT_EMPTY) {
        break;
      }
      if (ch != SLOT_DELETED)
        continue;

      //if (curDir->DIR_FileSize > maxSize)
      //  continue;

      char buf[12];
      format_entry_name(curDir, buf);

      if (strcmp(&buf[1], &(rc->filename[1]))) { // not the same file
        continue;
      }

      if (*count == 0) {
        if (*out)
          free(*out);
        *out = malloc(sizeof(DirEntry*));
      }
      else {
        *out = realloc(*out, sizeof(DirEntry*) * (*count + 1));
      }
      (*out)[*count] = curDir;
      (*count)++;
    }
    curNode = curNode->next;
  }
  de_node_destroy(rootEntity);
  return *count;
}

RecoverResult rc_find_right_entry_contiguous(RecoverCommand* rc, DirEntry** entries, long count, DirEntry** out) {
  *out = 0;
  if (!rc->sha1) {
    if (count != 1) {
      return RR_AMBIGUOUS;
    }
    else {
      *out = entries[0];
      return RR_SUCCESS;
    }
  }
  else {
    for (int i = 0; i < count; ++i) {
      DirEntry* entry = entries[i];

      long curCluster = de_get_file_start_cluster(entry);
      long countClusters = (int)ceil((double)entry->DIR_FileSize / rc->disk->clusterBytes);
      char* mem = malloc(countClusters * rc->disk->clusterBytes);
      char* curMem = mem;
      for (int j = 0; j < countClusters; ++j) {
        char* fileCluster = rc->disk->data + dh_get_cluster_offset(rc->disk, curCluster);
        memcpy(curMem, fileCluster, rc->disk->clusterBytes);
        curMem += rc->disk->clusterBytes;
        curCluster++;
      }

      char sha[SHA_DIGEST_LENGTH];
      SHA1(mem, entry->DIR_FileSize, sha);
      free(mem);

      if (strncmp(sha, rc->sha1, SHA_DIGEST_LENGTH) == 0) {
        if (!*out) {
          *out = entry;
        }
        else {
          return RR_AMBIGUOUS;
        }
      }
    }

    if (!*out) {
      return RR_NOT_FOUND;
    }
  }
  return RR_SUCCESS;
}

RecoverResult rc_find_right_entry_noncontiguous(RecoverCommand* rc, DirEntry** entries, long count, DirEntry** out, long* clusters, long* clusterCount)
{
  if (!rc->sha1)
  {
    return RR_NOT_FOUND;
  }

  PermutationGenerator pgs[5];
  int pgInitialized[5] = { 0, 0, 0, 0 ,0 };

  // get deleted clusters
  long* delClusters = 0;
  long delCount = 0;
  dh_get_deleted_clusters(rc->disk, &delClusters, &delCount, -1);

  // for every possible file
  *out = 0;
  RecoverResult res = RR_UNKNOWN;
  for (int i = 0; i < count && res == RR_UNKNOWN; ++i) {
    DirEntry* entry = entries[i];

    // cluster count needed
    *clusterCount = (long)ceil((double)entry->DIR_FileSize / rc->disk->clusterBytes);

    if (pgInitialized[*clusterCount - 1] == 0) {
      pg_init(&pgs[*clusterCount - 1], delClusters, delCount, *clusterCount);
      pgInitialized[*clusterCount - 1] = 1;
    }

    pg_reset(&pgs[*clusterCount - 1]);
    pg_generate(&pgs[*clusterCount - 1]);

    // for each combination
    long* curComb = 0;
    while (curComb = pg_next(&pgs[*clusterCount - 1])) {
      if (curComb[0] != de_get_file_start_cluster(entry))
        continue;

      // concat file
      char* mem = malloc(rc->disk->clusterBytes * *clusterCount);
      char* curMem = mem;
      for (int j = 0; j < *clusterCount; ++j) {
        long clu = curComb[j];

        char* fileCluster = rc->disk->data + dh_get_cluster_offset(rc->disk, clu);

        memcpy(curMem, fileCluster, rc->disk->clusterBytes);
        curMem += rc->disk->clusterBytes;
      }

      // check sha1
      char sha[SHA_DIGEST_LENGTH];
      SHA1(mem, entry->DIR_FileSize, sha);
      free(mem);

      if (strncmp(sha, rc->sha1, SHA_DIGEST_LENGTH) == 0) {
        if (!*out) {
          *out = entry;
          res = RR_SUCCESS;
          break;
        }
        else { // cal
          res = RR_AMBIGUOUS;
        }
      }
    }
  }

  if (!*out) {
    return RR_NOT_FOUND;
  }
  else {
    res = RR_SUCCESS;
  }

  for (int i = 0; i < 5; ++i) {
    if (pgInitialized[i])
      pg_destroy(&pgs[i]);
  }
  free(delClusters);
  return res;
}

RecoverResult rc_recover(RecoverCommand* rc) {
  DirEntry** entries = 0;
  long count = 0;
  if (!rc_find_entry(rc, &entries, &count, rc->isContiguous ? rc->disk->clusterBytes : LONG_MAX) || !count)
    return RR_NOT_FOUND;

  if (rc->isContiguous) {
    DirEntry* out = 0;
    RecoverResult res = rc_find_right_entry_contiguous(rc, entries, count, &out);

    if (res != RR_SUCCESS)
      return res;

    // recover name
    out->DIR_Name[0] = rc->filename[0];

    // recover fat record
    if (out->DIR_FileSize) {
      for (int i = 0; i < 2; ++i) {
        long curCluster = de_get_file_start_cluster(out);
        for (int j = 0; j < (int)ceil((double)out->DIR_FileSize / rc->disk->clusterBytes) - 1; ++j) {
          unsigned* fatRecord = dh_get_cluster_record(rc->disk, curCluster, i);
          *fatRecord = curCluster + 1;
          curCluster++;
        }
        unsigned* fatRecord = dh_get_cluster_record(rc->disk, curCluster, i);
        *fatRecord = CLUSTER_END;
      }
    }
    return res;
  }
  else {
    DirEntry* out = 0;
    long clusters[5], clusterCount = 0;
    RecoverResult res = rc_find_right_entry_noncontiguous(rc, entries, count, &out, clusters, &clusterCount);

    if (res != RR_SUCCESS)
      return res;

    // recover name
    out->DIR_Name[0] = rc->filename[0];

    // recover fat records
    unsigned* fatRecord = 0;
    for (int i = 0; i < clusterCount; ++i) {
      for (int j = 0; j < 2; ++j) {
        fatRecord = dh_get_cluster_record(rc->disk, clusters[i], j);
        if (i == clusterCount - 1)
          *fatRecord = CLUSTER_END;
        else
          *fatRecord = clusters[i + 1];
      }
    }
    return res;
  }
}
