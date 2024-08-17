#include <M5EPD.h>
#include "resources/ImageResource.h"
#include "resources/vacuum.h"
#include "ui/ui_manager.h"
#include "config.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>

#define STATUSBARHEIGHT 35
#define SCREENHEIGHT 960
#define SCREENWIDTH 540
#define ERRORLINEX 189

//forward declaration
String GetTempString(void);

enum DrawMode
{
    kHome = 0,
    kMusic = 1,
    kVacuum = 2,
    kTimer = 3,
    kTV = 4,
    kScreenSaver = 5
};

DrawMode drawMode = kHome;

M5EPD_Canvas canvas(&M5.EPD);
ui_manager UI(&canvas);
WiFiClient wifiClient;
PubSubClient client(wifiClient);
Preferences preferences;

uint32_t startMilis = -1;
RTC_DATA_ATTR int lastTemp = 0;

char const *drawModeEnnumToString(DrawMode iDrawmode)
{
    switch (iDrawmode)
    {
    case kHome:
        return "Home";
    case kMusic:
        return "Music";
    case kVacuum:
        return "Vacuumc";
    case kTimer:
        return "Timer";
    case kTV:
        return "TV";
    case kScreenSaver:
        return "Aus";
    }

    return "unkown";
}

uint8_t connect_wifi(const char *ssid, const char *passphrase)
{
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, passphrase);
    WiFi.setSleep(WIFI_PS_NONE);

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
        // wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        Serial.println("WiFi connected at: " + WiFi.localIP().toString());
    }
    else
    {
        Serial.println("WiFi connection *** FAILED ***");
    }
    return connectionStatus;
}

void mqttConnect()
{
    unsigned long start = millis();
    bool AttemptConnection = true;
    // Loop until we're reconnected
    while (AttemptConnection)
    {
        if (millis() > start + 2000)
        { // Wait 2-secs maximum
            AttemptConnection = false;
        }
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("arduinoClient"))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("MQTT_BASETOPIC", "Connected");
            // ... and resubscribe
            client.subscribe("inTopic");
            AttemptConnection = false;
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 1/2 second before retrying
            delay(500);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");
    Serial.write(payload, length);
    Serial.println();
}

void shutdown(int iSleepTime, bool iDraw = false)
{
    if (iDraw)
    {
        UI.create_label(640, 480, 30, 15, 256, "--> [");
        UI.draw_all(UPDATE_MODE_GC16);
    }

    delay(500);

    if (iSleepTime > 0)
        M5.shutdown(iSleepTime);
    else
        M5.shutdown();

    delay(iSleepTime * 1000);
    ESP.restart();
}
// Update RTC
// ToDo get Time and date from NTP
void UpdateRTC()
{
    rtc_date_t date;

    date.day = 24;
    date.mon = 9;
    date.year = 2022;

    M5.RTC.setDate(&date);

    Serial.printf("Datum: %02d.%02d.%02d", date.day, date.mon, date.year);

    rtc_time_t time;

    time.hour = 11;
    time.min = 16;
    time.sec = 0;

    M5.RTC.setTime(&time);

    Serial.printf("Uhrzeit: %02d:%02d:%02d", time.hour, time.min, time.sec);
}

int GetBattery()
{
    uint32_t vol = M5.getBatteryVoltage();
    Serial.println("Voltage : " + String(vol));

    if (vol < 3300)
    {
        vol = 3300;
    }
    else if (vol > 4350)
    {
        vol = 4350;
    }

    float battery = (float)(vol - 3300) / (float)(4350 - 3300);

    if (battery <= 0.01)
    {
        battery = 0.01;
    }
    if (battery > 1)
    {
        battery = 1;
    }
    battery = (int)(battery * 100);
    Serial.println("batteryCapacity: " + String(battery));

    return battery;
}

void DrawUI()
{
    // Draw the initial state of UI
    UI.draw_all(UPDATE_MODE_GC16);
}

void BuildStatusBar()
{
    // Draw Date left top
    // rtc_date_t date;
    // M5.RTC.getDate(&date);
    // Serial.printf("Datum: %02d.%02d.%02d", date.day, date.mon, date.year);

    // char buffer[50];
    // snprintf(buffer,50 ,"%02d.%02d.%02d", date.day, date.mon, date.year);

    // Draw temp top left
    int ret = M5.SHT30.UpdateData();
    Serial.printf("\nInit Temp: %d\n", ret);
    float temp = M5.SHT30.GetTemperature();
    Serial.printf("Temp: %f °C\n", temp);

    char buffer[50];
    snprintf(buffer, 50, "%2.1f°C", temp);
    UI.create_label(30, 10, 25, 15, 256, GetTempString());

    // Draw Battery top right
    int battery = GetBattery();

    UI.create_label(500, 10, 25, 15, 256, (String)battery + "%");

    // Draw current Mode
    // ToDo: Center Text
    UI.create_label(SCREENWIDTH / 2, 10, 25, 15, 256, drawModeEnnumToString(drawMode));

    // Draw horizontal Line
    UI.create_Hline(0, 35, 540, 15, 1);
}

