#ifndef __SIMPLE_LZ_IMPLEMENTATION_H__
#define __SIMPLE_LZ_IMPLEMENTATION_H__

#include <stdint.h>

uint32_t encode(const uint8_t *i, uint32_t isz, uint8_t *o, uint8_t w);
uint32_t decode(const uint8_t *i, uint8_t *o);

#endif /* __SIMPLE_LZ_IMPLEMENTATION_H__ */