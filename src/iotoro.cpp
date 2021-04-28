#include "iotoro.h"
#include "md5.h"

#include <string.h>

/* -- Inlines -- */

#define HEXIFLY_BIG_CHARS 1

#include <iostream>
#define IOTORO_LOG_DEBUG(msg) (std::cout << msg)
#define string std::string

#ifndef TO_STRING
    #define TO_STRING std::to_string
#endif


/* Returns the byte value 0-255 from an ascii. */
static inline uint8_t getAsciiValue(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    } else {
        return c - '0';
    }
}

/* Returns the ascii representation of a byte. */
static inline char valueToAscii(uint8_t val)
{
    if (val <  10) {
        return '0' + val;
    } else {
        #ifdef HEXIFLY_BIG_CHARS
            char offset = 'A';
        #else
            unsigned char offset = 'a';
        #endif
        return offset + (val - 10);
    }
}

/* Turns a stream of hexified ascii characters to stream of bytes, */
static inline void unHexifly(const char* from, char* to, size_t len)
{
    size_t i = 0, j = 0;
    while (i < len) {
        to[i] = getAsciiValue(from[j]) * 16 + getAsciiValue(from[j + 1]);
        j += 2;
        i++;
    }
}

/* Turns a stream of bytes into the ascii HEX representation. */
static inline void hexifly(const char* from, char* to, size_t len)
{
    size_t i = 0, j = 0;
    while (i < len) {
        uint8_t first = (from[i] & 0xf0) >> 4;
        uint8_t second = from[i] & 0x0f;

        to[j++] = valueToAscii(first);
        to[j++] = valueToAscii(second);

        i++;
    }
}

/* Performs md5sum of the given input. */
static inline void md5sum(const char* from, char to[16], size_t len)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, from, len);
    MD5_Final((unsigned char*) to, &ctx);
}


/* -- Connection -- */

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

int IotoroConnection::readPacket() 
{
    if (_isConnected) {
        return doRead();
    } else {
        IOTORO_LOG_DEBUG("Not connected, can't send packet!\n");
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
        doConnect();
        return 1;
    }
}

int IotoroConnection::closeConnection() 
{
    if (_isConnected) {
        return doDisconnect();
    } else {
        IOTORO_LOG_DEBUG("Not connected, nothing to close!\n");
        return -1;
    }
}



/* -- Client -- */

IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey, IotoroConnection* con)
 : IotoroClient(deviceId, deviceKey, con, AUOMATIC)
{}

IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey, IotoroConnection* con,
                           OPERATION_MODE mode)
 : connection(con), deviceIdHexified(deviceId), mode(mode)
{
    unHexifly(deviceId, (char*) this->deviceId, IOTORO_DEVICE_ID_SIZE);
    unHexifly(deviceKey, (char*) this->deviceKey, IOTORO_DEVICE_KEY_SIZE);
}

int IotoroClient::start()
{
    return connection->openConnection();
}

int IotoroClient::stop()
{
    return connection->closeConnection();
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


uint16_t IotoroClient::getPayloadSize()
{
    return IOTORO_PACKET_HEADER_SIZE
            + iotoroPacket.dataSize
            + IOTORO_DEVICE_ID_SIZE
            + getPadBytesRequired()
            + AES_IV_SIZE;
}

/* -- Private -- */
void IotoroClient::setHttpHeaders()
{
    // Calculate the md5sum of the device id before putting it into the endpoint URL.
    char deviceIdMd5sum[IOTORO_DEVICE_ID_SIZE_HEX];

    // The hexified version of the md5sum must be 2 * 16 (16 is md5sum size, aka 128 bits)
    char deviceIdM5dsumHexified[32];
    md5sum((char *) deviceIdHexified, deviceIdMd5sum, IOTORO_DEVICE_ID_SIZE_HEX);

    // Hexifly the md5sum of the result so it's string-friendly.
    hexifly((const char*) deviceIdMd5sum, deviceIdM5dsumHexified, 16);

    string endpoint = string(IOTORO_API_ENDPOINT) + string(deviceIdM5dsumHexified, 32) + string("/");
    string headers = string(IOTORO_API_METHOD) + string(" ") + endpoint  + string(" HTTP/1.1\r\n")
                     + string("Content-Type: application/x-www-form-urlencoded\r\n")
                     + string("Content-Length: ") + TO_STRING(getPayloadSize())
                     + string("\r\n\r\n");      // Seperate headers from the data.
    _httpHeaderSize = headers.size();
    memset(_httpHeaders, 0, IOTORO_MAX_HTTP_HEADER_SIZE);
    strcpy(_httpHeaders, headers.c_str());
}

void IotoroClient::setIotoroPacket(IOTORO_ACTION action, uint8_t iv[AES_IV_SIZE])
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
            iotoroPacket.dataSize = (sizeof(Param) * paramsSet);
            iotoroPacket.data = (uint8_t *) params;
            break;
        default:
            iotoroPacket.dataSize = 0;
            break;
    }
    iotoroPacket.iv = iv;
    iotoroPacket.deviceId = this->deviceId;
}



