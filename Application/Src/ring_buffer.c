#include "ring_buffer.h"
#include <stdlib.h>
#include <string.h>

struct _rbuf_ {
    uint8_t* data;
    size_t size;
    size_t count;
};

rbuf* rbuf_init(uint8_t* data, size_t size) {
    rbuf* ret_val = NULL;
    ret_val = malloc(sizeof(rbuf));
    if(ret_val) {
        memset((void*)data, 0, size);
        ret_val->data = data;
        ret_val->size = size;
        ret_val->count = 0;
    }
    return ret_val;
}

void rbuf_push(rbuf* buffer, uint8_t byte) {
    buffer->data[buffer->count] = byte;
    buffer->count = (buffer->count + 1) % buffer->size;
}

void rbuf_reset(rbuf* buffer) {
    buffer->count = 0;
}
