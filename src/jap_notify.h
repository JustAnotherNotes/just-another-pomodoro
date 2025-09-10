#ifndef JAP_NOTIFY_H
#define JAP_NOTIFY_H

#include <stdbool.h>

#define BELL "\a"

bool jap_notify_init();

bool jap_notify_show(char *title, char *msg);

#endif // JAP_NOTIFY_H
