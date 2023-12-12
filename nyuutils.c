#include "nyuutils.h"

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

unsigned int charToByte(char *chs)
{
    unsigned int p1 = hexToInt(chs[0]), p2 = hexToInt(chs[1]);
    if (p1 == -1 || p2 == -1)
        return -1;

    return (p1 << 4) | p2;
}

char *hexToBytes(char *hex, unsigned int len)
{
    char *res = malloc(len / 2 + 1);
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

char *bytesToHex(char *bytes, unsigned int len)
{
    char *res = malloc(len * 2 + 1);
    memset(res, 0, len * 2 + 1);
    for (int i = 0; i < len; i++)
        sprintf(res + i * 2, "%02x", bytes[i]);
    return res;
}