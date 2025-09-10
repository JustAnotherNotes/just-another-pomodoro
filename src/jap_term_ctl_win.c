#include <windows.h>

#include "jap_term_ctl.h"

static DWORD original;

void disable_raw_mode() {
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(stdin_handle, original);
}

void enable_raw_mode() {
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD raw;

    // Save original state
    GetConsoleMode(stdin_handle, &original);

    raw = original;

    // Disable line input (ENABLE_LINE_INPUT) and echo (ENABLE_ECHO_INPUT)
    // Disable processed input so Ctrl+C isn't translated into signals
    raw &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);

    SetConsoleMode(stdin_handle, raw);

    atexit(disable_raw_mode);
}
