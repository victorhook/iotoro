#ifndef IOTORO_ESP8266_H
#define IOTORO_ESP8266_H

#ifdef DEBUG
    #define IOTORO_LOG_DEBUG(msg) Serial.print(msg)
#else
    #define IOTORO_LOG_DEBUG(msg) (msg)
#endif

#define TO_STRING String

#include "iotoro.h"
#include "ESP8266WiFi.h"
#include <iostream>


class IotoroConnectionESP8266: public IotoroConnection
{
    private:
        WiFiClient* client;

    protected:
        /* Connects to the iotoro server. Returns -1 if error. */
        int doConnect();

        /* Disconnects to the iotoro server. Returns -1 if error. */
        int doDisconnect();

        /* Sends a packet to the iotoro server. Returns -1 if error. */
        int doWrite(const char* data, uint16_t len);

        /* Tries to read a packet and return it. This method blocks. */
        int doRead();

    public:
        IotoroConnectionESP8266();
        void setWifiClient(WiFiClient* client);
};


class IotoroClientESP8266: IotoroClient
{
    private:
        virtual void generateIv(uint8_t iv[AES_BLOCKLEN]);

    public:
        IotoroClientESP8266(const char* deviceId, const char* deviceKey, WiFiClient* client);
        IotoroClientESP8266(const char* deviceId, const char* deviceKey,
                            WiFiClient* client, OPERATION_MODE mode);

        void test()
        {
            IotoroClient::test();
        }

};

#endif