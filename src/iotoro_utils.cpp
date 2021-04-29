#include "iotoro.h"
#include "md5.h"

/* Returns the byte value 0-255 from an ascii. */
uint8_t getAsciiValue(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    } else {
        return c - '0';
    }
}

/* Returns the ascii representation of a byte. */
char valueToAscii(uint8_t val)
{
    if (val <  10) {
        return '0' + val;
    } else {
        #ifdef HEXIFLY_BIG_CHARS
            char offset = 'A';
        #else
            unsigned char offset = 'a';
        #endif
        return offset + (val - 10);
    }
}

/* Turns a stream of hexified ascii characters to stream of bytes, */
void unHexifly(const char* from, char* to, size_t len)
{
    size_t i = 0, j = 0;
    while (i < len) {
        to[i] = getAsciiValue(from[j]) * 16 + getAsciiValue(from[j + 1]);
        j += 2;
        i++;
    }
}

/* Turns a stream of bytes into the ascii HEX representation. */
void hexifly(const char* from, char* to, size_t len)
{
    size_t i = 0, j = 0;
    while (i < len) {
        uint8_t first = (from[i] & 0xf0) >> 4;
        uint8_t second = from[i] & 0x0f;

        to[j++] = valueToAscii(first);
        to[j++] = valueToAscii(second);

        i++;
    }
}

/* Performs md5sum of the given input. */
void md5sum(const char* from, char to[16], size_t len)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, from, len);
    MD5_Final((unsigned char*) to, &ctx);
}

/*  Returns the size of a parmeter type. 
    Note that these sizes are pre-decided, to ensure correct parsing
    on the server side.
*/
uint8_t getParamTypeSize(PARAM_TYPE type)
{
    if (type == PARAM_BOOL || type == PARAM_UINT_8 || type == PARAM_INT_8)
        return 1;
    else if (type == PARAM_UINT_16 || type == PARAM_INT_16)
        return 2;
    else if (type == PARAM_UINT_32 || type == PARAM_INT_32 || type == PARAM_FLOAT)
        return 4;
    else if (type == PARAM_UINT_64 || type == PARAM_INT_64 || type == PARAM_DOBULE)
        return 8;
    return -1;
}

/* Returns the size of a parameter settings. */
uint8_t getParamSettingSize(Param param)
{
    return IOTORO_MAX_PARAM_NAME_SIZE
           + getParamTypeSize(param.type)
           + IOTORO_PARAM_TYPE_SIZE;
}

/* Puts the parameter data into the buffer, with the correct size and format. */
void putParamData(char* buff, const Param& param)
{
    switch (param.type) {
        case PARAM_BOOL: 
            *((bool*) buff) = *param.paramPtr.b;
            break;
        case PARAM_UINT_8: 
            *((uint8_t*) buff) = *param.paramPtr.u8;
            break;
        case PARAM_INT_8: 
            *((int8_t*) buff) = *param.paramPtr.i8;
            break;
        case PARAM_UINT_16: 
            *((uint16_t*) buff) = *param.paramPtr.u16;
            break;
        case PARAM_INT_16: 
            *((int16_t*) buff) = *param.paramPtr.i16;
            break;
        case PARAM_UINT_32: 
            *((uint32_t*) buff) = *param.paramPtr.u32;
            break;
        case PARAM_INT_32: 
            *((int32_t*) buff) = *param.paramPtr.i32;
            break;
        case PARAM_UINT_64: 
            *((uint64_t*) buff) = *param.paramPtr.u64;
            break;
        case PARAM_INT_64: 
            *((int64_t*) buff) = *param.paramPtr.i64;
            break;
        case PARAM_FLOAT: 
            *((float*) buff) = *param.paramPtr.f;
            break;
        case PARAM_DOBULE: 
            *((double*) buff) = *param.paramPtr.d;
            break;
    }
}
