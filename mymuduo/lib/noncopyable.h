#pragma once
class noncopyable{
    private:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
    protected:
    noncopyable() = default;
};