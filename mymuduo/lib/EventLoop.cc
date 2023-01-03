#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory>

//防止一个线程创建多个EventLoop

__thread EventLoop* t_loopInThisThread = nullptr;

//定义默认的Poller IO复用接口的超时时间
const int kPollTimeMs = 10000;

int createEventfd() {
    int evtfd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(evtfd < 0) {
        LOG_FATAL("eventfd error:%d \n", errno);
    }
    return evtfd;
}

EventLoop::EventLoop() 
 :looping_(false), quit_(false), 
 callingPendingFunctors_(false),
 threadId_(CurrentThread::tid()),
 poller_(Poller::newDefaultPoller(this)),
 wakeupFd_(createEventfd()),
 wakeupChannel_(new Channel(this, wakeupFd_)){
    LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    if(t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
    }else{
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();

}


EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

// 开启事件循环
void EventLoop::loop() {
    looping_ = true;
    quit_ = false;
    LOG_INFO("EventLoop %p start looping \n", this);

    while(!quit_) {
        activeChannels_.clear();
        //监听两种fd 一种是client的fd, 一种wakeupfd
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

        for(Channel* Channel : activeChannels_) {
            // Poller监听哪些channel发生事件了，然后上报给EventLoop，通知channel处理相应的事件
            Channel->handlerEvent(pollReturnTime_);

        }
        
        doPendingFunctors();
    }
    LOG_INFO("EventLoop %p stop looping. \n", this);
    looping_ = false;

}

// 退出事件循环  1.loop在自己的线程中调用quit  2.在非loop的线程中，调用loop的quit
/**
 *              mainLoop
 * 
 *                                             no ==================== 生产者-消费者的线程安全的队列
 * 
 *  subLoop1     subLoop2     subLoop3
 */ 
void EventLoop::quit() {
    quit_ = true;

    // 如果是在其它线程中，调用的quit   在一个subloop(woker)中，调用了mainLoop(IO)的quit
    if(!isInLoopThread()) {
        wakeup();
    }
}

//在当前loop中执行cb
void EventLoop::runInLoop(Functor cb) {
    if(isInLoopThread()) {
        cb();
    }else{
        queueInLoop(cb);
    }
}
//把cb放入队列中，唤醒loop所在线程。执行cb
void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_) {
        wakeup(); //唤醒loop所在线程
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if(n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8", n);
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)) 
    {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
    }
}

void EventLoop::updateChannel(Channel* channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}


void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(const Functor& functor : functors) {
        functor();
    }

    callingPendingFunctors_ = false;
}
