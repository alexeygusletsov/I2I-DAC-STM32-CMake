#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>
#include <stddef.h>

typedef struct _rbuf_ rbuf;

rbuf* rbuf_init(uint8_t* data, size_t size);
void rbuf_push(rbuf* buffer, uint8_t byte);
void rbuf_reset(rbuf* buffer);

#endif // _RING_BUFFER_H_