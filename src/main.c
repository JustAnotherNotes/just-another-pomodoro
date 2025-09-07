#include <stdio.h>  // printf(); fflush(); stdout;
#include <stdlib.h> // system(); EXIT_SUCCESS;
#include <stdbool.h>
#include <sys/ioctl.h> // ioctl(); struct winsize; TIOCGWINSZ;
#include <unistd.h>    // STDIN_FILENO; STDOUT_FILENO;
#include <threads.h>
#include <termios.h> // tcgetattr(); tcsetattr(); struct termios; TCSAFLUSH; ICANON; ECHO

#define ESC "\033"
#define CSI "["       // Control Sequence Introducer
#define HOME_POS "H"  // moves cursor to home position (0, 0)
#define NEXT_LINE "E" // moves cursor to beginning of next line, # lines down
#define PREV_LINE "F" // moves cursor to beginning of previous line, # lines up
#define ERASE_LINE "2K"
#define ERASE_SCR "2J"
#define CARRIAGE_RETURN "\r"
#define BELL "\a"

const int INFO_LINES_NUM = 4;

const int WORK_LEN_SEC = 25 * 60;
const int REST_LEN_SEC = 5 * 60;

bool is_alive = true;
struct timespec framerate = {.tv_nsec = 33000000}; // 33 ms (30 fps)

typedef struct
{
    bool is_active;
    int time_len;
    int time_cur;
    int complete_count;
} pomodoro_t;

pomodoro_t p[20]; // 10 hours * 2 pomodoro
int p_index = 0;

typedef enum
{
    WORK,
    REST
} type;

type t = REST;

char *last_action = "none";

struct termios original;

char *type_to_str(type type)
{
    char *s = "";
    switch (type)
    {
    case WORK:
        s = "Work";
        break;
    case REST:
        s = "Rest";
        break;
    }
    return s;
}

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

    atexit(&disable_raw_mode);

    // Turn off canonical mode
    // Turn off ECHO mode so that keyboard is not printing to the terminal
    // ICANON and ECHO is bitflag. ~ is binary NOT operator
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int timer(void *arg)
{
    struct timespec sleep = {.tv_sec = 1};

    while (is_alive)
    {
        if (p[p_index].time_cur > 0)
        {
            p[p_index].time_cur -= 1;
        }
        thrd_sleep(&sleep, NULL);
    }

    thrd_exit(EXIT_SUCCESS);
}

int input(void *arg)
{
    char input;

    enable_raw_mode();
    while (is_alive)
    {
        input = getchar();

        if (input == 's')
        {
            if (p[p_index].is_active)
                continue;

            if (t == WORK)
            {
                t = REST;
                p[p_index].time_len = REST_LEN_SEC;
                p[p_index].time_cur = REST_LEN_SEC;
            }
            else // REST
            {
                t = WORK;
                p[p_index].time_len = WORK_LEN_SEC;
                p[p_index].time_cur = WORK_LEN_SEC;
            }
            p[p_index].is_active = true;

            last_action = "start";
        }
        else if (input == 'k')
        {
            p[p_index].time_cur = 0;
            p[p_index].is_active = false;

            last_action = "skip";
        }
        else if (input == 'n')
        {
            if (p[p_index].is_active)
                continue;

            p_index++;
            t = REST;

            last_action = "next";
        }
        else if (input == 'r')
        {
            if (p[p_index].is_active)
                continue;

            for (int i = 0; i <= p_index; i++)
            {
                p[i].is_active = false;
                p[i].time_cur = 0;
                p[i].complete_count = 0;
            }
            p_index = 0;

            last_action = "reset";
        }
        else if (input == 'q')
        {
            // TODO why so long?
            is_alive = false;

            last_action = "quit";
        }
    }

    thrd_exit(EXIT_SUCCESS);
}

int draw(void *arg)
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    int min;
    int sec;
    char *min_leading = "";
    char *sec_leading = "";

    char *type_str = "";

    // TODO blink if less then 20, need to fix
    float prog_max = window.ws_col - 20;
    float prog_cur;

    int i;

    while (is_alive)
    {
        if (p[p_index].time_cur > 0)
        {
            min = p[p_index].time_cur / 60;
            sec = p[p_index].time_cur % 60;

            if (min < 10)
            {
                min_leading = "0";
            }
            else
            {
                min_leading = "";
            }
            if (sec < 10)
            {
                sec_leading = "0";
            }
            else
            {
                sec_leading = "";
            }

            type_str = type_to_str(t);

            prog_cur = p[p_index].time_cur * prog_max / p[p_index].time_len;

            printf("%s %s%d:%s%d [", type_str, min_leading, min, sec_leading, sec);
            for (int i = 0; i < prog_cur; i++)
            {
                printf("|");
            }
            for (int j = prog_cur; j < prog_max; j++)
            {
                printf(" ");
            }
            printf("]\n");
        }
        else
        {
            printf(ESC CSI ERASE_LINE);
            if (t == WORK)
            {
                printf("Completed. Rest next.\n");
            }
            else // REST
            {
                printf("Completed. Work next.\n");
            }

            if (p[p_index].is_active)
            {
                p[p_index].is_active = false;
                if (t == WORK)
                {
                    p[p_index].complete_count += 1;
                }

                printf(BELL);
                // Require 'libnotify-bin'
                system("notify-send 'jap' 'Interval completed'");
            }
        }

        printf(ESC CSI ERASE_LINE);
        printf("Pomodoros: ");
        for (i = 0; i <= p_index; i++)
        {
            printf("%d ", p[i].complete_count);
        }
        printf("\n");

        printf("'s'tart | s'k'ip | 'n'ext | 'r'eset | 'q'uit\n");

        printf(ESC CSI ERASE_LINE);
        printf("(last action: %s)\n", last_action);

        fflush(stdout);
        thrd_sleep(&framerate, NULL);
        printf(ESC CSI "%d" PREV_LINE, INFO_LINES_NUM);
    }

    thrd_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    thrd_t t_draw;
    thrd_t t_input;
    thrd_t t_timer;

    thrd_create(&t_draw, draw, NULL);
    thrd_create(&t_input, input, NULL);
    thrd_create(&t_timer, timer, NULL);

    while (true)
    {
        if (!is_alive)
        {
            thrd_detach(t_draw);
            thrd_detach(t_input);
            thrd_detach(t_timer);
            break;
        }
        thrd_sleep(&framerate, NULL);
    }

    disable_raw_mode();
    // printf(ESC CSI ERASE_SCR ESC CSI HOME_POS);
    printf(ESC CSI "%d" NEXT_LINE, INFO_LINES_NUM);

    thrd_exit(EXIT_SUCCESS);
    return 0;
}
