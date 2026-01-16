#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "extrm.h"
#include "util.h"

typedef struct {
    uint8_t* data;
    size_t size;
} Buffer;

Buffer html = {0}, js = {0}, css = {0}, other = {0};

void append(Buffer* b, uint8_t* data, size_t size) {
    b->data = realloc(b->data, b->size + size);
    memcpy(b->data + b->size, data, size);
    b->size += size;
}

int has_ext(const char* name, const char* ext) {
    size_t n = strlen(name), e = strlen(ext);
    return n > e && strcmp(name + n - e, ext) == 0;
}

void walk(const char* path) {
    DIR* d = opendir(path);
    if (!d) return;

    struct dirent* ent;
    while ((ent = readdir(d))) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;

        char full[4096];
        snprintf(full, sizeof(full), "%s/%s", path, ent->d_name);

        struct stat st;
        if (stat(full, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            walk(full);
        } else {
            size_t sz;
            uint8_t* data = read_file(full, &sz);
            if (!data) continue;

            if (has_ext(ent->d_name, ".html"))
                append(&html, data, sz);
            else if (has_ext(ent->d_name, ".js"))
                append(&js, data, sz);
            else if (has_ext(ent->d_name, ".css"))
                append(&css, data, sz);
            else
                append(&other, data, sz);

            free(data);
        }
    }
    closedir(d);
}

void write_stream(FILE* out, uint8_t type, Buffer* buf) {
    if (!buf->size) return;

    size_t comp_size;
    uint8_t* comp = rle_compress(buf->data, buf->size, &comp_size);

    fwrite(&type, 1, 1, out);
    write_u64(out, buf->size);
    write_u64(out, comp_size);
    fwrite(comp, 1, comp_size, out);

    free(comp);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("usage: extrm <folder> <out.extrm>\n");
        return 1;
    }

    walk(argv[1]);

    FILE* out = fopen(argv[2], "wb");
    if (!out) {
        perror("open");
        return 1;
    }

    fwrite(EXTRM_MAGIC, 1, 5, out);
    write_u32(out, EXTRM_VERSION);
    write_u32(out, 4);

    write_stream(out, STREAM_HTML, &html);
    write_stream(out, STREAM_JS, &js);
    write_stream(out, STREAM_CSS, &css);
    write_stream(out, STREAM_OTHER, &other);

    fclose(out);

    printf("EXTRM v1 done.\n");
    printf("HTML: %zu bytes\n", html.size);
    printf("JS:   %zu bytes\n", js.size);
    printf("CSS:  %zu bytes\n", css.size);
    printf("OTHER:%zu bytes\n", other.size);

    return 0;
}
