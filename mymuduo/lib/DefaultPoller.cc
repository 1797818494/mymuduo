#include "Poller.h"
#include "EPollPolller.h"

#include<stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop* loop) {
    if(::getenv("MUDUO_USE_POLL")) {
        return nullptr;
    }else{
        return new EPollPoller(loop);
    }
}