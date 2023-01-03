#pragma once
#include "noncopyable.h"
#include<iostream>
#include<string>

class Stamptimer{
    public:
        Stamptimer();
        explicit Stamptimer(int64_t microSecondSinceEpoch);
        static Stamptimer now();
        std::string to_String() const;
    private:
        int64_t microSecondSinceEpoch_;
};