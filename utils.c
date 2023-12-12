#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "utils.h"

void disableBuffer()
{
    struct termios mode;

    tcgetattr(0, &mode);
    mode.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &mode);
}

void enableBuffer()
{
    struct termios mode;

    tcgetattr(0, &mode);
    mode.c_lflag |= (ECHO | ICANON);
    tcsetattr(0, TCSANOW, &mode);
}

void newSleep(float sec)
{
    struct timespec ts;

    if (sec < 0)
    {
        return;
    }

    ts.tv_sec = (int)sec;
    ts.tv_nsec = (sec - ((int)sec)) * 1000000000;
    nanosleep(&ts, NULL);
}

/*call this function just ONCE at the beginning of your program before using the random number generator */
void initRandom()
{
    srand(time(NULL));
}

/* Will return random integer between *low* and *high* inclusive.
   If the low is larger than the high, it will return -1 instead.
   In theory, it still works with negative *low* and *high* (as long as low is not larger than high).
   Please ensure you call initRandom function once before starting using this function. */
int randomUCP(int low, int high)
{
    int number = -1;

    if (low <= high)
    {
        number = (rand() % (high - low + 1)) + low;
    }

    return number;
}
