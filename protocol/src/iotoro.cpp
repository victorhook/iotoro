#include "iotoro.h"
#include "AES.h"

#define string std::string



/* -- Connection -- */

IotoroConnection::IotoroConnection()
{
    char url[] = IOTORO_API_URL;
    strcpy(hostIp, url);
    hostPort = IOTORO_API_PORT;
}


/* -- Client -- */

IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey)
 : IotoroClient(deviceId, deviceKey, AUOMATIC)
{}

IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey,
                           OPERATION_MODE mode)
 : deviceId(deviceId), deviceKey(deviceKey), mode(mode)
{}


int IotoroClient::start()
{
    return connection->doConnect();
}

int IotoroClient::stop()
{
    return connection->doDisconnect();
}

int IotoroClient::ping()
{
    return 1;
}

void IotoroClient::setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], uint8_t* ptr)
{
    params[paramsSet].paramPtr.u8 = ptr;
    setParam(name, PARAM_UINT_8);
}

void IotoroClient::setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], int8_t* ptr)
{
    params[paramsSet].paramPtr.i8 = ptr;
    setParam(name, PARAM_INT_8);
}

void IotoroClient::setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], PARAM_TYPE type)
{
    Param* param = &params[paramsSet];
    strcpy(param->name, name);
    param->type = type;
    paramsSet++;
}

void IotoroClient::setParamWriteFrequency(uint16_t frequency)
{
    this->paramWriteFrequency = frequency;
}

void IotoroClient::setPingFrequency(uint16_t frequency)
{
    this->pingFrequency = frequency;
}

void IotoroClient::setOperationMode(OPERATION_MODE mode)
{
    this->mode = mode;
}

uint16_t IotoroClient::getParamWriteFrequency()
{
    return 1;
}

uint16_t IotoroClient::getPingFrequency()
{
    return 1;
}

OPERATION_MODE IotoroClient::getOperationMode()
{
    return mode;
}


/* -- Private -- */
void IotoroClient::setHttpHeaders(uint16_t httpPayloadSize)
{
    string headers = string(IOTORO_API_METHOD) + string(" ") + string(IOTORO_API_ENDPOINT) + string(" HTTP/1.1\r\n")
                     + string("Content-Type: application/x-www-form-urlencoded\r\n")
                     + string("Content-Length: ") + std::to_string(httpPayloadSize) 
                     + string("\r\n\r\n");      // Seperate headers from the data.
    _httpHeaderSize = headers.size();
    memset(_httpHeaders, 0, IOTORO_MAX_HTTP_HEADER_SIZE);
    strcpy(_httpHeaders, headers.c_str());
}

void IotoroClient::setIotoroPacket(IOTORO_ACTION action)
{
    /**
         |  0 - 3  | 4 - 7  |    8-23     |  24 - *   |
         |---------|--------|-------------|-----------|
         | Version | Action | Payload len |  Content  |
     */
    iotoroPacket.version = IOTORO_VERSION;
    iotoroPacket.action = action;
    switch (action) {
        case IOTORO_WRITE_UP:
            iotoroPacket.payloadSize = getIotoroPacketSize();
            iotoroPacket.data = (char *) params;
            break;
        default:
            iotoroPacket.payloadSize = 0;
            break;
    }
}

uint16_t IotoroClient::getIotoroPacketPayloadSize() 
{
    return (sizeof(Param) * paramsSet);
}

uint16_t IotoroClient::getIotoroPacketSize() 
{
    // Header is always 3. 
    // 0:   version + action
    // 1-2: payload length
    uint16_t size = IOTORO_PACKET_HEADER_SIZE;

    // Append the payload size.
    size += iotoroPacket.payloadSize;

    return size;
}



/* strcopy doesn't work for some reason so need this... */
static void copyStr(char* to, char* from) {
    while (*from)
        *to++ = *from++;
}


void IotoroClient::sendPacket() 
{ 
    uint16_t index = _httpHeaderSize;

    // Create a buffer for the packet.
    uint16_t packetSize = _httpHeaderSize + getIotoroPacketSize();
    char packet[packetSize];

    // Move the http headers to the packet
    copyStr(packet, _httpHeaders);

    // Start adding the payload.
    packet[index++] = iotoroPacket.version << 4 | iotoroPacket.action;

    // Add the payload size to the packet. 
    // Note that this must be treated as 16-bit word instead of a byte.
    *(uint16_t* ) (packet + index) = iotoroPacket.payloadSize;
    
    // Increment the index.
    index += 2;

    for (uint16_t i = 0; i < iotoroPacket.payloadSize; i++) {
        packet[index++] = iotoroPacket.data[i];
    }

    connection->sendPacket(packet, packetSize);
}