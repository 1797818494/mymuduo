#pragma once

#include "noncopyable.h"
#include "Stamptimer.h"

#include<vector>
#include<unordered_map>
class Channel;
class EventLoop;
//IO 复用核心模块, 上层依赖的底层抽象
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    //所有IO复用保留统一的接口
    virtual Stamptimer poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    //判断该channel是否在Poller当中
    bool hasChannel(Channel* channel) const;
    //EventLoop 可以通过该接口获得默认的IO复用实现
    static Poller* newDefaultPoller(EventLoop* loop);

protected:
    //Map的key: sockfd value sockfd所在的Channel
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    //定义Poller所属的时间循环
    EventLoop* ownerLoop_;

};
