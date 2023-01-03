#pragma once

#include <vector>
#include <string>
#include <algorithm>

//网络库底层的缓冲器
/**
 * -----------|--------|-----------
 * Prepending   reader     writer
 *           readIdx  writeIdx
*/
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t InitialSize = kInitialSize)
    : buffer_(kInitialSize + kCheapPrepend)
    , readerIndex_(kCheapPrepend)
    , writerIndex_(kCheapPrepend)
    {}

    size_t readableBytes() const 
    {
        return writerIndex_ - readerIndex_;
    }
    size_t writableBytes() const 
    {
        return buffer_.size() - writerIndex_;
    }
    size_t prependableBytes() const 
    {
        return readerIndex_;
    }
    //返回数据中可读的起始位置
    const char* peek() const 
    {
        return begin() + readerIndex_;
    }
    void retrieve(size_t len) 
    {
        if(len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }
    void retrieveAll()
    {
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }
    
    std::string retrieveAsString(size_t len) 
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWriteableBytes(size_t len) 
    {
        if(writableBytes() < len) 
        {
            makeSpace(len);
        }
    }

    void append(const char* data, size_t len)
    {
        ensureWriteableBytes(len);
        std::copy(data, data + len,
        begin() + writerIndex_);
        writerIndex_ += len;
    }
    char* beginWrite()
    {
        return begin() + writerIndex_;
    }
    
    const char* beginWrite() const 
    {
        return begin() + writerIndex_;
    }

    //从fd上读取数据
    ssize_t readFd(int fd, int* saveErrno);
    //通过fd发送数据
    ssize_t writeFd(int fd, int* saveErrno);
private:
    char* begin() 
    {
        return &*buffer_.begin();
    }
    const char* begin() const 
    {
        return &*buffer_.begin();
    }
    void makeSpace(size_t len)
    {
        if(writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(len + kCheapPrepend);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                begin() + writerIndex_,
                begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readable + readerIndex_;
        }
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
    
};