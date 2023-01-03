#include "EPollPolller.h"
#include "Logger.h"
#include "Channel.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>

//channel还没有被添加poller
const int kNew = -1;
//channel已经被添加到poller中
const int kAdded = 1;
//channel从poller中被删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
: Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
events_(kInitEventListSize) {
    if(epollfd_ < 0) {
        LOG_FATAL("epoll_create err:%d\n", errno);
    }
}
EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}
Stamptimer EPollPoller::poll(int timeoutMs, ChannelList* activeChannelList) {
     LOG_INFO("func=%s => fd total count:%lu \n", __FUNCTION__, channels_.size());
     int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
     int saveErrno = errno;
     Stamptimer now(Stamptimer::now());
     if(numEvents > 0) {
        LOG_INFO("%d events happened\n", numEvents);
        fillActiveChannels(numEvents, activeChannelList);
        if(numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        }
     }else if(numEvents == 0) {
        LOG_DEBUG("%s timeout!\n",__FUNCTION__);

     }else{
        if(saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!");
        }
     }
     return now;
}

/**
 * EventLoop = ChannelList + Poller
 * Poller  has ChannelMap<fd, Channel*> and epollfd_
 * 
*/
void EPollPoller::updateChannel(Channel* channel) {
    const int index = channel->index();
    LOG_INFO("func=%s => fd=%d events=%d index=%d \n", __FUNCTION__, channel->fd(), channel->events(), index);
    if(index == kNew || index == kDeleted) {
        if(index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);

    }else{
        int fd = channel->fd();
        if(channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

//从poller中删除channel
void EPollPoller::removeChannel(Channel* channel) {
    int fd = channel->fd();
    channels_.erase(fd);
    LOG_INFO("func = %s=>fd=%d\n", __FUNCTION__, fd);
    int index = channel->index();
    if(index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

//填写活跃连接
void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannelList) const {
    for(int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannelList->push_back(channel);
    }
}

//更新channel通道 epoll_ctl 
void EPollPoller::update(int operation, Channel* channel) {
    epoll_event event;
    bzero(&event, sizeof event);
    int fd = channel->fd();
    event.events = channel->events();
    //event.data.fd = fd;
    event.data.ptr = channel;
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }
        else
        {
            LOG_FATAL("epoll_ctl add/mod error,:%d %d %d \n", errno, epollfd_, fd);
        }
    }
}



