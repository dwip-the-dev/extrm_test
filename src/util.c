#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* xmalloc(size_t n) {
    void* p = malloc(n);
    if (!p) {
        perror("malloc");
        exit(1);
    }
    return p;
}

uint8_t* read_file(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    uint8_t* buf = xmalloc(size);
    fread(buf, 1, size, f);
    fclose(f);

    *out_size = size;
    return buf;
}

void write_u32(FILE* f, uint32_t v) {
    fwrite(&v, sizeof(v), 1, f);
}

void write_u64(FILE* f, uint64_t v) {
    fwrite(&v, sizeof(v), 1, f);
}

/*
 * VERY SIMPLE RLE
 * [count][byte]
 * count = 1..255
 */
uint8_t* rle_compress(uint8_t* data, size_t in_size, size_t* out_size) {
    uint8_t* out = xmalloc(in_size * 2);
    size_t w = 0;

    for (size_t i = 0; i < in_size;) {
        uint8_t b = data[i];
        uint8_t count = 1;

        while (i + count < in_size && data[i + count] == b && count < 255)
            count++;

        out[w++] = count;
        out[w++] = b;
        i += count;
    }

    *out_size = w;
    return out;
}
