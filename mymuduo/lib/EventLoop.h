#pragma once 

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "Stamptimer.h"
#include "CurrentThread.h"

class Channel;
class Poller;

class EventLoop : noncopyable 
{
public:
    using Functor = std::function<void()>;
    
    EventLoop();
    ~EventLoop();

    //开启事件循环
    void loop();
    //退出事件循环
    void quit();

    //当前loop中执行cb
    void runInLoop(Functor cb);
    //把cb放入队列中，唤醒loop所在线程,执行cb
    void queueInLoop(Functor cb);

    //用来唤醒loop所在的线程
    void wakeup();

    //Channel -> EventLoop -> Poller
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    //判断EventLoop对象是否在自己的线程里面
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid(); }

    Stamptimer pollReturnTime() const {}
private:
    void handleRead(); // wake up
    void doPendingFunctors(); //do callback

    using ChannelList = std::vector<Channel*>;
    
    std::atomic_bool looping_; //原子操作
    std::atomic_bool quit_;//标记退出loop循环

    const pid_t threadId_;//记录当前loop的线程id

    Stamptimer pollReturnTime_;//poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_; //主要作用，当mainLoop获取一个新用户的channel，通过轮询算法选择一个subloop，通过该成员唤醒subloop处理channel
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    std::atomic_bool callingPendingFunctors_; //标准当前loop是否在处理回调的过程中
    std::vector<Functor> pendingFunctors_; //存储loop所要执行的callback操作
    std::mutex mutex_; //用来保护上面的vector容器

};