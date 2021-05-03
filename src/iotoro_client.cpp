#include "iotoro.h"


/* -- Client -- */
IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey, IotoroConnection* con,
                           OPERATION_MODE mode)
 : connection(con), deviceIdHexified(deviceId), deviceKeyHexified(deviceKey), mode(mode)
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

uint16_t IotoroClient::getTotalParamsSize()
{
    uint16_t sum = 0;
    Param* params = (Param*) payloadBuf;
    for (uint8_t i = 0; i < paramsSet; i++) {
        sum += getParamSettingSize(params[i]);
    }
    return sum;
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
            iotoroPacket.dataSize = getTotalParamsSize();
            iotoroPacket.data = (uint8_t *) payloadBuf;
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
    uint8_t remainder = size % AES_BLOCKLEN;
    return remainder == 0 ? AES_BLOCKLEN :  AES_BLOCKLEN - remainder;
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

void IotoroClient::debugPacket(const char* packet, const uint16_t packetSize)
{
    uint16_t index = _httpHeaderSize;
    int padsRequired = getPadBytesRequired();

    printf(" | ");
    asHex(packet + index, 3);
    printf(" | ");
    index += 3;

    asHex(packet + index, iotoroPacket.dataSize);
    printf(" | ");
    index += iotoroPacket.dataSize;

    asHex(packet + index, IOTORO_DEVICE_ID_SIZE);
    printf(" | ");
    index += IOTORO_DEVICE_ID_SIZE;

    asHex(packet + index, padsRequired);
    printf(" | ");
    index += padsRequired;

    // ----------------
    index = _httpHeaderSize;

    printf("\n\nPacket: ");
    asHex(packet + index, packetSize - index);
    printf("\n----------------------------------\n");

    printf("Header [3]: ");
    asHex(packet + index, 3);
    printf("\n----------------------------------\n");
    index += 3;

    printf("Payload [%d]: ", iotoroPacket.dataSize);
    asHex(packet + index, iotoroPacket.dataSize);
    printf("\n----------------------------------\n");
    index += iotoroPacket.dataSize;

    printf("Device id [%d]: ", IOTORO_DEVICE_ID_SIZE);
    asHex(packet + index, IOTORO_DEVICE_ID_SIZE);
    printf("   | REAL: [%d] ", IOTORO_DEVICE_ID_SIZE);
    asHex((char*) deviceId, IOTORO_DEVICE_ID_SIZE);
    printf("\n----------------------------------\n");
    index += IOTORO_DEVICE_ID_SIZE;

    printf("Padbytes [%d]: ", padsRequired);
    asHex(packet + index, padsRequired);
    printf("\n----------------------------------\n");
    index += padsRequired;

    printf("Device key: ");
    asHex((char*) deviceKey, IOTORO_DEVICE_KEY_SIZE);
    printf("\n\n");

}

int IotoroClient::send(IOTORO_ACTION action)
{
    uint8_t iv[AES_BLOCKLEN];
    //generateIv(iv);
    memset(iv, 0, 16);

    setIotoroPacket(action, iv);
    setHttpHeaders();

    // Create a buffer for the packet.
    uint16_t packetSize = getTotalPacketSize();
    char packet[packetSize];

    fillPacket(packet, packetSize);
    debugPacket(packet, packetSize);

    connection->openConnection();
    connection->sendPacket(packet, packetSize);
    recv();
    connection->closeConnection();
    return 1;
}

int IotoroClient::sendParams()
{
    return send(IOTORO_WRITE_UP);
}

size_t IotoroClient::fillBuffWithParams(char* buff)
{
    /*
        |  0-9  |  10  |  11-18  |
        |-------|------|---------|
        |  name | type |  value  |
    */
    size_t index = 0;
    Param* paramPtr = (Param*) payloadBuf;

    for (uint8_t i = 0; i < paramsSet; i++) {
        Param param = paramPtr[i];

        // 0-9 - name
        for (char c: param.name) {
            buff[index++] = c;
        }

        // 10 - type
        buff[index++] = param.type;

        // 11-18 - value
        putParamData(buff + index, param);
        index += getParamTypeSize(param.type);
    }
    printf("Filled params with %lu bytes\n", index);
    return index;
}

bool IotoroClient::packetNeedsdParams()
{
    return iotoroPacket.action == IOTORO_WRITE_UP;
}

void IotoroClient::fillPacket(char* packet, const uint16_t packetSize) 
{ 
    memset(packet, 0, packetSize);
    uint16_t index = _httpHeaderSize;
    uint16_t padBytesRequired = getPadBytesRequired();

    // Move the http headers to the packet
    memcpy(packet, (const char*) _httpHeaders, _httpHeaderSize);

    // Start adding the payload.
    packet[index++] = iotoroPacket.version << 4 | iotoroPacket.action;

    // Add the payload size to the packet. 
    // Note that this must be treated as 16-bit word instead of a byte.
    *(uint16_t* ) (packet + index) = iotoroPacket.dataSize;
    index += 2;

    if (packetNeedsdParams())
        index += fillBuffWithParams(packet + index);

    // Append the device id to the data.
    memcpy(packet + index, deviceId, IOTORO_DEVICE_ID_SIZE);
    index += IOTORO_DEVICE_ID_SIZE;

    // Add pad bytes to ensure data is block size of 16 bytes (AES128)
    addPadBytes(packet + index, padBytesRequired);
    index += padBytesRequired;
    
    printf("Before encryption: \n");
    debugPacket(packet, packetSize);

    uint16_t dataToEncrypt = index - _httpHeaderSize;
    encryptPayload(packet + _httpHeaderSize, dataToEncrypt);

    printf("\nEfter encryption:\n");

    // Finally, add iv to the payload.
    memcpy(packet + index, iotoroPacket.iv, AES_IV_SIZE);
    index += AES_IV_SIZE;

    addHttpEnding(packet + index);
}

void IotoroClient::addPadBytes(char* payload, uint8_t padBytesRequired)
{
    memset(payload, padBytesRequired, padBytesRequired);
}

void IotoroClient::encryptPayload(char* payload, size_t len)
{
    // Init the aes algorithm.
    AES_init_ctx_iv(&aes, (const uint8_t *) deviceKey, (const uint8_t *) iotoroPacket.iv);

    // Encrypt the data.
    AES_CBC_encrypt_buffer(&aes, (uint8_t *) payload, len);
}

int IotoroClient::getPayloadIndex(const char* buf, const size_t maxLen)
{
    // TODO: Handle incorrect formats.
    const char *payloadIndex = strstr(buf, "\r\n\r\n");
    size_t index = payloadIndex - buf + HTTP_ENDING_CR_BYTES;
    return (index > maxLen || index < 0) ? -1 : index;
}

/* Currently only saves the http status code and nothing else from the header. */
void IotoroClient::parseHttpHeaders(const char* data, const size_t maxLen)
{
    char* p = (char*) data;
    string version, statusCode, statusWord;

    while (*p != ' ') {       // Version
        version.push_back(*p++);
    }
    p++;
    while (*p != ' ') {      // StatusCode
        statusCode.push_back(*p++);
    }
    p++;
    while (*p != '\r') {      // StatusWord
        statusWord.push_back(*p++);
    }

    int httpStatusCode = atoi(statusCode.c_str());
    iotoroResponsePacket.httpStatus = httpStatusCode;
}

int IotoroClient::recv()
{
    char buf[IOTORO_MAX_TCP_PACKET_READ_SIZE];
    int read = connection->readPacket(buf, IOTORO_MAX_TCP_PACKET_READ_SIZE);

    parseHttpHeaders(buf, read);
    int payloadIndex = getPayloadIndex(buf, read);

    if (payloadIndex < 0) {
        IOTORO_LOG_DEBUG("Failed to parse http headers!");
        return -1;
    }

    // Find the iv in the packet.
    int ivIndex = read - AES_IV_SIZE;
    fillPacketIv(buf + ivIndex, iotoroPacket.iv);

    int payloadLength = read            // Total bytes read
                        - payloadIndex  // Start of packet body
                        - AES_IV_SIZE;  // Size of IV
    
    decryptPayload(buf + payloadIndex, payloadLength, iotoroPacket.iv);

    // Last X bytes are always pad-bytes, since we're using AES.
    payloadLength -= getPadBytes(buf, payloadLength);

    decodePayload(buf + payloadIndex, payloadLength);

    return read;
}

void IotoroClient::decodePayload(char* buf, const size_t len)
{
    char first = buf[0];
    iotoroPacket.version = (first & 0xf0) >> 4;
    iotoroPacket.action = (IOTORO_ACTION) (first & 0x0f);
    iotoroPacket.dataSize = (buf[1] << 8) | buf[2];

    int dataLength = len > IOTORO_MAX_PAYLOAD_SIZE 
                     ? IOTORO_MAX_PAYLOAD_SIZE
                     : len;
    strncpy((char*) payloadBuf, buf, dataLength);
    iotoroPacket.data = (uint8_t*) payloadBuf;
}

void IotoroClient::decryptPayload(char* buf, const size_t len, const uint8_t* iv)
{
    // Init the aes algorithm with the device key.
    AES_init_ctx_iv(&aes, (const uint8_t *) deviceKey, iv);

    // Encrypt the data with the given iv.
    AES_CBC_decrypt_buffer(&aes, (uint8_t *) buf, len);
}

void IotoroClient::fillPacketIv(const char* buf, uint8_t* iv)
{
    // The iv is the last AES_IV_SIZE bytes of the packet.
    strncpy((char*) iv, buf, AES_IV_SIZE);
}


/* Set params. */
void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], PARAM_TYPE type)
{
    Param* param = &((Param*) payloadBuf)[paramsSet];
    strcpy(param->name, name);
    param->type = type;
    paramsSet++;
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], bool& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.b = &ptr;
    setParam(name, PARAM_BOOL);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint8_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.u8 = &ptr;
    setParam(name, PARAM_UINT_8);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int8_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.i8 = &ptr;
    setParam(name, PARAM_INT_8);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint16_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.u16 = &ptr;
    setParam(name, PARAM_UINT_16);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int16_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.i16 = &ptr;
    setParam(name, PARAM_INT_16);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint32_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.u32 = &ptr;
    setParam(name, PARAM_UINT_32);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int32_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.i32 = &ptr;
    setParam(name, PARAM_INT_32);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint64_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.u64 = &ptr;
    setParam(name, PARAM_UINT_64);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int64_t& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.i64 = &ptr;
    setParam(name, PARAM_INT_64);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], float& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.f = &ptr;
    setParam(name, PARAM_FLOAT);
}

void IotoroClient::setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], double& ptr)
{
    ((Param*) payloadBuf)[paramsSet].paramPtr.d = &ptr;
    setParam(name, PARAM_DOBULE);
}

