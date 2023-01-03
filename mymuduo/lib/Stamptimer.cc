#include "Stamptimer.h"
#include <time.h>
Stamptimer::Stamptimer() : microSecondSinceEpoch_(0){}
Stamptimer::Stamptimer(int64_t microSecondSinceEpoch) 
: microSecondSinceEpoch_(microSecondSinceEpoch){

}
Stamptimer Stamptimer::now() {
    return Stamptimer(time(NULL));
}
std::string Stamptimer::to_String() const {
    struct tm* mytm = localtime(&microSecondSinceEpoch_);
    char buf[128] = {0};
    snprintf(buf, 128, "%04d/%02d/%02d %02d:%02d", 
    mytm->tm_year + 1900, mytm->tm_mon + 1, mytm->tm_mday,
    mytm->tm_hour, mytm->tm_min);
    return buf;
}