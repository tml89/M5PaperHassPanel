#ifndef _CONFIG_H_
#define _CONFIG_H_

// Wifi: SSID and password
const char *WIFI_SSID = "Obi Wlan Kenobi";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "M5PaperHassPanel";
const PROGMEM char* MQTT_SERVER_IP = "192.168.178.9";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "[Redacted]";
const PROGMEM char* MQTT_PASSWORD = "[Redacted]";

const PROGMEM char* MQTT_BASETOPIC = "m5paper/";

const u_int32_t SLEEPTIMER = 60; //Sleep in Seconds after inaktivity

#endif
