#include "iotoro.h"
#include "iotoro_protocol.h"
#include "AES.h"



const char* API_IP = IOTORO_API_URL;
const char* API_URL = IOTORO_API_URL;
const uint16_t API_PORT = IOTORO_API_PORT;


IotoroConnection::IotoroConnection(const char* hostIp, const uint16_t hostPort)
    : hostIp(hostIp), hostPort(hostPort)
{}

IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey)
 : IotoroClient(deviceId, deviceKey, AUOMATIC)
{}

IotoroClient::IotoroClient(const char* deviceId, const char* deviceKey,
                           OPERATION_MODE mode)
 : deviceId(deviceId), deviceKey(deviceKey), mode(mode),
   connection(API_IP, API_PORT),
   _httpHeaders("POST /API/ HTTP/1.1\r\n Content-Type: application/x-www-form-urlencoded\r\n")
{}


void IotoroClient::makeHttpPacket(iotoroPacket& body)
{
    /*
    std::string len = std::string("Content-Length: ") + std::to_string(body.size) + std::string("\r\n");
    _httpHeaders.reserve(len.size());
    _httpPacket.body = body;
    _httpPacket.header = _httpHeaders;
    */
}

void IotoroClient::makeIotoroPacket(const IOTORO_ACTION action, const uint16_t size, 
                                    const char* data)
{
    /*
    _iotoroPacket.version = IOTORO_VERSION;
    _iotoroPacket.action = action;
    _iotoroPacket.size = size;
    _iotoroPacket.data = data;
    */
}


int IotoroClient::start()
{
    return connection.connect();
}

int IotoroClient::stop()
{
    return connection.disconnect();
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

    // Copy the name into param-name buffer.
    for (uint8_t i = 0; i < IOTORO_PARAM_MAX_NAME_SIZE; i++) {
        if (!name[i])
            break;
        param->name[i] = name[i];
    }

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