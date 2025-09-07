#include <termios.h> // tcgetattr(); tcsetattr(); struct termios; TCSAFLUSH; ICANON; ECHO
#include <unistd.h> // STDIN_FILENO;

#include "jap_term_ctl.h"

struct termios original;

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

void enable_raw_mode()
{
    struct termios raw;

    // Save the state of the terminal
    tcgetattr(STDIN_FILENO, &raw);
    tcgetattr(STDIN_FILENO, &original);

    // Turn off canonical mode
    // Turn off ECHO mode so that keyboard is not printing to the terminal
    // ICANON and ECHO is bitflag. ~ is binary NOT operator
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
