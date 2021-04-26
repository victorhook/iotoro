#include "avsdns.h"


/* -- Helper methods -- */
static void asHex(uint8_t* data, uint16_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%.2x ", data[i]);
    }
}

/* Turns a url like. www.google.com to -> www3google6com */
static void buildUrlName(const char* url, size_t len, uint8_t* name)
{
    uint8_t urlCounter[3] = {0, 0, 0};
    uint8_t index = 0;

    for (char* p = (char*) url; p < url + len; p++) {
        if (*p == '.')
            index++;
        else
            urlCounter[index]++;
    }

    size_t i = 0;

    for (uint8_t round = 0; round < 3; round++) {
        name[i++] = urlCounter[round];

        for (uint8_t j = 0; j < urlCounter[round]; j++) {
            name[i + j] = url[i + j - 1];
        }

        i += urlCounter[round];
    }

    name[i] = 0;
}

static uint16_t generateId()
{
    return rand() % 0xffff;
}

/* -- Private -- */


void DNSClient::sendPacket(uint8_t* buf, size_t len)
{
    printf("Sending packet...\n");
    asHex(buf, len);
    client->connect(dnsServer, 53);
    client->write(buf, len);
}


void DNSClient::fillBuffer(uint8_t* buf, const DNSPacket* packet)
{
    size_t i = 0;
    
    memcpy(buf, (const char*) packet, DNS_DEFAULT_HEADER_SIZE);
    i += (uint16_t) DNS_DEFAULT_HEADER_SIZE;
    memcpy(buf + i, (const char*) packet->query, packet->dataSize);
    i += packet->dataSize + 1;
    *(uint16_t*) (buf + i) = packet->type;
    *(uint16_t*) (buf + i + 2) = packet->cls;
}

void DNSClient::fillDNSPacket(DNSPacket* packet, size_t dataSize)
{
    // Id
    packet->txId = generateId();

    // Header
    packet->header.first_byte = AVSDNS_FIRST_BYTE;
    packet->header.second_byte = AVSDNS_SECOND_BYTE;
    packet->header.questions = AVSDNS_QUESTIONS;
    packet->header.answers = AVSDNS_ANSWERS;
    packet->header.auth_rrs = AVSDNS_AUTH_RSS;
    packet->header.add_rrs = AVSDNS_ADD_RSS;

    // Type and class
    packet->type = AVSDNS_TYPE;
    packet->cls = AVSDNS_CLASS;

    // Size params 
    packet->totalSize = (uint16_t) (DNS_DEFAULT_PACKET_SIZE + dataSize);
    packet->dataSize = dataSize;
}

/* -- Public -- */

DNSClient::DNSClient(WiFiClient* client)
{
    this->client = client;
}

void DNSClient::send(const char* url, const size_t len)
{
    uint16_t urlSize = len + 2;
    uint8_t query[urlSize];

    buildUrlName(url, len, query);

    DNSPacket packet;
    packet.query = (uint8_t*) query;
    fillDNSPacket(&packet, urlSize);


    uint8_t buf[packet.totalSize];
    fillBuffer(buf, &packet);
    sendPacket(buf, packet.totalSize);
}
