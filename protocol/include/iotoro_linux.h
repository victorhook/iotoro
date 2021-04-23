#ifndef IOTORO_LINUX_H
#define IOTORO_LINUX_H

#define DEBUG 1

#include "iotoro.h"
#include <iostream>

#ifdef DEBUG
    #define IOTORO_LOG(msg) (std::cout << msg << std::endl)
#else
    #define IOTORO_LOG(msg) (msg)
#endif

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <unistd.h>


class IotoroConnectionLinux: public IotoroConnection
{
    private:
        int sockfd;
            int newsockfd, portno;
            socklen_t clilen;
            struct sockaddr_in serv_addr, cli_addr;

    public:
        IotoroConnectionLinux();

        /* Connects to the iotoro server. Returns -1 if error. */
        virtual int doConnect();

        /* Disconnects to the iotoro server. Returns -1 if error. */
        virtual int doDisconnect();

        /* Sends a packet to the iotoro server. Returns -1 if error. */
        virtual int sendPacket(const char* data, uint16_t len);

        /* Tries to read a packet and return it. This method blocks. */
        virtual void readPacket();

        void test() {
            doConnect();
            std::string msg = "Hello world!";
            sendPacket(msg.c_str(), msg.size());
            readPacket();
            doDisconnect();
        }
};


class IotoroClientLinux: IotoroClient
{
    public:
        IotoroClientLinux(const char* deviceId, const char* deviceKey);
        IotoroClientLinux(const char* deviceId, const char* deviceKey, 
                          OPERATION_MODE mode);
};

#endif