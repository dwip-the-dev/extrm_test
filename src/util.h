#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

uint8_t* read_file(const char* path, size_t* out_size);
void write_u32(FILE* f, uint32_t v);
void write_u64(FILE* f, uint64_t v);
void* xmalloc(size_t n);

uint8_t* rle_compress(uint8_t* data, size_t in_size, size_t* out_size);

#endif
