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

#define RECON_OBJECT_BUFFER_SIZE 100

recon_status recon_object_buffer_create(recon_object_buffer** buffer) {
    *buffer = (recon_object_buffer*) malloc(sizeof (recon_object_buffer*));
    (*buffer)->data = (msgpack_object*) malloc(RECON_OBJECT_BUFFER_SIZE * sizeof (msgpack_object));
    (*buffer)->alloc = RECON_OBJECT_BUFFER_SIZE;
    (*buffer)->size = 0;
    return RECON_OK;
}

recon_status recon_object_buffer_size(recon_object_buffer* buffer, int* size) {
    *size = buffer->size;
    return RECON_OK;
}

recon_status recon_object_buffer_get(recon_object_buffer* buffer, int i, msgpack_object* obj) {
    if (i < 0 || i >= buffer->size) {
        return RECON_NOT_FOUND;
    }
    msgpack_object_print(stderr, buffer->data[i]);
    *obj = buffer->data[i];
    return RECON_OK;
}

recon_status recon_object_buffer_append(recon_object_buffer* buffer, msgpack_object object) {
    if (buffer->alloc - buffer->size < 1) {
        size_t nsize = (buffer->alloc) ? buffer->alloc * 2 : RECON_OBJECT_BUFFER_SIZE;
        void* tmp = realloc(buffer->data, nsize);
        if (!tmp) {
            return RECON_BUFFER_RESIZE_ERROR;
        }
        buffer->data = (msgpack_object*) tmp;
        buffer->alloc = nsize;
    }
    memcpy(buffer->data+buffer->size, &object, sizeof(msgpack_object));
//    buffer->data[buffer->size] = object;
    buffer->size++;
    return RECON_OK;
}

recon_status recon_object_buffer_destroy(recon_object_buffer* buffer) {
    free(buffer->data);
    return RECON_OK;
}
