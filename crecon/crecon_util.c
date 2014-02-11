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

msgpack_object get_array_element(msgpack_object_array array, int i) {
    return array.ptr[i];
}

double as_double(msgpack_object object) {
    switch (object.type) {
        case MSGPACK_OBJECT_DOUBLE:
            return object.via.dec;
        case MSGPACK_OBJECT_POSITIVE_INTEGER:
            return (double) object.via.u64;
        case MSGPACK_OBJECT_NEGATIVE_INTEGER:
            return (double) object.via.i64;
        case MSGPACK_OBJECT_BOOLEAN:
            return object.via.boolean ? 1.0 : 0.0;
    }
    return 0.0;
}

int as_int(msgpack_object object) {
    switch (object.type) {
        case MSGPACK_OBJECT_POSITIVE_INTEGER:
            return (int) object.via.u64;
        case MSGPACK_OBJECT_NEGATIVE_INTEGER:
            return (int) object.via.i64;
        case MSGPACK_OBJECT_BOOLEAN:
            return object.via.boolean ? 1 : 0;
    }
    return 0;
}

recon_booleantype as_boolean(msgpack_object object) {
    switch (object.type) {
        case MSGPACK_OBJECT_POSITIVE_INTEGER:
            if (object.via.u64 > 0) {
                return RECON_TRUE;
            } else {
                return RECON_FALSE;
            }
        case MSGPACK_OBJECT_NEGATIVE_INTEGER:
            if (object.via.i64 > 0) {
                return RECON_TRUE;
            } else {
                return RECON_FALSE;
            }
        case MSGPACK_OBJECT_BOOLEAN:
            if (object.via.boolean) {
                return RECON_TRUE;
            } else {
                return RECON_FALSE;
            }
    }
    return RECON_FALSE;
}

