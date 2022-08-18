#include "system_util.h"

system_util::system_util()
{
}

uint8_t system_util::connect_wifi(const char *ssid, const char *passphrase)
{
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, passphrase);

    unsigned long start = millis();
    uint8_t connectionStatus;
    bool AttemptConnection = true;
    while (AttemptConnection)
    {
        connectionStatus = WiFi.status();
        if (millis() > start + 2000)
        { // Wait 2-secs maximum
          AttemptConnection = false;
        }
        if (connectionStatus == WL_CONNECTED || connectionStatus == WL_CONNECT_FAILED)
        {
          AttemptConnection = false;
        }
        delay(50);
    }
    if (connectionStatus == WL_CONNECTED)
    {
        //wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        Serial.println("WiFi connected at: " + WiFi.localIP().toString());
    }
    else
    {
        Serial.println("WiFi connection *** FAILED ***");
    }
    return connectionStatus;
   
}

void system_util::reboot()
{
    Serial.println("reboot.");
    esp_restart();
}
