#include <sys/time.h>


unsigned long long gettime(void)
{
    int retval;
    struct timeval tp;
    unsigned long long ctime;

    retval = gettimeofday(&tp, NULL);
    ctime = tp.tv_sec;
    ctime *= 1000000;
    ctime += tp.tv_usec;

    return ctime;
}