void BuildBackgroundGrid()
{
    UI.create_Vline(ERRORLINEX, 0, 960, 15, 1);
    UI.create_Vline(ERRORLINEX * 2, 0, 960, 15, 1);

    for (size_t i = 0; i <= 4; i++)
    {
        UI.create_Hline(0, STATUSBARHEIGHT + (189 * i + 1), SCREENWIDTH, 15, 1);
    }
}

float getTemp()
{
    Serial.print("GetTemp...");
    int ret = M5.SHT30.UpdateData();
    float temp = M5.SHT30.GetTemperature();
    temp = ((int) (temp * 10.0) / 10.0);
    // Store the temp to the Preferences
    preferences.putFloat("temp", temp);
    Serial.println("done");
    return temp;
}

String GetTempString()
{

    float temp = getTemp();
    Serial.printf("Temp: %f °C\n", temp);

    char buffer[5];
    snprintf(buffer, 50, "%2.1f°C", temp);
    return buffer;
}

float GetRelHumidity()
{
    Serial.print("GetRelHumidity...");
    int ret = M5.SHT30.UpdateData();
    float hum = M5.SHT30.GetRelHumidity();
    hum = ((int) (hum * 10.0) / 10.0);
    // Store the humidity to the Preferences
    preferences.putFloat("hum", hum);
    Serial.println("done");
    return hum;
}

String GetRelHumidityString()
{
    char buffer[5];
    float hum = GetRelHumidity();
    snprintf(buffer, 50, "%2.1f%%", hum);
    return buffer;
}

void BuildScreenSaverUI()
{
    UI.clear();
    drawMode = kScreenSaver;
    canvas.setTextDatum(TL_DATUM);

    BuildStatusBar();

    UI.create_label(ERRORLINEX + 10, SCREENHEIGHT * 0.25 - 40, 80, 15, 256, GetTempString());
    UI.create_label(ERRORLINEX + 10, SCREENHEIGHT * 0.75 - 40, 80, 15, 256, GetRelHumidityString());
}

void BuildHomeUI()
{
    // define ui design
    // top left
    UI.create_button(50, 60, 190, 160, ImageResource_top_button_190x160, ImageResource_top_button_black_190x160);
    UI.create_object(50, 220, 190, 60, ImageResource_middle_button_190x60);
    UI.create_button(50, 280, 190, 160, ImageResource_bottom_button_190x160, ImageResource_bottom_button_black_190x160);

    // top right
    UI.create_button(300, 60, 190, 160, ImageResource_top_button_190x160, ImageResource_top_button_black_190x160);
    UI.create_object(300, 220, 190, 60, ImageResource_middle_button_190x60);
    UI.create_button(300, 280, 190, 160, ImageResource_bottom_button_190x160, ImageResource_bottom_button_black_190x160);

    // bottom left
    UI.create_button(50, 60 + 460, 190, 160, ImageResource_top_button_190x160, ImageResource_top_button_black_190x160);
    UI.create_object(50, 220 + 460, 190, 60, ImageResource_middle_button_190x60);
    UI.create_button(50, 280 + 460, 190, 160, ImageResource_bottom_button_190x160, ImageResource_bottom_button_black_190x160);

    // bottom right
    UI.create_button(300, 60 + 460, 190, 160, ImageResource_top_button_190x160, ImageResource_top_button_black_190x160);
    UI.create_object(300, 220 + 460, 190, 60, ImageResource_middle_button_190x60);
    UI.create_button(300, 280 + 460, 190, 160, ImageResource_bottom_button_190x160, ImageResource_bottom_button_black_190x160);

    // label (x, y, font_size, color, cache_size, title);
    UI.create_label(145, 242, 22, 15, 256, "Kaffeemaschine");
    UI.create_label(395, 242, 22, 15, 256, "Anlage");
    UI.create_label(145, 702, 22, 15, 256, "Licht Wohnzimmer");
    UI.create_label(395, 702, 22, 15, 256, "Gute Nacht");

    // Initialize after "create_xxxx"
    UI.initialize();

    // Draw the initial state of UI
    UI.draw_all(UPDATE_MODE_GC16);
}

