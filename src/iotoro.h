#ifndef IOTORO_H
#define IOTORO_H

#include <stdint.h>
#include <string.h>
#include "aes.h"

/* -- Platform specific -- */
#define HEXIFLY_BIG_CHARS 1

#include <iostream>
#define IOTORO_LOG_DEBUG(msg) (std::cout << msg)
#define string std::string

#ifndef TO_STRING
    #define TO_STRING std::to_string
#endif

 
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
#define IOTORO_PARAM_TYPE_SIZE 1        // The type that indicates what type the parameter is.

/* Configurable */
#define IOTORO_MAX_PARAM_NAME_SIZE 10
#define IOTORO_MAX_PARAMETERS 10
#define IOTORO_MAX_HTTP_HEADER_SIZE 150
#define IOTORO_MAX_PAYLOAD_HEADER_SIZE 1
#define IOTORO_MAX_TCP_PACKET_READ_SIZE 512

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
    PARAM_BOOL,
    PARAM_UINT_8,
    PARAM_INT_8,
    PARAM_UINT_16,
    PARAM_INT_16,
    PARAM_UINT_32,
    PARAM_INT_32,
    PARAM_UINT_64,
    PARAM_INT_64,
    PARAM_FLOAT,
    PARAM_DOBULE,
} __attribute__((packed)) PARAM_TYPE;

union ParamPtr {
    uint8_t* u8;
    int8_t* i8;
    uint16_t* u16;
    int16_t* i16;
    uint32_t* u32;
    int32_t* i32;
    uint64_t* u64;
    int64_t* i64;
    float* f;
    double* d;
    bool* b;
};

typedef struct {
    char name[IOTORO_MAX_PARAM_NAME_SIZE];      // 10
    PARAM_TYPE type;                            // 1
    ParamPtr paramPtr;                          // 1
} Param;

typedef struct
{
    uint8_t version;
    IOTORO_ACTION action;
    uint16_t dataSize;
    uint8_t* data;
    uint8_t* deviceId;
    uint8_t* iv;
}  IotoroPacket; 

typedef struct
{
    uint16_t httpStatus;
    IotoroPacket* iotoroPacket;
} IotoroResponsePacket;

constexpr uint16_t IOTORO_MAX_PAYLOAD_SIZE = IOTORO_MAX_PARAMETERS * sizeof(PARAM_TYPE);


class IotoroConnection
{
    protected:
        char hostIp[IP_ADDR_SIZE];
        uint16_t hostPort;
        bool _isConnected;

        IotoroConnection();

        virtual int doWrite(const char* data, uint16_t len) = 0;
        virtual int doRead(char* buff, const size_t len) = 0;
        virtual int doConnect() = 0;
        virtual int doDisconnect() = 0;

    public:
        /* Sends a packet to the iotoro server. Returns -1 if error. */
        int sendPacket(const char* data, uint16_t len);

        /* Tries to read a packet and return it. This method blocks. */
        int readPacket(char* buff, size_t len);

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
        char* payloadBuf[IOTORO_MAX_PAYLOAD_SIZE];
        uint8_t paramsSet;


    private:
        // For authorization and encryption.
        uint8_t deviceId[IOTORO_DEVICE_ID_SIZE];
        uint8_t deviceKey[IOTORO_DEVICE_KEY_SIZE];
        const char* deviceIdHexified;
        const char* deviceKeyHexified;

        // AES encryption.
        AES_ctx aes;

        // Param settings.
        OPERATION_MODE mode;

        uint16_t paramWriteFrequency;
        uint16_t pingFrequency;

        char _httpHeaders[IOTORO_MAX_HTTP_HEADER_SIZE];
        uint16_t _httpHeaderSize;

        IotoroPacket iotoroPacket;
        IotoroResponsePacket iotoroResponsePacket;
 
        // Setters & getters helper methods
        void setIotoroPacket(IOTORO_ACTION action, uint8_t iv[AES_IV_SIZE]);
        void setHttpHeaders();
        void setPayloadHeaders(IOTORO_ACTION action);
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], PARAM_TYPE type);

        /* Parses the headers of the http packet. */
        void parseHttpHeaders(const char* data, const size_t maxLen);
        /* Returns the index of where the payload starts in a http packet. 
           Returns -1 if the payload is in incorrect http format. */
        int getPayloadIndex(const char* data, const size_t maxLen);

        void decryptPayload(char* buf, const size_t len, const uint8_t* iv);
        void decodePayload(char* buf, const size_t len);
        void fillPacketIv(const char* buf, uint8_t* iv);

        void debugPacket(const char* packet, const uint16_t packetSize);

        uint8_t getPadBytesRequired();
        uint16_t getPayloadSize();
        uint16_t getTotalPacketSize();
        uint16_t getTotalParamsSize();

        size_t fillBuffWithParams(char* buff);
        bool packetNeedsdParams();
        void fillPacket(char* buff, const uint16_t packetSize);

        // Encryption
        void addPadBytes(char* payload, uint8_t padBytesRequired);
        void encryptPayload(char* payload, size_t len);

        /* Generataing initialization vector for AES encryption is port-specific. */
        virtual void generateIv(uint8_t iv[AES_BLOCKLEN]){};

    public:
        IotoroClient(const char* deviceId, const char* deviceKey, 
                    IotoroConnection* con, OPERATION_MODE mode);

        /* 
            Sends a packet to the iotoro backend server.
            This is done by:
            - Create the payload package.
            - Create the necessary HTTP headers.
            - Encrypt the payload
            - Send the data 
        */
        int send(IOTORO_ACTION action);

        int recv();

        int sendParams();

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
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], bool& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint8_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int8_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint16_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int16_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint32_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int32_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], uint64_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], int64_t& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], float& ptr);

        /* Adds a paramter to the paramlist, that will be pushed upstream. */
        void setParam(const char name[IOTORO_MAX_PARAM_NAME_SIZE], double& ptr);
};

/* --- Utils --- */

/* Returns the byte value 0-255 from an ascii. */
uint8_t getAsciiValue(char c);

/* Returns the ascii representation of a byte. */
char valueToAscii(uint8_t val);

/* Turns a stream of hexified ascii characters to stream of bytes, */
void unHexifly(const char* from, char* to, size_t len);

/* Turns a stream of bytes into the ascii HEX representation. */
void hexifly(const char* from, char* to, size_t len);

/* Performs md5sum of the given input. */
void md5sum(const char* from, char to[16], size_t len);

/*  
    Returns the size of a parmeter type. 
    Note that these sizes are pre-decided, to ensure correct parsing
    on the server side.
*/
uint8_t getParamTypeSize(PARAM_TYPE type);

/* Returns the size of a parameter settings. */
uint8_t getParamSettingSize(Param param);

/* Puts the parameter data into the buffer, with the correct size and format. */
void putParamData(char* buff, const Param& param);

#endif