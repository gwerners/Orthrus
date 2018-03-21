/*
Copyright 2018 Gabriel Wernersbach Farinas

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "logger.h"
#include "stdio.h"

#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

int EnabledLog = LOG_COLOR | LOG_INFO | LOG_WARNING | LOG_ERROR; // LOG_DEBUG
FILE* log_output = NULL;

const char*
timestamp()
{
  static char timestamp[40];
  time_t ltime;
  // ltime=time(NULL);
  struct tm _tm, *tm;
  struct timeb tmb;

  /*    long            ms; // Milliseconds
      time_t          s;  // Seconds
      struct timespec spec;

      clock_gettime(CLOCK_REALTIME, &spec);

      s  = spec.tv_sec;
      ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
  */
  ftime(&tmb);
  ltime = tmb.time;
  tm = localtime_r(&ltime, &_tm);

  sprintf((char*)&timestamp,
          "%04d%02d%02d%02d%02d%02d%03d",
          tm->tm_year + 1900,
          tm->tm_mon + 1,
          tm->tm_mday,
          tm->tm_hour,
          tm->tm_min,
          tm->tm_sec,
          tmb.millitm);
  return (const char*)&timestamp;
}

void
InitializeLog()
{
  log_output = stdout;
}
