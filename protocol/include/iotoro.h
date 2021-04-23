#ifndef IOTORO_H
#define IOTORO_H


#include <stdint.h>
#include <string>
 
/* --- Constants --- */
#define IOTORO_API_URL "localhost"
#define IOTORO_API_PORT 8000

#define IOTORO_API_METHOD "POST"
#define IOTORO_API_ENDPOINT "/api"
#define IOTORO_PACKET_HEADER_SIZE 3

#define IP_ADDR_SIZE 100

/* Configurable */
#define IOTORO_PARAM_MAX_NAME_SIZE 10
#define IOTORO_MAX_PARAMETERS 10
#define IOTORO_MAX_HTTP_HEADER_SIZE 150
#define IOTORO_MAX_PAYLOAD_HEADER_SIZE 1

#define IOTORO_VERSION 1


typedef enum {
    AUOMATIC,
    MANUAL
} OPERATION_MODE;

typedef enum {
    IOTORO_PING = 1,
    IOTORO_WRITE_UP = 2,
    IOTORO_WRITE_DOWN = 3,
    IOTORO_PONG = 4,
    IOTORO_READ_UP = 5,
    IOTORO_READ_DOWN = 6
} IOTORO_ACTION;
    
typedef enum {
    PARAM_UINT_8,
    PARAM_INT_8,
} PARAM_TYPE;

union ParamPtr {
    uint8_t* u8;
    int8_t* i8;
};

typedef struct {
    char name[IOTORO_PARAM_MAX_NAME_SIZE];
    PARAM_TYPE type;
    ParamPtr paramPtr;
} Param;

typedef struct {
    uint8_t version;
    IOTORO_ACTION action;
    uint16_t payloadSize;
    char* data;
} IotoroPacket;
 
 

class IotoroConnection
{
    protected:
        char hostIp[IP_ADDR_SIZE];
        uint16_t hostPort;
        bool _isConnected;

        IotoroConnection();

    public:
        /* Connects to the iotoro server. Returns -1 if error. */
        virtual int doConnect() { return 0; }

        /* Disconnects to the iotoro server. Returns -1 if error. */
        virtual int doDisconnect() { return 0; }

        /* Sends a packet to the iotoro server. Returns -1 if error. */
        virtual int sendPacket(const char* data, uint16_t len) { return 0; }

        /* Tries to read a packet and return it. This method blocks. */
        virtual void readPacket() {}

        /* Returns if the connection is connected or not. */
        bool isConnected() { return _isConnected; }
};


class IotoroClient
{
    protected:
        IotoroConnection* connection;

    private:
        const char* deviceId;
        const char* deviceKey;
        OPERATION_MODE mode;
        Param params[IOTORO_MAX_PARAMETERS];
        uint8_t paramsSet;

        uint16_t paramWriteFrequency;
        uint16_t pingFrequency;


        //iotoroPacket _iotoroPacket;
        char _httpHeaders[IOTORO_MAX_HTTP_HEADER_SIZE];
        uint16_t _httpHeaderSize;

        IotoroPacket iotoroPacket;

        void setIotoroPacket(IOTORO_ACTION action);
        void setHttpHeaders(uint16_t httpPayloadSize);
        void setPayloadHeaders(IOTORO_ACTION action);
        void setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], PARAM_TYPE type);

        void sendPacket();

        uint16_t getIotoroPacketSize();
        uint16_t getIotoroPacketPayloadSize();

    public:
        IotoroClient(const char* deviceId, const char* deviceKey);
        IotoroClient(const char* deviceId, const char* deviceKey, 
                     OPERATION_MODE mode);


        void test() {
            setHttpHeaders(200);
            setIotoroPacket(IOTORO_PING);
            sendPacket();
        }

        /* 
            Starts constant communication with the backend server.
            Note: This requires mode to be set on AUOTOMATIC.
            This method return -1 if an error occurs.
        */
        int start();

        /* Stops the all connections to the server. */
        int stop();

        /* Sends a ping message to the iotoro server. Returns -1 if error. */
        int ping();

        /* Sets the frequency to write parameters upstream. In milliseconds */
        void setParamWriteFrequency(uint16_t frequency);

        /* Sets the frequency to ping upstream. In milliseconds */
        void setPingFrequency(uint16_t frequency);

        /* Sets the operation mode. */
        void setOperationMode(OPERATION_MODE mode);

        /* Returns the frequency to write parameters upstream. In milliseconds */
        uint16_t getParamWriteFrequency();

        /* Returns the frequency to ping upstream. In milliseconds */
        uint16_t getPingFrequency();

        /* Returns the operation mode. */
        OPERATION_MODE getOperationMode();

        /* Param setters
           The parameters can have multiple different types.
        */

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], uint8_t* ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], int8_t* ptr);
};

#endif