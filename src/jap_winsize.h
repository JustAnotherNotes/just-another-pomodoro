#ifndef JAP_WINSIZE_H
#define JAP_WINSIZE_H

#include <stdbool.h>

typedef struct {
    int rows;
    int cols;
} jap_winsize_t;

bool get_winsize(jap_winsize_t *winsize);

#endif
