#include <stdio.h>
#include <windows.h>

#include "jap_winsize.h"

bool get_winsize(jap_winsize_t *winsize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    winsize->rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    winsize->cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return true;
}
