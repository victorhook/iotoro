#include "iotoro.h"


IotoroConnection::IotoroConnection()
{
    char url[] = IOTORO_API_URL;
    strcpy(hostIp, url);
    hostPort = IOTORO_API_PORT;
}

int IotoroConnection::sendPacket(const char* data, uint16_t len) 
{ 
    IOTORO_LOG_DEBUG("\n");

    if (_isConnected) {
        return doWrite(data, len-1);
    } else {
        IOTORO_LOG_DEBUG("Not connected, can't send packet!\n");
        return -1;
    }
}

int IotoroConnection::readPacket(char* buff, size_t len) 
{
    if (_isConnected) {
        int bytesRead = doRead(buff, len);
        IOTORO_LOG_DEBUG("Read ");
        IOTORO_LOG_DEBUG(bytesRead);
        IOTORO_LOG_DEBUG(" bytes.\n");
        return bytesRead;
    } else {
        IOTORO_LOG_DEBUG("Not connected, can't read packet!\n");
        return -1;
    }
}

int IotoroConnection::openConnection() 
{
    if (_isConnected) {
        IOTORO_LOG_DEBUG("Already connected!\n");
        return -1;
    } else {
        IOTORO_LOG_DEBUG("Trying to connect to ");
        IOTORO_LOG_DEBUG(hostIp);
        IOTORO_LOG_DEBUG(" on port ");
        IOTORO_LOG_DEBUG(hostPort);
        IOTORO_LOG_DEBUG("\n");
        int connectedResult = doConnect();
        if (connectedResult)
            _isConnected = true;
        else
            IOTORO_LOG_DEBUG("Failed to open connection!\n");
        return _isConnected;
    }
}

int IotoroConnection::closeConnection() 
{
    if (_isConnected) {
        _isConnected = false;
        return doDisconnect();
    } else {
        IOTORO_LOG_DEBUG("Not connected, nothing to close!\n");
        return -1;
    }
}
