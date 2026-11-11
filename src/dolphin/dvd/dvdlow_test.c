#include <dolphin.h>
#include <dolphin/dvd.h>

extern OSAlarm AlarmForBreak;
extern OSAlarm AlarmForTimeout;

int __DVDLowTestAlarm(const OSAlarm* alarm) {
    if (alarm == &AlarmForBreak) {
        return 1;
    }
    if (alarm == &AlarmForTimeout) {
        return 1;
    }
    return 0;
}
