#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

#define DEBUG 1

#include "iotoro.h"
#include "iotoro_linux.h"

const char* deviceId = "6e795abf2d397b25";
const char* deviceKey = "2a7056cfc228272140867b6a69a50d81";


uint8_t sensor1 = 0;
int8_t sensor2 = 1;


IotoroClientLinux cona(deviceId, deviceKey);

 
char c[] = "13feffeb83805790";

int main() { 
    //AES_init_ctx_iv(&aes, key, iv);
    //AES_CBC_encrypt_buffer(&aes, (unsigned char*) msg, 32);
    cona.test();

    std::cout << "test over" << std::endl;

    // while (1);

}  


/*
    con.setParam("X1", &sensor1);
    con.setParam("X2", &sensor2);
    sensor1 = 2;
    sensor2 = 4;
*/