#ifndef __NYU_UTILS__
#define __NYU_UTILS__

unsigned int hexToInt(char ch);

unsigned int charToByte(char* chs);

char* hexToBytes(char* hex, unsigned int len);

char* bytesToHex(char* bytes, unsigned int len);

void format_entry_name(const DirEntry* entry, char* buf);

void swap(long* a, long* b);

unsigned long factorial(long n);

unsigned long calculate_combination(long m, long n);

typedef struct PermutationGenerator
{
  long* pool;
  long poolSize;

  long choose;

  long** result;
  long resultSize;

  long current;
} PermutationGenerator;

void pg_init(PermutationGenerator* pg, long* p, long ps, long c);

void pg_generate(PermutationGenerator* pg);

long* pg_next(PermutationGenerator* pg);

void pg_reset(PermutationGenerator* pg);

void pg_destroy(PermutationGenerator* pg);

#define SHA_DIGEST_LENGTH 20

#endif