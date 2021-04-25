#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

#define DEBUG 1

#include "iotoro.h"
#include "iotoro_linux.h"

const char* deviceId = "13feffeb83805790";
const char* deviceKey = "c0c04877f74d69c68e120d45ddca18d0";


IotoroClientLinux cona(deviceId, deviceKey);

#include "aes.h"


int main() { 
    cona.test();

    // while (1);
}  


/*
    con.setParam("X1", &sensor1);
    con.setParam("X2", &sensor2);
    sensor1 = 2;
    sensor2 = 4;
        //AES_ctx aes;
    //AES_init_ctx_iv(&aes, devKey, (const uint8_t *) iv);
    //printVector(data);
    //AES_CBC_encrypt_buffer(&aes, (unsigned char*) data, 16); 
*/