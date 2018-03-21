
#include "logger.h"
#include "stdio.h"

#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

int EnabledLog = LOG_INFO | LOG_DEBUG | LOG_WARNING | LOG_ERROR;
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

const char* banner =
  "   ▒█████   ██▀███  ▄▄▄█████▓ ██░ ██  ██▀███   █    ██   ██████ \n"
  "  ▒██▒  ██▒▓██ ▒ ██▒▓  ██▒ ▓▒▓██░ ██▒▓██ ▒ ██▒ ██  ▓██▒▒██    ▒ \n"
  "  ▒██░  ██▒▓██ ░▄█ ▒▒ ▓██░ ▒░▒██▀▀██░▓██ ░▄█ ▒▓██  ▒██░░ ▓██▄   \n"
  "  ▒██   ██░▒██▀▀█▄  ░ ▓██▓ ░ ░▓█ ░██ ▒██▀▀█▄  ▓▓█  ░██░  ▒   ██▒\n"
  "  ░ ████▓▒░░██▓ ▒██▒  ▒██▒ ░ ░▓█▒░██▓░██▓ ▒██▒▒▒█████▓ ▒██████▒▒\n"
  "  ░ ▒░▒░▒░ ░ ▒▓ ░▒▓░  ▒ ░░    ▒ ░░▒░▒░ ▒▓ ░▒▓░░▒▓▒ ▒ ▒ ▒ ▒▓▒ ▒ ░\n"
  "    ░ ▒ ▒░   ░▒ ░ ▒░    ░     ▒ ░▒░ ░  ░▒ ░ ▒░░░▒░ ░ ░ ░ ░▒  ░ ░\n"
  "  ░ ░ ░ ▒    ░░   ░   ░       ░  ░░ ░  ░░   ░  ░░░ ░ ░ ░  ░  ░  \n"
  "      ░ ░     ░               ░  ░  ░   ░        ░           ░  \n";

void
InitializeLog(FILE* out)
{
  log_output = out;
  fwrite(banner, sizeof(char), strlen(banner), log_output);
}
