#ifndef IOTORO_LINUX_H
#define IOTORO_LINUX_H

#define DEBUG 1

#include "iotoro.h"

#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <unistd.h>

#ifdef DEBUG
    #define IOTORO_LOG(msg) (std::cout << msg << std::endl)
#else
    #define IOTORO_LOG(msg) (msg)
#endif


class IotoroConnectionLinux: public IotoroConnection
{
    private:
        int sockfd;
            int newsockfd, portno;
            socklen_t clilen;
            struct sockaddr_in serv_addr, cli_addr;

    protected:
        /* Connects to the iotoro server. Returns -1 if error. */
        int doConnect();

        /* Disconnects to the iotoro server. Returns -1 if error. */
        int doDisconnect();

        /* Sends a packet to the iotoro server. Returns -1 if error. */
        int doWrite(const char* data, uint16_t len);

        /* Tries to read a packet and return it. This method blocks. */
        int doRead(char* buff, const size_t len);

    public:
        IotoroConnectionLinux();

};
 

class IotoroClientLinux: public IotoroClient
{
    private:
        virtual void generateIv(uint8_t iv[AES_BLOCKLEN]);

    public:
        IotoroClientLinux(const char* deviceId, const char* deviceKey, 
                          OPERATION_MODE mode = AUOMATIC);

  
};

#endif