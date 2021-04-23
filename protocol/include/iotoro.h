#ifndef IOTORO_H
#define IOTORO_H

#include "iotoro_protocol.h"

#include <stdint.h>
#include <string>
 
/* --- Defines --- */
#define IOTORO_API_URL "127.0.0.1"
#define IOTORO_API_ENDPOINT "/api"
#define IOTORO_API_PORT 8000
#define IOTORO_PARAM_MAX_NAME_SIZE 10
#define IOTORO_MAX_PARAMETERS 10


#define IOTORO_VERSION 1


typedef enum {
    AUOMATIC,
    MANUAL
} OPERATION_MODE;


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

 
class IotoroConnection
{
    protected:
        const char* hostIp;
        const uint16_t hostPort;

    public:
        IotoroConnection(const char* hostIp, const uint16_t hostPort);

        /* Connects to the iotoro server. Returns -1 if error. */
        virtual int connect() { return 0; }

        /* Disconnects to the iotoro server. Returns -1 if error. */
        virtual int disconnect() { return 0; }

        /* Sends a packet to the iotoro server. Returns -1 if error. */
        virtual int sendPacket() { return 0; }

        /* Tries to read a packet and return it. This method blocks. */
        virtual void readPacket() {}
};


class IotoroClient
{
    private:
        const char* deviceId;
        const char* deviceKey;
        OPERATION_MODE mode;
        Param params[IOTORO_MAX_PARAMETERS];
        uint8_t paramsSet;

        uint16_t paramWriteFrequency;
        uint16_t pingFrequency;

        IotoroConnection connection;

        //iotoroPacket _iotoroPacket;
        //httpPacket _httpPacket;
        std::string _httpHeaders;

        void setParam(const char name[IOTORO_PARAM_MAX_NAME_SIZE], PARAM_TYPE type);
    public:

        IotoroClient(const char* deviceId, const char* deviceKey);
        IotoroClient(const char* deviceId, const char* deviceKey, 
                     OPERATION_MODE mode);


        void makeHttpPacket(iotoroPacket& body);

        void makeIotoroPacket(const IOTORO_ACTION action, const uint16_t size, 
                                    const char* data);

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