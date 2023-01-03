#pragma once
#include "noncopyable.h"
#include "Stamptimer.h"
#include <functional>
#include <memory>
/**
 * 封装了fd， event和一些回调函数
*/
class EventLoop;
class Channel : noncopyable {
    public:
        using EventCallback = std::function<void()>;
        using ReadEventCallback = std::function<void(Stamptimer)>;
        Channel(EventLoop* loop, int fd);
        ~Channel() {}
        void handlerEvent(Stamptimer receiveTime);
        void setReadCallback(ReadEventCallback cb) {readCallback_ = std::move(cb);}
        void setWriteCallback(EventCallback cb) {writeCallback_  = std::move(cb); }
        void setCloseCallback(EventCallback cb) {closeCallback_  = std::move(cb); }
        void setErrorCallback(EventCallback cb) {errorCallback_  = std::move(cb); }
        //set fd's events
        void enableReading() {events_ |= kReadEvent; update();}
        void disableReading() {events_ &= ~kReadEvent; update();}
        void enableWriting() {events_ |= KWriteEvent; update();}
        void disableWriting() {events_ &= ~KWriteEvent; update();}
        void disableAll() {events_ = kNoneEvent; update();}
        bool isNoneEvent() const {return events_ == kNoneEvent;}
        bool isWriting() const {return events_ & KWriteEvent; }
        bool isReading() const {return events_ & kReadEvent; }
        int index() {return idx_; }
        void set_index(int idx) {idx_ = idx;}
        int events() const { return events_; }
        void set_revents(int revt) { revents_ = revt; } // used by pollers
        // one loop per thread
        EventLoop* ownerLoop() {return loop_;}
        void remove();
        void tie(const std::shared_ptr<void>&);
        int fd() const {return fd_; }

    private:
        void update();
        void handleEventWithGuard(Stamptimer receiveTime);
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int KWriteEvent;
        EventLoop *loop_; // 一个事件循环
        const int fd_;
        int events_;
        int revents_;
        int idx_;
        std::weak_ptr<void> tie_;
        bool tied_;
        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;
};