#include <sys/ioctl.h> // ioctl(); struct winsize; TIOCGWINSZ;
#include <unistd.h>    // STDOUT_FILENO;

#include "jap_winsize.h"

bool get_winsize(jap_winsize_t *winsize) {
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    winsize->rows = window.ws_row;
    winsize->cols = window.ws_col;

    return true;
}
