#include <stdio.h>  // printf(); fflush(); stdout;
#include <stdlib.h> // system(); EXIT_SUCCESS;
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "jap_term_ctl.h"
#include "jap_winsize.h"
#include "jap_notify.h"

#define ESC "\033"
#define CSI "["       // Control Sequence Introducer
#define HOME_POS "H"  // moves cursor to home position (0, 0)
#define NEXT_LINE "E" // moves cursor to beginning of next line, # lines down
#define PREV_LINE "F" // moves cursor to beginning of previous line, # lines up
#define ERASE_LINE "2K"
#define ERASE_SCR "2J"

const int INFO_LINES_NUM = 4;

// const int WORK_LEN_SEC = 25 * 60;
// const int REST_LEN_SEC = 5 * 60;
const int WORK_LEN_SEC = 4;
const int REST_LEN_SEC = 2;

bool is_alive = true;
struct timespec framerate = { .tv_nsec = 33000000 }; // 33 ms (30 fps)

typedef struct {
    bool is_active;
    int time_len;
    int time_cur;
    int complete_count;
} pomodoro_t;

int p_capacity = 20; // 10 hours * 2 pomodoro
int p_index = 0;
pomodoro_t *p;

typedef enum {
    WORK,
    REST
} type;

type t = REST;

char *last_action = "none";

char *type_to_str(type type) {
    char *s = "";
    switch (type) {
        case WORK:
            s = "Work";
            break;
        case REST:
            s = "Rest";
            break;
    }
    return s;
}

void *timer(void *arg) {
    struct timespec sleep = {.tv_sec = 1};

    while (is_alive) {
        if (p[p_index].time_cur > 0) {
            p[p_index].time_cur -= 1;
        }
        nanosleep(&sleep, NULL);
    }

    return NULL;
}

void *input(void *arg) {
    char input;

    enable_raw_mode();
    atexit(&disable_raw_mode);

    while (is_alive) {
        input = getchar();

        if (input == 's') {
            if (p[p_index].is_active) continue;

            if (t == WORK) {
                t = REST;
                p[p_index].time_len = REST_LEN_SEC;
                p[p_index].time_cur = REST_LEN_SEC;
            } else { // REST
                t = WORK;
                p[p_index].time_len = WORK_LEN_SEC;
                p[p_index].time_cur = WORK_LEN_SEC;
            }
            p[p_index].is_active = true;

            last_action = "start";
        } else if (input == 'k') {
            if (p[p_index].is_active) {
                p[p_index].time_cur = 0;
                p[p_index].is_active = false;
            } else {
                if (t == WORK) {
                    t = REST;
                } else {
                    t = WORK;
                }
            }

            last_action = "skip";
        } else if (input == 'n') {
            if (p[p_index].is_active) continue;

            p_index++;
            if (p_index == p_capacity) {
                int old_cap = p_capacity;
                p_capacity *= 2;
                p = realloc(p, p_capacity * sizeof(pomodoro_t));
                for (int i = old_cap; i < p_capacity; i++) {
                    p[i] = (pomodoro_t){0};
                }
            }
            t = REST;

            last_action = "next";
        } else if (input == 'r') {
            if (p[p_index].is_active) continue;

            for (int i = 0; i <= p_index; i++) {
                p[i].is_active = false;
                p[i].time_cur = 0;
                p[i].complete_count = 0;
            }
            p_index = 0;
            t = REST;

            last_action = "reset";
        } else if (input == 'q') {
            is_alive = false;

            last_action = "quit";
        }
    }

    return NULL;
}

void *draw(void *arg) {
    jap_winsize_t winsize;
    get_winsize(&winsize);

    int min;
    int sec;
    char *min_leading = "";
    char *sec_leading = "";

    char *type_str = "";

    const int prog_max = winsize.cols - 14;
    int prog_cur;

    int i;

    while (is_alive) {
        if (p[p_index].time_cur > 0) {
            min = p[p_index].time_cur / 60;
            sec = p[p_index].time_cur % 60;

            if (min < 10) {
                min_leading = "0";
            } else {
                min_leading = "";
            }
            if (sec < 10) {
                sec_leading = "0";
            } else {
                sec_leading = "";
            }

            type_str = type_to_str(t);

            prog_cur = p[p_index].time_cur * prog_max / p[p_index].time_len;

            printf("%s %s%d:%s%d [", type_str, min_leading, min, sec_leading, sec);
            for (int i = 0; i < prog_cur; i++) {
                printf("|");
            }
            for (int j = prog_cur; j < prog_max; j++) {
                printf(" ");
            }
            printf("]\n");
        } else {
            printf(ESC CSI ERASE_LINE);
            if (t == WORK) {
                printf("Completed. Rest next.\n");
            } else { // REST
                printf("Completed. Work next.\n");
            }

            if (p[p_index].is_active) {
                p[p_index].is_active = false;
                if (t == WORK) {
                    p[p_index].complete_count += 1;
                    jap_notify_show(
                        "Work interval completed", "Take some rest");
                } else { // REST
                    jap_notify_show(
                        "Rest interval completed", "Prepare to work");
                }
            }
        }

        printf(ESC CSI ERASE_LINE);
        printf("Pomodoros: ");
        for (i = 0; i <= p_index; i++) {
            if (i < winsize.cols / 2.4) {
                printf("%d ", p[i].complete_count);
            } else {
                printf("...");
                break;
            }
        }
        printf("\n");

        printf("'s'tart | s'k'ip | 'n'ext | 'r'eset | 'q'uit\n");

        printf(ESC CSI ERASE_LINE);
        printf("(last action: %s)\n", last_action);

        fflush(stdout);
        nanosleep(&framerate, NULL);
        printf(ESC CSI "%d" PREV_LINE, INFO_LINES_NUM);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    p = calloc(p_capacity, sizeof(pomodoro_t));
    jap_notify_init();

    pthread_t t_draw;
    pthread_t t_input;
    pthread_t t_timer;

    pthread_create(&t_draw, NULL, draw, NULL);
    pthread_create(&t_input, NULL, input, NULL);
    pthread_create(&t_timer, NULL, timer, NULL);

    while (true) {
        if (!is_alive) {
            pthread_detach(t_draw);
            pthread_detach(t_input);
            pthread_detach(t_timer);
            break;
        }
        nanosleep(&framerate, NULL);
    }

    // printf(ESC CSI ERASE_SCR ESC CSI HOME_POS);
    printf(ESC CSI "%d" NEXT_LINE, INFO_LINES_NUM);

    return 0;
}