void buildVacuumUI()
{
    // top left

    UI.create_object((540 - 490) / 2, 50, 490, 490, ImageResource_vacuum_490x490);

    DrawUI();
}

// ToDo Draw wackeup reason
esp_sleep_wakeup_cause_t print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        UI.create_label(240, 580, 22, 15, 256, "Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        UI.create_label(240, 580, 22, 15, 256, "Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        UI.create_label(240, 580, 22, 15, 256, "Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        UI.create_label(240, 580, 22, 15, 256, "Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        UI.create_label(240, 580, 22, 15, 256, "Wakeup caused by ULP program");
        break;
    default:
        UI.create_label(240, 580, 22, 15, 256, "Wakeup was not caused by deep sleep");
        break;
    }
    return wakeup_reason;
}
void setup()
{
    //  Check power on reason before calling M5.begin()
    //  which calls RTC.begin() which clears the timer flag.
    Wire.begin(21, 22);
    uint8_t data = M5.RTC.readReg(0x01);
    bool timerWakeUp = (data & 0b00000100) == 0b00000100;
    Serial.begin(115200);

    preferences.begin("my-app", false);
    unsigned int counter = preferences.getUInt("counter", 0);
    // Increase counter by 1
    counter++;
    // Print the counter to Serial Monitor
    Serial.printf("Current counter value: %u\n", counter);

    //ToDo push battery to mqtt on every 10s startup

    // Store the counter to the Preferences
    Serial.println("Store the counter to the Preferences");
    preferences.putUInt("counter", counter);

    Serial.println("Get Temp and Hum from Preferences");
    float nvsTemp = preferences.getFloat("temp", -1);
    float nvsHum = preferences.getFloat("hum", -1);

    bool needUpdate = nvsTemp == getTemp() && nvsHum == GetRelHumidity();

    Serial.println("Temp: " + GetTempString());
    Serial.println("Hum: " + GetRelHumidityString());
    
    // if timer wakeup and prev. temp is eq - shutdown
    if (timerWakeUp && !needUpdate)
    {
        Serial.println("timer wakeup and prev. temp is eq - shutdown");
        //shutdown(5 * 60, false);
    }

    M5.begin(true, false, true, true, true);       
        
    // Initialize after "create_xxxx"
    UI.initialize();

    if (GetBattery() < 5)
    {
        UI.create_label(SCREENWIDTH / 2, SCREENHEIGHT / 2, 22, 15, 256, "Akku leer");
        DrawUI();
        shutdown(0);
    }

    // Check timer flag
    if (timerWakeUp)
    {
        Serial.println("Power on by: RTC timer");
        BuildScreenSaverUI();
        DrawUI();
        shutdown(5 * 60, true);
    }

    startMilis = millis();

    BuildStatusBar();
    BuildHomeUI();
    DrawUI();

    // Try to connect wifi.
    if (connect_wifi(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED)
    {
        shutdown(900, true);
    }

    client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
    client.setCallback(callback);

    // delay(1000);
    // Serial.printf("clear ui and draw vacuum");
    // UI.clear();
    // drawVacuum();
}

void loop()
{
    mqttConnect();

    if (!client.connected())
    {
        shutdown(600);
    }

    // Return 0 or more if the button is pressed, -1 otherwise.
    int id = UI.check_executable_button_id();
    if (id >= 0)
    {
        // reset sleepTimer
        startMilis = millis();

        char buffer[50];
        snprintf(buffer, 50, "Touchbutton=%d", id);
        Serial.printf("execute button (id = %d)\n", id);
        // When the process is finished, set the button to the default color.
        client.publish(MQTT_BASETOPIC, buffer);
        UI.release_button(id, UPDATE_MODE_DU);
    }

    if (M5.BtnL.wasPressed())
    {
        Serial.printf("Btn L Pressed");
    }
    if (M5.BtnP.wasPressed())
        Serial.printf("Btn P Pressed");
    if (M5.BtnR.wasPressed())
        Serial.printf("Btn R Pressed");

    if (M5.BtnP.pressedFor(2000))
    {
        Serial.printf("Btn P was pressed for 2 sec, Shutdown");
        BuildScreenSaverUI();
        shutdown(900, true);
    }

    M5.update();

    client.loop();

    if ((millis() - startMilis) > SLEEPTIMER * 1000)
    {
        Serial.println("millis: " + String(millis()));
        Serial.println("startMilis:" + String(startMilis));
        BuildScreenSaverUI();
        DrawUI();
        shutdown(900, true);
    }
}