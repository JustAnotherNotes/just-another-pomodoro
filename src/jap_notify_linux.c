#include <stdbool.h>
#include <libnotify/notify.h> // Require 'libnotify-bin'

#include "jap_notify.h"

NotifyNotification *notifications[20]; // 10 hours * 2 pomodoro
int n_capacity = 20;
int n_size = 0;
// NotifyNotification **notifications = malloc(sizeof(NotifyNotification*) * n_capacity);

void jap_clean();

bool jap_notify_init() {
    if (!notify_is_initted()) {
        notify_init("Just Another Pomodoro");
        atexit(&jap_clean);
    }
    return true;
}

bool jap_notify_show(char *title, char *msg) {
    printf(BELL);

    NotifyNotification *n = notify_notification_new(title, msg, NULL);
    notifications[n_size++] = n;
    notify_notification_show(n, NULL);

    return true;
}

void jap_clean() {
    for (int i = 0; i < n_size; i++) {
        NotifyNotification *n = notifications[i];
        notify_notification_close(n, NULL);
        g_object_unref(G_OBJECT(n));
    }

    notify_uninit();
}
