
#ifndef __logger_h__
#define __logger_h__

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>

#define LOG_DEBUG (0x1 << 0)
#define LOG_ERROR (0x1 << 1)
#define LOG_WARNING (0x1 << 2)
#define LOG_INFO (0x1 << 3)
#define LOG_COLOR (0x1 << 4)

#define exeName "REST-2.0"

extern int EnabledLog;
extern FILE* log_output;

const char*
timestamp();

void
InitializeLog();

// next line make log more "clean"
//#define BASIC_LOG

#define COLOR_TERM_ON

#ifdef COLOR_TERM_ON
#define COLOR_START "\33["
#define COLOR_RED "31;49m"    /*RED*/
#define COLOR_GREEN "32;49m"  /*GREEN*/
#define COLOR_YELLOW "33;49m" /*YELLOW*/
#define COLOR_BLUE "34;49m"   /*BLUE*/
#define COLOR_PURPLE "35;49m" /*PURPLE*/
#define COLOR_CYAN "36;49m"   /*CYAN*/
#define COLOR_END "\33[m"
#define COLOR_CURRENT "31;40m" /*RED*/
#else
#define COLOR_START
#define COLOR_RED
#define COLOR_GREEN
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_PURPLE
#define COLOR_CYAN
#define COLOR_END
#define COLOR_CURRENT
#endif

#define clean_errno (errno == 0 ? "None" : strerror(errno))

#ifdef BASIC_LOG

#define _log_debug_(M, ...)                                                    \
  if (EnabledLog & LOG_DEBUG)                                                  \
  fprintf(log_output, M, ##__VA_ARGS__), fflush(log_output)

#define log_debug(M, ...)                                                      \
  if (EnabledLog & LOG_DEBUG)                                                  \
  fprintf(log_output, "\n" M, ##__VA_ARGS__), fflush(log_output)

#define log_error(M, ...)                                                      \
  if (EnabledLog & LOG_ERROR)                                                  \
  fprintf(log_output, "\n" M, ##__VA_ARGS__), fflush(log_output)

#define log_warning(M, ...)                                                    \
  if (EnabledLog & LOG_WARNING)                                                \
  fprintf(log_output, "\n" M, ##__VA_ARGS__), fflush(log_output)

#define log_info(M, ...)                                                       \
  if (EnabledLog & LOG_INFO)                                                   \
  fprintf(log_output, "\n" M, ##__VA_ARGS__), fflush(log_output)
#else

#define _log_debug_(M, ...)                                                    \
  if (EnabledLog & LOG_DEBUG)                                                  \
  fprintf(log_output, M, ##__VA_ARGS__), fflush(log_output)

#define log_debug(M, ...)                                                      \
  if (EnabledLog & LOG_DEBUG) {                                                \
    if (EnabledLog & LOG_COLOR) {                                              \
      fprintf(log_output,                                                      \
              "\n%s:" COLOR_START COLOR_GREEN "D" COLOR_END                    \
              ":" COLOR_START COLOR_CURRENT "%s" COLOR_END                     \
              ":" COLOR_START COLOR_CYAN "%16s" COLOR_END                      \
              ":" COLOR_START COLOR_CYAN "%5d" COLOR_END                       \
              ":%s[" COLOR_START COLOR_GREEN M COLOR_END "]",                  \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    } else {                                                                   \
      fprintf(log_output,                                                      \
              "\n%s:D:%s:%16s:%5d:%s[" M "]",                                  \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    }                                                                          \
  }

#define log_error(M, ...)                                                      \
  if (EnabledLog & LOG_ERROR) {                                                \
    if (EnabledLog & LOG_COLOR) {                                              \
      fprintf(log_output,                                                      \
              "\n%s:" COLOR_START COLOR_RED "E" COLOR_END                      \
              ":" COLOR_START COLOR_CURRENT "%s" COLOR_END                     \
              ":" COLOR_START COLOR_CYAN "%16s" COLOR_END                      \
              ":" COLOR_START COLOR_CYAN "%5d" COLOR_END                       \
              ":%s:" COLOR_START COLOR_RED "%d|%s" COLOR_END                   \
              ":[" COLOR_START COLOR_RED M COLOR_END "]",                      \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              errno,                                                           \
              clean_errno,                                                     \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    } else {                                                                   \
      fprintf(log_output,                                                      \
              "\n%s:E:%s:%16s:%5d:%s:%d|%s:[" M "]",                           \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              errno,                                                           \
              clean_errno,                                                     \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    }                                                                          \
  }
#define log_warning(M, ...)                                                    \
  if (EnabledLog & LOG_WARNING) {                                              \
    if (EnabledLog & LOG_COLOR) {                                              \
      fprintf(log_output,                                                      \
              "\n%s:" COLOR_START COLOR_YELLOW "W" COLOR_END                   \
              ":" COLOR_START COLOR_CURRENT "%s" COLOR_END                     \
              ":" COLOR_START COLOR_CYAN "%16s" COLOR_END                      \
              ":" COLOR_START COLOR_CYAN "%5d" COLOR_END                       \
              ":%s:" COLOR_START COLOR_YELLOW "%d|%s" COLOR_END                \
              ":[" COLOR_START COLOR_YELLOW M COLOR_END "]",                   \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              errno,                                                           \
              clean_errno,                                                     \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    } else {                                                                   \
      fprintf(log_output,                                                      \
              "\n%s:W:%s:%16s:%5d:%s:%d|%s:[" M "]",                           \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              errno,                                                           \
              clean_errno,                                                     \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    }                                                                          \
  }
#define log_info(M, ...)                                                       \
  if (EnabledLog & LOG_INFO) {                                                 \
    if (EnabledLog & LOG_COLOR) {                                              \
      fprintf(log_output,                                                      \
              "\n%s:" COLOR_START COLOR_CYAN "I" COLOR_END                     \
              ":" COLOR_START COLOR_CURRENT "%s" COLOR_END                     \
              ":" COLOR_START COLOR_CYAN "%16s" COLOR_END                      \
              ":" COLOR_START COLOR_CYAN "%5d" COLOR_END                       \
              ":%s:[" COLOR_START COLOR_CYAN M COLOR_END "]",                  \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    } else {                                                                   \
      fprintf(log_output,                                                      \
              "\n%s:I:%s:%16s:%5d:%s:[" M "]",                                 \
              timestamp(),                                                     \
              exeName,                                                         \
              __FILE__,                                                        \
              __LINE__,                                                        \
              __FUNCTION__,                                                    \
              ##__VA_ARGS__);                                                  \
      fflush(log_output);                                                      \
    }                                                                          \
  }

#endif

#endif
