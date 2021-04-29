#define DEBUG 1
#include "iotoro_linux.h"

const char* deviceId = "13feffeb83805790";
const char* deviceKey = "c0c04877f74d69c68e120d45ddca18d0";


IotoroClientLinux client(deviceId, deviceKey);

uint8_t sensor1 = 0, sensor2 = 3;

void k(int& i)
{
    printf("\n\n%d \n\n", i);
}

class Test{
    int& sens;
    public:
        Test(int& sensor) : sens(sensor) {}
        void k()
        {
            printf("\n\n%d \n\n", sens);
        }
};

int main() {
    client.setParam("X1", sensor1);
    client.setParam("X2", sensor2);
    //client.printParams();
    //client.sendParams();
    client.send(IOTORO_WRITE_UP);
    //client.read();

    sensor2 = 11;
    //client.send(IOTORO_WRITE_UP);



    // while (1);
}   



/*
    
    con.setParam("X2", &sensor2); 
    sensor1 = 2;
    sensor2 = 4;
        //AES_ctx aes;
    //AES_init_ctx_iv(&aes, devKey, (const uint8_t *) iv);
    //printVector(data);
    //AES_CBC_encrypt_buffer(&aes, (unsigned char*) data, 16); 
*/