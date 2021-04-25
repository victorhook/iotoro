#ifndef IOTORO_H
#define IOTORO_H


#include <stdint.h>
#include <string.h>
#include <string>
#include <iostream>

#include "aes.h"
 
/* --- Constants --- */
#define IOTORO_API_URL "localhost"
#define IOTORO_API_PORT 8000

#define IOTORO_API_METHOD "POST"
#define IOTORO_API_ENDPOINT "/api/"
#define IOTORO_PACKET_HEADER_SIZE 3
#define IOTORO_DEVICE_ID_SIZE 8                             // In bytes.
#define IOTORO_DEVICE_ID_SIZE_HEX IOTORO_DEVICE_ID_SIZE*2   // In bytes.
#define IOTORO_DEVICE_KEY_SIZE 16                           // In bytes.

#define IP_ADDR_SIZE 100
#define HTTP_ENDING_CR_BYTES 4 

/* Configurable */
#define IOTORO_PARAM_MAX_NAME_SIZE 10
#define IOTORO_MAX_PARAMETERS 10
#define IOTORO_MAX_HTTP_HEADER_SIZE 150
#define IOTORO_MAX_PAYLOAD_HEADER_SIZE 1

#define IOTORO_VERSION 1

#define DEBUG_LOG(msg) (std::cout << msg)


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

/*
typedef struct {
    uint8_t version;
    IOTORO_ACTION action;
    uint16_t payloadSize;
    char* data;
} IotoroPacket;
*/

typedef struct
{
    uint8_t version;
    IOTORO_ACTION action;
    uint16_t dataSize;
    uint8_t* data;
    uint8_t* deviceId;
    uint8_t* iv;
}  IotoroPacket; 


class IotoroConnection
{
    protected:
        char hostIp[IP_ADDR_SIZE];
        uint16_t hostPort;
        bool _isConnected;

        IotoroConnection();

        virtual int doWrite(const char* data, uint16_t len) = 0;
        virtual int doRead() = 0;
        virtual int doConnect() = 0;
        virtual int doDisconnect() = 0;

    public:
        /* Sends a packet to the iotoro server. Returns -1 if error. */
        int sendPacket(const char* data, uint16_t len);

        /* Tries to read a packet and return it. This method blocks. */
        int readPacket();

        /* Connects to the iotoro server. Returns -1 if error. */
        int openConnection();

        /* Disconnects to the iotoro server. Returns -1 if error. */
        int closeConnection();

        /* Returns if the connection is connected or not. */
        bool isConnected() { return _isConnected; }
};


class IotoroClient
{
    protected:
        IotoroConnection* connection;

    private:
        // For authorization and encryption.
        uint8_t deviceId[IOTORO_DEVICE_ID_SIZE];
        uint8_t deviceKey[IOTORO_DEVICE_KEY_SIZE];
        char* deviceIdHexified;

        // AES encryption.
        AES_ctx aes;

        // Param settings.
        OPERATION_MODE mode;
        Param params[IOTORO_MAX_PARAMETERS];
        uint8_t paramsSet;
        uint16_t paramWriteFrequency;
        uint16_t pingFrequency;

        char _httpHeaders[IOTORO_MAX_HTTP_HEADER_SIZE];
        uint16_t _httpHeaderSize;

        IotoroPacket iotoroPacket;

        // Setters & getters helper methods
        void setIotoroPacket(IOTORO_ACTION action, uint8_t iv[AES_IV_SIZE]);
        void setHttpHeaders();
        void setPayloadHeaders(IOTORO_ACTION action);
        void setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], PARAM_TYPE type);

        uint8_t getPadBytesRequired();
        uint16_t getPayloadSize();
        uint16_t getTotalPacketSize();

        void sendPacket();
        void encryptPayload(char* payload, uint16_t start, uint16_t end, uint8_t padBytesRequired);

        /* Generataing initialization vector for AES encryption is port-specific. */
        virtual void generateIv(uint8_t iv[AES_BLOCKLEN]){};

    public:
        IotoroClient(const char* deviceId, const char* deviceKey, IotoroConnection* con);
        IotoroClient(const char* deviceId, const char* deviceKey, IotoroConnection* con, 
                     OPERATION_MODE mode);


        void printVector(uint8_t vec[16]) {
            for (size_t i = 0; i < 16; i++) {
                printf("%x ", vec[i]);
            }
            printf("\n");
        }

        /* 
            Sends a packet to the iotoro backend server.
            This is done by:
            - Create the payload package.
            - Create the necessary HTTP headers.
            - Encrypt the payload
            - Send the data 
        */
        int send(IOTORO_ACTION action);

        void test()
        {
            send(IOTORO_PING);
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