#ifndef _SYSTEM_UTIL_H_
#define _SYSTEM_UTIL_H_

#include <M5EPD.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include "../data_struct.h"

class system_util
{
public:
    system_util();

    uint8_t connect_wifi(const char *ssid, const char *passphrase);
    void reboot();

private:
};

#endif
