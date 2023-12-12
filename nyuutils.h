#ifndef __NYU_UTILS__
#define __NYU_UTILS__

unsigned int hexToInt(char ch);

unsigned int charToByte(char *chs);

char *hexToBytes(char *hex, unsigned int len);

char *bytesToHex(char *bytes, unsigned int len);

#endif