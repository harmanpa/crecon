#include "crecon_impl.h"

uint32_t recon_util_bytes_to_int(unsigned char* bytes) {
    return (((uint32_t) bytes[0]) << 24) + (((uint32_t) bytes[1]) << 16) + (((uint32_t) bytes[2]) << 8) + (uint32_t) bytes[3];
}

void recon_util_int_to_bytes(uint32_t i, char* bytes) {
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;
}

void recon_util_tic(void** t) {
    //gettimeofday()
}

long recon_util_toc(void* t) {
    return 0L;
}

int recon_util_digits(int n) {
    int count = 1;
    while (n != 0) {
        n /= 10;
        ++count;
    }
    return count;
}
/*

void recon_ibuffer_append(recon_ibuffer* buffer, int i) {
    
}

*/
