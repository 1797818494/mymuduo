#include "Logger.h"
#include "Stamptimer.h"
Logger& Logger::create() {
    static Logger LoggerInstance;
    return LoggerInstance;
}
void Logger::Log(std::string msg) {
    switch (LoggerLevel_)
    {
    case DEBUG:
        std::cout<<"DEBUG";
        break;
    case FATAL:
        std::cout<<"FATAL";
        break;
    case ERROR:
        std::cout<<"ERROR";
        break;
    case INFO:
        std::cout<<"INFO";
        break;
    
    default:
        break;
    };
    /**
     * 
     * this part will output the message of time
     * 
     */
    std::cout<<Stamptimer::now().to_String()<<": "<<msg<<std::endl;
}

