#include "InetAddress.h"
#include "string.h"
#include "arpa/inet.h"
InetAddress::InetAddress(int port, std::string ip) {
    bzero(&myAddress_, sizeof(myAddress_));
    myAddress_.sin_family = AF_INET;
    myAddress_.sin_port = htons(port);
    myAddress_.sin_addr.s_addr = inet_addr(ip.c_str());
}
std::string InetAddress::toIP() const {
    char buf[32] = {0};
    ::inet_ntop(AF_INET, &myAddress_.sin_addr, buf, 32);
    return buf;
}
uint16_t InetAddress::toPort() const {
    return ntohs(myAddress_.sin_port);
}
std::string InetAddress::toIpPort() const {
    char buf[32] = {0};
    ::inet_ntop(AF_INET, &myAddress_.sin_addr, buf, 32);
    sprintf(buf + strlen(buf), " %d", ntohs(myAddress_.sin_port));
    return buf;
}
void InetAddress::setSockAddress(sockaddr_in new_address) {
    memcpy(&myAddress_, &new_address, sizeof(new_address));
}