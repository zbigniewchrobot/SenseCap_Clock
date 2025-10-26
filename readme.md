# ⏰ SenseCap Clock 

An Arduino-based digital clock running on the **SenseCAP Indicator** by Seeed Studio, powered by ESP32.  
It displays the current time using a 480x480 RGB display with time synchronized via NTP.

---

## Features

- Supports 4" 480x480 RGB display (ST7701)
- Automatic Wi-Fi connection
- Time synchronization with NTP server (`pool.ntp.org`)
- Analog clock with hour, minute, and second hands
- PWM backlight control on pin 45
- Custom thick line drawing for clock hands
- Uses Arduino libraries: `Arduino_GFX_Library`, `WiFi`, `time.h`

---

## Requirements

### Hardware

- SenseCAP Indicator ESP32 device with  4" RGB display (ST7701 controller) https://wiki.seeedstudio.com/Sensor/SenseCAP/SenseCAP_Indicator/Get_started_with_SenseCAP_Indicator/ 

- Arduiono details https://wiki.seeedstudio.com/SenseCAP_Indicator_ESP32_Arduino/

- How to start esp32 with arduino https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html 
    
- Wi-Fi network access

### Software

- Arduino IDE (PlatformIO didnt tested yet)  
- Libraries:  
  - `Arduino_GFX_Library`  
  - `PCA95x5`  
  - `WiFi`  
  - `time.h`  
- Additional source files:  
  - `Indicator_Extender.h` and `Indicator_Extender.cpp`  
  - `Indicator_SWSPI.h` and `Indicator_SWSPI.cpp`
  - `lv_conf.h.readymade` which must be copied to the arduino library folder
  - `localdata.h`  - MUST be created by you with WiFi SSID and password!!!     
- Board to be used in Arduino `ESP32S3 Dev Module`
---


## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/zbigniewchrobot/SenseCap_Clock.git
   cd SenseCap_Clock
2. Create localdata.h file with below content
   ```C
      #define WIFI_SSID "your_ssid"
      #define WIFI_PASSWORD "your_password"
3. Copy file lv_conf.h.readymade as lv_conf.h to Arduino library folder. See also comments in lv_conf.h.readymade file
4. Compile (Verify) and Upload to the device


## 👤 Autor

**Zbigniew Chrobot**  
📧 zbigniewchrobot@o2.pl  
🔗 [github.com/zbigniewchrobot](https://github.com/zbigniewchrobot)

