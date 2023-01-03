#pragma once

#include "Poller.h"
#include "Stamptimer.h"

#include<vector>
#include<sys/epoll.h>


/**
 * epoll_create and epoll_ctl(add / mod / del) 
 * and epoll_wait
*/

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    //继承接口， 也就是基类Poller的抽象方法
    Stamptimer poll(int timeoutMs, ChannelList* activeChannelList) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
private:
    static const int kInitEventListSize = 16;
    //填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList* activeChannel) const;
    void update(int operation, Channel* channel);
    using EventList = std::vector<epoll_event>;
    int epollfd_;
    EventList events_;
};