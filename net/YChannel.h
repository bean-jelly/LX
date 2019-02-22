#ifndef LX_CHANNEL_H
#define LX_CHANNEL_H

#include <LX/base/YTimestamp.h>
#include <LX/base/YNoncopyable.h>
#include <functional>
#include <memory>

namespace LX
{
    namespace net
    {
        class EventLoop;

        class Channel : noncopyable
        {
        public:
            typedef std::function<void()> EventCallback;
            typedef std::function<void(Timestamp)> ReadEventCallback;

            Channel(EventLoop* loop, int fd);
            ~Channel();

            void handleEvent(Timestamp receiveTime);
            void setReadCallback(ReadEventCallback cb) {readCallback_ = std::move(cb);}
            void setWriteCallback(EventCallback cb) {writeCallback_ = std::move(cb);}
            void setCloseCallback(EventCallback cb) {closeCallback_ = std::move(cb);}
            void setErrorCallback(EventCallback cb) {errorCallback_ = std::move(cb);}

            //Tie this channel to the owner object managed by shared_ptr
            //prevent the owner object being destroyed in handleEvent
            void tie(const std::shared_ptr<void>&);

            int fd() const {return fd_;}
            int events() const {return events_;}
            void set_revents(int revt) {revents_ = revt;}
            bool isNoneEvent() const {return events_ == kNoneEvent;}

            void enableReading() {events_ |= kReadEvent; update();}
            void disableReading() {events_ &= ~kReadEvent; update();}
            void enableWriting() {events_ |= kWriteEvent; update();}
            void disableWriting() { events_ &= ~kWriteEvent; update(); }
            void disableAll() { events_ = kNoneEvent; update(); }
            bool isWriting() const { return events_ & kWriteEvent; }
            bool isReading() const { return events_ & kReadEvent; }

            int index() {return index_;}
            void set_index(int idx) {index_ = idx;}

            string reventsToString() const;
            string eventsToString() const;

            void doNotLogHup() {logHup_ = false;}

            EventLoop* ownerLoop() {return loop_;}
            void remove();

        private:
            static string eventsToString(int fd, int ev);
            void update();
            void handleEventWithGuard(Timestamp receiveTime);

            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            EventLoop*      loop_;
            const int       fd_;        // 文件描述符，但不负责关闭该文件描述符
            int             events_;    // 关注的事件
            int             revents_;   // poll/epoll返回的事件
            int             index_;     // 表示在poll的事件数组中的序号 used by Poller.
            bool            logHup_;    // 表示某channel的状态

            std::weak_ptr<void> tie_;
            bool                tied_;
            bool                eventHandling_;
            bool                addedToLoop_;
            ReadEventCallback   readCallback_;
            EventCallback       writeCallback_;
            EventCallback       closeCallback_;
            EventCallback       errorCallback_;
        };
    }
}

#endif