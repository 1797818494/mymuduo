#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

/**
 *从fd上读数据。POller工作在LT模式
 * buffer缓冲区有大小， 但是从fd上读数据的时候。却不知道tcp数据的最终大小
 * 
*/

ssize_t Buffer::readFd(int fd, int* saveErrno) 
{
    char extrabuf[65536] = {0}; //the memory from stack 64k 因为tcp窗口大小字段为16bit，最大容量为64k
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if(n < 0) 
    {
        *saveErrno = errno;
    }
    else if(n <= writable) 
    {
        writerIndex_ += n;
    }
    else 
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable); // writerIndex_开始写 n - writable大小的数据
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int* savaErrno) 
{
    ssize_t n = ::write(fd, peek(), readableBytes());
    if(n < 0) 
    {
        *savaErrno = errno;
    }
    return n;
}