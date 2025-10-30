#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <Arduino.h>
#include "ota_update.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

// OTA Configuration
#define OTA_HOSTNAME "ESP32-Eyes-Skull"
#define OTA_PASSWORD "skull2024"  // Change this to your preferred password

// WiFi Configuration - Add your credentials to config.h or define here
#ifndef WIFI_SSID
#define WIFI_SSID "KumOnham"     // Replace with your WiFi SSID
#endif
#ifndef WIFI_PASSWORD  
#define WIFI_PASSWORD "LettuceInn"  // Replace with your WiFi password
#endif

// OTA Status indicators
extern bool ota_in_progress;
extern unsigned long ota_progress_millis;

// Function declarations
void setupOTA();
void handleOTA();

bool connectWiFi();
void displayOTAStatus(const char* message);
void onOTAStart();
void onOTAEnd(); 
void onOTAProgress(unsigned int progress, unsigned int total);
void onOTAError(ota_error_t error);

// Simple OTA update functionality

#endif // OTA_UPDATE_H