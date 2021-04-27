#include "iotoro_esp8266.h"
#include "ESP8266mDNS.h"

 /* -- Connection -- */

int IotoroConnectionESP8266::doConnect()
{
    return client->connect(hostIp, hostPort);
}

int IotoroConnectionESP8266::doDisconnect()
{
    client->stop();
    return 1;
}

int IotoroConnectionESP8266::doWrite(const char* data, uint16_t len)
{
    return client->write(data, len);
}

int IotoroConnectionESP8266::doRead()
{
    return 1;
}

IotoroConnectionESP8266::IotoroConnectionESP8266()
{
    MDNS.

    //constchar *ipp = ip.toString().c_str();
}

void IotoroConnectionESP8266::setWifiClient(WiFiClient* client)
{
    this->client = client;
}


/* -- Client -- */

void IotoroClientESP8266::generateIv(uint8_t iv[AES_BLOCKLEN])
{
    for(uint8_t i = 0; i < AES_BLOCKLEN; i++)
        iv[i] = rand() % 256;
}


// Create an instance of the connection.
IotoroConnectionESP8266* iotoroESP8266Con = new IotoroConnectionESP8266();


IotoroClientESP8266::IotoroClientESP8266(const char* deviceId, const char* deviceKey, WiFiClient* client)
    : IotoroClient(deviceId, deviceKey, (IotoroConnection *) iotoroESP8266Con)
{
    iotoroESP8266Con->setWifiClient(client);
}

IotoroClientESP8266::IotoroClientESP8266(const char* deviceId, const char* deviceKey,
                                         WiFiClient* client, OPERATION_MODE mode)
    : IotoroClient(deviceId, deviceKey, (IotoroConnection *) iotoroESP8266Con, mode)
{
    iotoroESP8266Con->setWifiClient(client);
}
