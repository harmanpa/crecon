#include "crecon_impl.h"

uint32_t bytes_to_int(char* bytes) {
    return (((uint32_t) bytes[0]) << 24) + (((uint32_t) bytes[1]) << 16) + (((uint32_t) bytes[2]) << 8) + (uint32_t) bytes[3];
}

void int_to_bytes(uint32_t i, char* bytes) {
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;
}


