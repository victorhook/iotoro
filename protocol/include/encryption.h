#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdint.h>

#define IV_LENGTH 16


/* Encodes the given data with the device key. */
void encryptPacket(const char* deviceId, const char* deviceKey,
                   uint8_t* packet, uint16_t len)
{
    // Generate initialization vector.

    // Create the AES cipher.

    // Add deviceId to the data.

    // Add padding to ensure block size of 16 (AES).

    // Encrypt the data.

}

/* Decodes a packet using AES encryption, given the device key. */
void decryptPacket()
{
    // Turn the data into a packet helper format.

    // Create cypher from given iv and the device key.

    // Decrypt & unpad the data.

    // Pack the info into a packet format.
}


void _fillRandomBytes(uint8_t bytes[16]);
void _generateIv(uint8_t bytes[16]);


#endif