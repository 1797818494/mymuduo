#pragma once
#include<netinet/in.h>
#include<string>
class InetAddress{
    public:
    explicit InetAddress(int port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in& myAddress)
    : myAddress_(myAddress) {}
    std::string toIP() const;
    uint16_t toPort() const;
    std::string toIpPort() const;
    const sockaddr_in* getAddress() const 
    {
        return &myAddress_;
    }
    void setSockAddress(const sockaddr_in new_address);

    private:
    struct sockaddr_in myAddress_;
};