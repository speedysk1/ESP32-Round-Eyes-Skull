// An adaption of the "UncannyEyes" sketch (see eye_functions tab)
// for the TFT_eSPI library but adapted for two 240x240 px displays.

// For wiring see the wiring folder.

// Configuration settings for the eye, eye style, display count,
// chip selects and x offsets can be defined in the sketch "config.h" tab.

// To increase performance DMA should stay enabled.
// Two 240x240 px displays run at 32 FPS.
#define USE_DMA

#include <Arduino.h>
#include <cstdint>

// Load TFT driver library
// Use the predefined user setup Setup46_GC9A01_ESP32 of the TFT_eSPI library.
// Either select via library/TFT_eSPI/User_Setup_Select.h or as you like.
// An update of the library will overwrite the file above.
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft;

#define BUFFER_SIZE 1024

#ifdef USE_DMA
  #define BUFFERS 2
#else
  #define BUFFERS 1
#endif

uint16_t pbuffer[BUFFERS][BUFFER_SIZE];
bool dmaBuf = 0;

#include "globals.h"    // Include shared types and declarations first
#include "config.h"     // ****** CONFIGURATION IS DONE IN HERE ******

#ifdef ENABLE_OTA
#include "ota_update.h" // OTA update functionality
#endif

 // functions in the user*.cpp files, please enable one
extern void user_setup(void);
extern void user_loop(void);

// Eye functions declaration
extern void initEyes(void);
extern void updateEye(void);

#define SCREEN_X_START 0
#define SCREEN_X_END   SCREEN_WIDTH   // Badly named, actually the "eye" width!
#define SCREEN_Y_START 0
#define SCREEN_Y_END   SCREEN_HEIGHT  // Actually "eye" height

eyeStruct eye[NUM_EYES]; // One-per-eye structure array

uint32_t startTime;  // For FPS indicator

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }  Serial.println("starting");

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
  // enable backlight pin, initially off
  Serial.println("backlight turned off");
  pinMode(DISPLAY_BACKLIGHT, OUTPUT);
  digitalWrite(DISPLAY_BACKLIGHT, LOW);
#endif

  user_setup();
  // IMPORTANT: Call initEyes() before tft.init() to set up CS pins and avoid display conflicts (see hardware integration notes)
  initEyes();

#ifdef ENABLE_OTA
  // Setup OTA updates after hardware initialization
  setupOTA();
#endif

  Serial.println("initialising displays");
  tft.init();

#ifdef USE_DMA
  tft.initDMA();
#endif

  // raise chip select(s) so that displays can be individually configured
  digitalWrite(eye[0].tft_cs, HIGH);
  if (NUM_EYES > 1) digitalWrite(eye[1].tft_cs, HIGH);

  for (uint8_t e = 0; e < NUM_EYES; e++) {
    digitalWrite(eye[e].tft_cs, LOW);
    tft.setRotation(eyeInfo[e].rotation);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(eye[e].tft_cs, HIGH);
  }

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
  Serial.println("backlight on");
  analogWrite(DISPLAY_BACKLIGHT, BACKLIGHT_MAX);
#endif

  startTime = millis();
}

void loop() {
  // Handle OTA updates (only when not in progress to avoid interfering with eye rendering)
#ifdef ENABLE_OTA
  if (!ota_in_progress) {
    handleOTA();
  }
#endif
  
  // Skip eye updates during OTA to prevent conflicts
#ifdef ENABLE_OTA
  if (!ota_in_progress) {
    updateEye();
  }
#else
  updateEye();
#endif
  // use user_loop, do not add things here
}
