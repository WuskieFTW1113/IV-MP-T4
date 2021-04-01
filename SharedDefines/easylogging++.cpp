#include "easylogging++.h"
#include <exporting.h>

char loggerBuffer[2000];

#if linux
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>


typedef void (*log_LockFn)(void *udata, int lock);

static struct {
	void *udata;
	log_LockFn lock;
	FILE *fp;
	int level;
	int quiet;
} L;

static void lock(void)   {
	if(L.lock) {
		L.lock(L.udata, 1);
	}
}


static void unlock(void) {
	if(L.lock) {
		L.lock(L.udata, 0);
	}
}

void log_set_fp() {
	FILE *file_pointer;
	file_pointer = fopen("server.log", "w");
	if(file_pointer) {
		L.fp = file_pointer;
	}
	else printf("Log didnt open\n");
}

void mlog(const char *fmt, ...) {
	/* Acquire lock */
	lock();

	/* Get current time */
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	char buf[32];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';

	va_list arg_list;
	va_start(arg_list, fmt);

	vsnprintf(loggerBuffer, 2000, fmt, arg_list);

	va_end(arg_list);

	fprintf(stderr, "%s: %s\n", buf, loggerBuffer);
	fflush(stderr);

	if(L.fp) {
		fprintf(L.fp, "%s: %s\n", buf, loggerBuffer);
		fflush(L.fp);
	}

	/* Release lock */
	unlock();
}
#else
#include "truelog.h"
_INITIALIZE_EASYLOGGINGPP

void log_set_fp() {
	//do nothing
}

void mlog(const char *fmt, ...)
{
	va_list arg_list;
	va_start(arg_list, fmt);
	vsnprintf(loggerBuffer, 2000, fmt, arg_list);
	va_end(arg_list);

	LINFO << loggerBuffer;
}
#endif