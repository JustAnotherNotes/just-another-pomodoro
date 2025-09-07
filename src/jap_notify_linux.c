#include <stdbool.h>
#include <libnotify/notify.h> // Require 'libnotify-bin'

#include "jap_notify.h"

#define BELL "\a"

NotifyNotification *notifications[20]; // 10 hours * 2 pomodoro
int n_size = 0;

void notify_clean()
{
    for (int i = 0; i < n_size; i++)
    {
        NotifyNotification *n = notifications[i];
        notify_notification_close(n, NULL);
        g_object_unref(G_OBJECT(n));
    }

    notify_uninit();
}

bool notify_user(char *title, char *msg)
{
    printf(BELL);

    if (!notify_is_initted())
    {
        notify_init("Just Another Pomodoro");
        atexit(&notify_clean);
    }

    NotifyNotification *n = notify_notification_new(title, msg, NULL);
    notifications[n_size++] = n;
    notify_notification_show(n, NULL);

    return true;
}
