#pragma once
#include <string>
#include "noncopyable.h"

enum LOG_LEVEL {
  INFO,
  FATAL,
  ERROR,
  DEBUG,
};
// TODO: why do while(0)
#define LOG_INFO(MESSAGE, ...)                  \
  do {                                          \
    Logger &instance = Logger::create();        \
    instance.setLevel(INFO);                    \
    char buf[128] = {0};                        \
    snprintf(buf, 128, MESSAGE, ##__VA_ARGS__); \
    instance.Log(buf);                          \
  } while (0)

#define LOG_FATAL(MESSAGE, ...)                 \
  do {                                          \
    Logger &instance = Logger::create();        \
    instance.setLevel(FATAL);                   \
    char buf[128] = {0};                        \
    snprintf(buf, 128, MESSAGE, ##__VA_ARGS__); \
    instance.Log(buf);                          \
    exit(-1);                                   \
  } while (0)
#define LOG_ERROR(MESSAGE, ...)                 \
  do {                                          \
    Logger &instance = Logger::create();        \
    instance.setLevel(ERROR);                   \
    char buf[128] = {0};                        \
    snprintf(buf, 128, MESSAGE, ##__VA_ARGS__); \
    instance.Log(buf);                          \
  } while (0)
/**
 * only if we are at debug mode, we can log the
 * message of debug
 */
#ifdef MUDEBUG
#define LOG_DEBUG(MESSAGE, ...)                 \
  do {                                          \
    Logger &instance = Logger::create();        \
    instance.setLevel(INFO);                    \
    char buf[128] = {0};                        \
    snprintf(buf, 128, MESSAGE, ##__VA_ARGS__); \
    instance.Log(buf);                          \
  } while (0)
#else
#define LOG_DEBUG(MESSAGE, ...)

#endif

class Logger : noncopyable {
 public:
  static Logger &create();
  void Log(std::string msg);
  void setLevel(int64_t Level) { LoggerLevel_ = Level; }

 private:
  static void init();
  int64_t LoggerLevel_;
  static Logger *logger_;
  static pthread_once_t pthread_once_;
};
