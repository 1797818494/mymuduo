#include "Logger.h"
#include "Stamptimer.h"
// 单例模式
Logger &Logger::create() {
  pthread_once(&pthread_once_, &Logger::init);
  return *logger_;
}

void Logger::init() { logger_ = new Logger(); }
void Logger::Log(std::string msg) {
  switch (LoggerLevel_) {
    case DEBUG:
      std::cout << "DEBUG";
      break;
    case FATAL:
      std::cout << "FATAL";
      break;
    case ERROR:
      std::cout << "ERROR";
      break;
    case INFO:
      std::cout << "INFO";
      break;

    default:
      break;
  };
  std::cout << Stamptimer::now().to_String() << ": " << msg << std::endl;
}

Logger *Logger::logger_ = NULL;
pthread_once_t Logger::pthread_once_ = PTHREAD_ONCE_INIT;