uint8_t IotoroClient::getPadBytesRequired()
{
    // Headers + Payload + Device id needs to be encrypted.
    uint8_t size = IOTORO_PACKET_HEADER_SIZE + iotoroPacket.dataSize + IOTORO_DEVICE_ID_SIZE;
    uint8_t remainder = AES_BLOCKLEN % size;
    return remainder == 0 ? AES_BLOCKLEN : remainder;
}

uint16_t IotoroClient::getTotalPacketSize()
{
    return _httpHeaderSize \
            + IOTORO_PACKET_HEADER_SIZE
            + iotoroPacket.dataSize
            + IOTORO_DEVICE_ID_SIZE
            + getPadBytesRequired()
            + AES_IV_SIZE
            + HTTP_ENDING_CR_BYTES;
}

static inline void addHttpEnding(char* packet)
{
    packet[0] = '\r';
    packet[1] = '\n';
    packet[2] = '\r';
    packet[3] = '\n';
}

int IotoroClient::send(IOTORO_ACTION action)
{
    uint8_t iv[AES_BLOCKLEN];
    generateIv(iv);

    setIotoroPacket(action, iv);
    setHttpHeaders();

    connection->openConnection();
    sendPacket();
    connection->readPacket();
    connection->closeConnection();
    return 1;
}

void IotoroClient::sendPacket() 
{ 
    uint16_t index = _httpHeaderSize;

    // Before we create a buffer for the packet, we need to know if, and how
    // many bytes of padding we need to add during the encryption.
    uint16_t padBytesRequired = getPadBytesRequired();
    uint16_t packetSize = getTotalPacketSize();

    // Create a buffer for the packet.
    char packet[packetSize];

    // Move the http headers to the packet
    memcpy(packet, (const char*) _httpHeaders, _httpHeaderSize);

    // Start adding the payload.
    packet[index++] = iotoroPacket.version << 4 | iotoroPacket.action;

    // Add the payload size to the packet. 
    // Note that this must be treated as 16-bit word instead of a byte.
    *(uint16_t* ) (packet + index) = iotoroPacket.dataSize;
    
    index += 2;
    for (uint16_t i = 0; i < iotoroPacket.dataSize; i++) 
        packet[index++] = iotoroPacket.data[i];

    // Append the device id to the data.
    memcpy(packet + index, deviceId, IOTORO_DEVICE_ID_SIZE);
    index += IOTORO_DEVICE_ID_SIZE;

    encryptPayload(packet, _httpHeaderSize, index, padBytesRequired);
    index += padBytesRequired;

    // Finally, add iv to the payload.
    memcpy(packet + index, iotoroPacket.iv, AES_IV_SIZE);
    index += AES_IV_SIZE;

    addHttpEnding(packet + index);

    connection->sendPacket(packet, packetSize);
}

void IotoroClient::encryptPayload(char* payload, uint16_t start, uint16_t end, uint8_t padBytesRequired)
{
    // Init the aes algorithm.
    AES_init_ctx_iv(&aes, (const uint8_t *) deviceKey, (const uint8_t *) iotoroPacket.iv);

    // Pad the data, if needed.
    memset(payload + end, padBytesRequired, padBytesRequired);

    // Encrypt the data.
    size_t len = (end - start) + padBytesRequired;
    AES_CBC_encrypt_buffer(&aes, (uint8_t *) (payload + start), len);
}
