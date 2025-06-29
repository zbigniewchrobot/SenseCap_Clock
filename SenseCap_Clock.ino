/*
 * Arduino program to show simple clock on SenseCap Indicator by seed studio 
 * https://wiki.seeedstudio.com/Sensor/SenseCAP/SenseCAP_Indicator/Get_started_with_SenseCAP_Indicator/
 *  * 
 * After restart hardware, especially display is initialized and then Indicator is connected to WiFi and reads time from NTP server
 *
 * Thanks to github user u4mzu4 for the SWSPI config file that support the Sensecap indicator
 * Thanks to LongDirtyAnimAlf for helping update the Arduino library for the SenseCAP indicator, including touchscreen support
 * Description of Arduion for SenseCap Indicator https://wiki.seeedstudio.com/SenseCAP_Indicator_ESP32_Arduino/
 * Thanks LongDirtyAnimAlf, Hendra and u4mzu4's efforts and your work will be exhibited.
*/

#include <Arduino_GFX_Library.h>
#include <PCA95x5.h>
#include <WiFi.h>
#include <time.h>

#include "Indicator_Extender.h"
#include "Indicator_SWSPI.h"
#include "localdata.h"  // create you own localdata.h file with

#define HOR_RES 480
#define VER_RES 480
#define CENTER_X 240
#define CENTER_Y 240
#define RADIUS_H 150  // 72
#define RADIUS_M 200  // 96
#define RADIUS_S 220  // 110

//#define GFX_BL DF_GFX_BL  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define GFX_BL 45
// PWM
#define PWM_PIN GFX_BL
#define PWM_FREQ_HZ 1000    // 500 Hz PWM
#define PWM_RESOLUTION 100  // 0–100% duty
// backlight and colors
#define BACKLIGHT 2
// below colors in rgb565 format
#define COLOR_W 63496  // 45061
#define COLOR_S 27469  // 21130


/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX* gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

#define GFX_DEV_DEVICE ESP32_S3_RGB


void drawThickLine(int x0, int y0, int x1, int y1, uint16_t color, uint8_t thickness);
void drawClock(int h, int m, int s);

// option 1:
// Uncomment for 4" rect display

Arduino_DataBus *bus = new Indicator_SWSPI(
  GFX_NOT_DEFINED /* DC */, EXPANDER_IO_LCD_CS /* CS */,
  SPI_SCLK /* SCK */, SPI_MOSI /* MOSI */, GFX_NOT_DEFINED /* MISO */);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  18 /* DE */, 17 /* VSYNC */, 16 /* HSYNC */, 21 /* PCLK */,
  4 /* R0 */, 3 /* R1 */, 2 /* R2 */, 1 /* R3 */, 0 /* R4 */,
  10 /* G0 */, 9 /* G1 */, 8 /* G2 */, 7 /* G3 */, 6 /* G4 */, 5 /* G5 */,
  15 /* B0 */, 14 /* B1 */, 13 /* B2 */, 12 /* B3 */, 11 /* B4 */,
  1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
  1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  HOR_RES /* width */, VER_RES /* height */, rgbpanel, 2 /* rotation */, false /* auto_flush */,
  bus, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));


#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/
// time
struct tm timeinfo;
int hour = 12;
int minute = 30;
int second = 250;

// PWM
volatile int dutyCycle = BACKLIGHT;  // backlight 0...100 (0-switch off)
int counter = 0;
esp_timer_handle_t pwmTimer;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
// volatile bool pinState = false;

//  przerwanie dla PWM
void IRAM_ATTR onTimerCallback(void* arg) {
  if (counter < dutyCycle) {
    digitalWrite(PWM_PIN, HIGH);
  } else {
    digitalWrite(PWM_PIN, LOW);
  }

  counter += 1;
  if (counter >= PWM_RESOLUTION) counter = 0;
}

void setup(void) {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  while(!Serial);
  Serial.println("Zegar by ZCh Zybi");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  extender_init();

  // Init Display

  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }

  gfx->fillScreen(BLACK);

#ifdef GFX_BL

  // version with backlight ON/OFF
  //pinMode(GFX_BL, OUTPUT);
  //digitalWrite(GFX_BL, HIGH);

  // version with backlight PWM
  //
  pinMode(PWM_PIN, OUTPUT);
  //digitalWrite(PWM_PIN, HIGH);

  const esp_timer_create_args_t timer_args = {
    .callback = &onTimerCallback,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "pwmTimer"
  };
  esp_timer_create(&timer_args, &pwmTimer);
  esp_timer_start_periodic(pwmTimer, 1000000 / (PWM_FREQ_HZ * PWM_RESOLUTION));  // µs

