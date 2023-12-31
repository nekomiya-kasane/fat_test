﻿#include "nyuutils.h"
#include "nyufat32.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

unsigned int hexToInt(char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  else if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  else
    return -1;
}

unsigned int charToByte(char* chs)
{
  unsigned int p1 = hexToInt(chs[0]), p2 = hexToInt(chs[1]);
  if (p1 == -1 || p2 == -1)
    return -1;

  return (p1 << 4) | p2;
}

char* hexToBytes(char* hex, unsigned int len)
{
  char* res = malloc(len / 2 + 1);
  memset(res, 0, len / 2 + 1);
  for (int i = 0; i < len / 2; i++) {
    unsigned int ch = charToByte(hex + i * 2);
    if (ch == -1) {
      free(res);
      return 0;
    }
    res[i] = ch;
  }
  return res;
}

char* bytesToHex(char* bytes, unsigned int len)
{
  char* res = malloc(len * 2 + 1);
  memset(res, 0, len * 2 + 1);
  for (int i = 0; i < len; i++)
    sprintf(res + i * 2, "%02x", bytes[i]);
  return res;
}

void format_entry_name(const DirEntry* entry, char* buf)
{
  int i = 0;
  while (entry->DIR_Name[i] != '\0' && entry->DIR_Name[i] != 0x20 && i < 8)
  {
    buf[i] = entry->DIR_Name[i];
    i++;
  }
  if (entry->DIR_Name[8] == ' ' || entry->DIR_Name[8] == 0x20)
  {
    buf[i] = '\0';
    return;
  }

  buf[i] = '.';
  i++;
  for (int j = 8; j < 11; j++)
  {
    if (entry->DIR_Name[j] == ' ' || entry->DIR_Name[j] == 0x20)
      break;
    buf[i] = entry->DIR_Name[j];
    i++;
  }
  buf[i] = '\0';
}

void swap(long* a, long* b) {
  long temp = *a;
  *a = *b;
  *b = temp;
}

long factorial(long n) {
  if (n == 0 || n == 1) {
    return 1;
  }
  else {
    return n * factorial(n - 1);
  }
}

long calculate_permutation(long m, long n)
{
  if (m < n) {
    return 0;
  }

  return factorial(m) / factorial(m - n);
}

long calculate_combination(long m, long n) {
  if (m < n) {
    return 0;
  }

  return factorial(m) / (factorial(n) * factorial(m - n));
}

void generate_permutations(long* a, long n, long k, long i, long** result, long* cur)
{
  if (i == 0)
  {
    for (int j = n; j < n + k; j++) {
      result[*cur][j - n] = a[j];
    }
    (*cur)++;
    return;
  }

  for (int j = 0; j < n; j++)
  {
    swap(&a[j], &a[n - 1]);
    generate_permutations(a, n - 1, k, i - 1, result, cur);
    swap(&a[j], &a[n - 1]);
  }
}

void pg_init(PermutationGenerator* pg, long* p, long ps, long c) {
  pg->pool = p;
  pg->poolSize = ps;
  pg->choose = c;

  long total = calculate_combination(ps, c) * factorial(c);
  pg->result = malloc(total * sizeof(long*));

  for (long i = 0; i < total; ++i) {
    pg->result[i] = malloc(sizeof(long) * c);
  }

  pg->resultSize = total;

  pg->current = -1;
}

void pg_generate(PermutationGenerator* pg) {
  long count = 0;
  long total = calculate_permutation(pg->poolSize, pg->choose);
  pg->result = malloc(total * sizeof(long*));
  for (long i = 0; i < total; i++) {
    pg->result[i] = malloc(sizeof(long) * pg->choose);
  }

  generate_permutations(pg->pool, pg->poolSize, pg->choose, pg->choose, pg->result, &count);
}

long* pg_next(PermutationGenerator* pg) {
  pg->current++;
  if (pg->current >= pg->resultSize) {
    return 0;
  }
  return pg->result[pg->current];
}

void pg_reset(PermutationGenerator* pg) {
  pg->current = -1;
}

void pg_destroy(PermutationGenerator* pg) {
  for (long i = 0; i < pg->resultSize; i++) {
    free(pg->result[i]);
  }
  free(pg->result);
}