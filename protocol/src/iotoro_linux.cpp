#include "iotoro_linux.h"

/* Creds to http://libraryofcprograms.blogspot.com/2014/10/url-to-ip-address.html */
static int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname( hostname )) == NULL) {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++) {
        // Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}

int IotoroConnectionLinux::doConnect()
{
    if (_isConnected) {
        IOTORO_LOG("Already connected");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        IOTORO_LOG("Error opening socket");
        return -1;
    }

    // Clear address
    bzero(&serv_addr, sizeof(serv_addr));
  
    // assign IP, PORT
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(hostIp);
    serv_addr.sin_port = htons(hostPort);
    
    if (connect(sockfd, (const sockaddr*) &serv_addr, sizeof(serv_addr)) != 0) {
        IOTORO_LOG("Error connecting to server");
        return -1;
    }

    _isConnected = true;
    return 1;
}

int IotoroConnectionLinux::doDisconnect()
{
    if (!_isConnected) {
        IOTORO_LOG("Not connected, can't disconnect.");
        return -1;
    }

    close(sockfd);

    sockfd = 0;
    _isConnected = false;
    return 1;
}

int IotoroConnectionLinux::sendPacket(const char* data, uint16_t len)
{
    int res = write(sockfd, data, len);
    if (res < 0) {
        IOTORO_LOG("Failed to write data to socket!");
    }
    return len - res;
}

void IotoroConnectionLinux::readPacket()
{
    char buf[100];
    bzero(buf, 11);
    read(sockfd, buf, 10);
    std::cout << buf << std::endl;
}


IotoroConnectionLinux::IotoroConnectionLinux()
{
    // Perform DNS lookup and set the correct hostIp.
    char buf[100] = IOTORO_API_URL;
    hostname_to_ip(buf, hostIp);
    hostPort = IOTORO_API_PORT;
}

// Create a connection object.
IotoroConnection con = IotoroConnectionLinux();


/* -- Client -- */

IotoroClientLinux::IotoroClientLinux(const char* deviceId, const char* deviceKey)
    : IotoroClient(deviceId, deviceKey)
{
    connection = &con;
}
IotoroClientLinux::IotoroClientLinux(const char* deviceId, const char* deviceKey, 
                                     OPERATION_MODE mode)
    : IotoroClient(deviceId, deviceKey, mode)
{
    connection = &con;
} 