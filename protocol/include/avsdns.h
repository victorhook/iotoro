#ifndef AVSDNS_H
#define AVSDNS_H

#include "ESP8266WiFi.h"
#include <iostream>
#include <string.h>


#define AVSDNS_TYPE_A 1
#define AVSDNS_CLAS_IN 1

#define AVSDNS_QR 0
#define AVSDNS_OPCODE 0
#define AVSDNS_AA 0
#define AVSDNS_TC 0
#define AVSDNS_RD 1
#define AVSDNS_RA 0
#define AVSDNS_Z 0
#define AVSDNS_RCODE 0

#define AVSDNS_FIRST_BYTE ((AVSDNS_QR << 7) | (AVSDNS_OPCODE << 6) | (AVSDNS_AA << 2) | (AVSDNS_TC << 1) | (AVSDNS_RD << 0))
#define AVSDNS_SECOND_BYTE ((AVSDNS_RA << 7) | (AVSDNS_Z << 6) | (AVSDNS_RCODE << 3))

#define AVSDNS_QUESTIONS 1
#define AVSDNS_ANSWERS 0
#define AVSDNS_AUTH_RSS 0
#define AVSDNS_ADD_RSS 0

#define DNS_DEFAULT_HEADER_SIZE 12
#define DNS_DEFAULT_PACKET_SIZE 16
#define DNS_DEFAULT_ENDING_SIZE 4

// Request specific
#define AVSDNS_TYPE AVSDNS_TYPE_A
#define AVSDNS_CLASS AVSDNS_CLAS_IN

typedef struct {
    uint8_t first_byte;
    uint8_t second_byte;
    uint16_t questions;
    uint16_t answers;
    uint16_t auth_rrs;
    uint16_t add_rrs;
} DNSHeader;

typedef struct {
    uint16_t txId;
    DNSHeader header;
    uint8_t* query;
    uint16_t type;
    uint16_t cls;
    uint16_t totalSize;
    uint16_t dataSize;
} DNSPacket;


class DNSClient {
    private:
        WiFiClient* client;
        const char* dnsServer;
        void fillDNSPacket(DNSPacket* packet, size_t dataSize);
        void sendPacket(uint8_t* buf, size_t len);
        void fillBuffer(uint8_t* buf, const DNSPacket* packet);

    public:
        DNSClient(WiFiClient* client);
        void send(const char* url, const size_t len);
};


#endif