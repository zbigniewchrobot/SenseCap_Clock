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
    
- Wi-Fi network access

### Software

- Arduino IDE (PlatformIO didnt tested yet)  
- Libraries:  
  - `Arduino_GFX_Library`  
  - `PCA95x5`  
  - `WiFi`  
  - `time.h`  
- Additional source files:  
  - `Indicator_Extender.h`  
  - `Indicator_SWSPI.h`
  - `localdata.h`  - MUST be created by you with WiFi SSID and password!!!      
- Board used in Arduino 'ESP32S3 Dev Module'
---


## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/zbigniewchrobot/SenseCap_Clock.git
   cd SenseCap_Clock
2. Create localdata.h fie with data like below
   ```C
      #define WIFI_SSID "your_ssid"
      #define WIFI_PASSWORD "your_password"
3. Compile (Verify) and Upload
