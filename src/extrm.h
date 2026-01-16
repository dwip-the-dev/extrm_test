#ifndef EXTRM_H
#define EXTRM_H

#include <stdint.h>

#define EXTRM_MAGIC "EXTRM"
#define EXTRM_VERSION 1

enum StreamType {
    STREAM_HTML = 1,
    STREAM_JS   = 2,
    STREAM_CSS  = 3,
    STREAM_OTHER = 4
};

#endif
