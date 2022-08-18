#ifndef _CONFIG_H_
#define _CONFIG_H_

// Wifi: SSID and password
const char *WIFI_SSID = "ssid";
const char *WIFI_PASSWORD  = "password";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "M5PaperHassPanel";
const PROGMEM char* MQTT_SERVER_IP = "[Redacted]";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "[Redacted]";
const PROGMEM char* MQTT_PASSWORD = "[Redacted]";

#endif
