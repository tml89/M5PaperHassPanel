# M5Paper Home-Assistant Panel

# Features

* WIP
 
# Requirement

* M5Paper (Web site: https://m5stack-store.myshopify.com/collections/m5-core/products/m5paper-esp32-development-kit-960x540-4-7-eink-display-235-ppi)
* PlatformIO
 
# Installation

Change your Wifi SSID and password(`src/wifi_info.h`).

```cpp
#ifndef _WIFI_INFO_H_
#define _WIFI_INFO_H_

const char *ssid = "ssid";
const char *password = "password";

#endif
```


 
# Note
 
If you want to change the image of the button, use the following.  
https://github.com/m5stack/M5EPD/tree/main/tools/image2gray

If you want to change the font, use the following.  
https://github.com/m5stack/M5EPD/tree/main/tools/ttf2bin
 
