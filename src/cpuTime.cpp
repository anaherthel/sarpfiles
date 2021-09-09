#include <sys/time.h>
#include <sys/resource.h>

#include "cpuTime.h"

double cpuTime() {
	static struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return (((double)usage.ru_utime.tv_sec)+(((double)usage.ru_utime.tv_usec))/((double)1000000));
}