#endif


  delay(5000);  // 5 seconds

  // USTAW CZAS
  /*
  struct tm czas;
  czas.tm_year = 2025 - 1900;  // Rok - 1900 (opcjonalnie)
  czas.tm_mon  = 5 - 1;        // Miesiąc 0–11 (opcjonalnie)
  czas.tm_mday = 19;           // Dzień (opcjonalnie)

  czas.tm_hour = 13;
  czas.tm_min  = 10;
  czas.tm_sec  = 0;

  time_t t = mktime(&czas);  // Konwertuj na czas POSIX
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, nullptr);  // Ustaw czas systemowy
  */

  gfx->setCursor(0, 20);
  gfx->setTextColor(RED, BLACK);
  gfx->setTextSize(2);

  // set up time by  WiFi NTP
  Serial.println("Szukam WiFi...");
  gfx->print("Szukam WiFi... \n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // replace with your own data
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Nadal szukam WiFi...");
    gfx->print("Nadal szukam WiFi...\n");
    gfx->flush();
    delay(200);
  }
  Serial.println("Połaczono WiFi...");
  gfx->print("Połączono WiFi...\n");

  // NTP
  Serial.println("Łącze NTP ...");
  gfx->print("Łączę NTP...\n");
  configTime(3600, 3600, "pool.ntp.org");  // GMT +1, DST +1 (Polska)
  delay(200);
  gfx->fillScreen(BLACK);
  gfx->flush();
}

// loop
void loop() {

  // update every second
  static uint32_t last_update = 0;
  if (millis() - last_update > 1000) {
    last_update = millis();
    while (!getLocalTime(&timeinfo))
      ;

    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    second = timeinfo.tm_sec;

    // update esp32 rtc via NTP once per hour
    if ((minute == 0) && (second == 0)) {
      configTime(3600, 3600, "pool.ntp.org");  // GMT +1, DST +1 (Polska)
    }

    // update display
    drawClock(hour, minute, second);
    gfx->flush();
  }
}

//
void drawThickLine(int x0, int y0, int x1, int y1, uint16_t color, uint8_t thickness) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float len = sqrt(dx * dx + dy * dy);
  float offsetX = -dy * thickness / (2 * len);
  float offsetY = dx * thickness / (2 * len);

  int x0a = x0 + offsetX;
  int y0a = y0 + offsetY;
  int x0b = x0 - offsetX;
  int y0b = y0 - offsetY;
  int x1a = x1 + offsetX;
  int y1a = y1 + offsetY;
  int x1b = x1 - offsetX;
  int y1b = y1 - offsetY;

  gfx->fillTriangle(x0a, y0a, x1a, y1a, x1b, y1b, color);
  gfx->fillTriangle(x0a, y0a, x0b, y0b, x1b, y1b, color);
}

void drawClock(int h, int m, int s) {

  static int last_hx = 0, last_hy = 0;
  static int last_mx = 0, last_my = 0;
  static int last_sx = 0, last_sy = 0;

  // Wymaż stare wskazówki (kolor tła)
  //gfx->drawLine(CENTER_X, CENTER_Y, last_hx, last_hy, BLACK);
  drawThickLine(CENTER_X, CENTER_Y, last_hx, last_hy, BLACK, 12);
  //gfx->drawLine(CENTER_X, CENTER_Y, last_mx, last_my, BLACK);
  drawThickLine(CENTER_X, CENTER_Y, last_mx, last_my, BLACK, 8);
  //gfx->drawLine(CENTER_X, CENTER_Y, last_sx, last_sy, BLACK);
  drawThickLine(CENTER_X, CENTER_Y, last_sx, last_sy, BLACK, 1);

  // Obliczenia kątów w radianach
  float angle_s = s * 6 * DEG_TO_RAD;
  float angle_m = m * 6 * DEG_TO_RAD;
  float angle_h = (h % 12 + m / 60.0) * 30 * DEG_TO_RAD;

  // Współrzędne końcowe
  int sx = CENTER_X + RADIUS_S * sin(angle_s);
  int sy = CENTER_Y - RADIUS_S * cos(angle_s);

  int mx = CENTER_X + RADIUS_M * sin(angle_m);
  int my = CENTER_Y - RADIUS_M * cos(angle_m);

  int hx = CENTER_X + RADIUS_H * sin(angle_h);
  int hy = CENTER_Y - RADIUS_H * cos(angle_h);

  // Rysuj nowe wskazówki
  drawThickLine(CENTER_X, CENTER_Y, hx, hy, COLOR_W, 12);
  drawThickLine(CENTER_X, CENTER_Y, mx, my, COLOR_W, 8);
  drawThickLine(CENTER_X, CENTER_Y, sx, sy, COLOR_S, 1);

  // Zapamiętaj pozycje
  last_hx = hx;
  last_hy = hy;
  last_mx = mx;
  last_my = my;
  last_sx = sx;
  last_sy = sy;

  gfx->setCursor(10, 10);
  gfx->setTextColor(RED, BLACK);
  char timeString[9];  // HH:MM:SS + null terminator
  sprintf(timeString, "%02d:%02d:%02d", hour, minute, second);
  gfx->print(timeString);
}



//*
//gfx->setCursor(random(gfx->width()), random(gfx->height()));
//gfx->setTextColor(random(0xffff), random(0xffff));
//gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
//gfx->println("Sensecap Indicator");

//int x0 = random(480);
//int y0 = random(480);
//int r0 = random(100);
//int colorc = random(65535);
//gfx->fillCircle(x0, y0, r0, colorc);
//gfx->drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
//gfx->setCursor(10, 10);   gfx->setTextColor(RED); gfx->println("Sensecap Indicator");

/*
  // aktualizuj czas
  second++;
  if (second>=60) {
    second = 0;
    minute++;
    if (minute>=60) {
      minute=0;
      hour++;
      if (hour>=12) {
      hour=0;
      }
    }
  }
  */
