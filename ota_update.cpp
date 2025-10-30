#include "ota_update.h"
#include "globals.h"

// Global variables
bool ota_in_progress = false;
unsigned long ota_progress_millis = 0;

/**
 * Connect to WiFi network
 * Returns true if connected successfully
 */
bool connectWiFi() {
  Serial.println("Connecting to WiFi...");
  displayOTAStatus("WiFi Connecting");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) { // 30 second timeout
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
    displayOTAStatus("WiFi Connected");
    return true;
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    displayOTAStatus("WiFi Failed");
    return false;
  }
}

/**
 * Setup OTA update functionality
 */
void setupOTA() {
  Serial.println("Setting up OTA updates...");
  
  // Try to connect to WiFi
  if (!connectWiFi()) {
    Serial.println("Skipping OTA setup - no WiFi connection");
    return;
  }
  
  // Configure ArduinoOTA
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  
  // Set OTA callbacks
  ArduinoOTA.onStart(onOTAStart);
  ArduinoOTA.onEnd(onOTAEnd);
  ArduinoOTA.onProgress(onOTAProgress);
  ArduinoOTA.onError(onOTAError);
  
  // Start ArduinoOTA service
  ArduinoOTA.begin();
  
  Serial.println("OTA updates ready!");
  Serial.printf("Hostname: %s\n", OTA_HOSTNAME);
  Serial.printf("Use Arduino IDE or platformio with: %s\n", WiFi.localIP().toString().c_str());
  displayOTAStatus("OTA Ready");
}



/**
 * Handle OTA updates (call this in main loop)
 */
void handleOTA() {
  // Only handle Arduino OTA if WiFi is connected and not in middle of eye rendering
  if (WiFi.status() == WL_CONNECTED && !ota_in_progress) {
    ArduinoOTA.handle();
  }
}

/**
 * Display OTA status on screens (optional visual feedback)
 */
void displayOTAStatus(const char* message) {
  Serial.printf("OTA Status: %s\n", message);
  // Optional: Could display status on eye screens if desired
  // This would require careful integration with the eye rendering system
}

/**
 * OTA Start callback
 */
void onOTAStart() {
  ota_in_progress = true;
  String type;
  if (ArduinoOTA.getCommand() == U_FLASH) {
    type = "sketch";
  } else { // U_SPIFFS
    type = "filesystem";
  }
  
  Serial.println("Start updating " + type);
  displayOTAStatus("OTA Starting");
  
  // Clear screens to black during update
  digitalWrite(eye[0].tft_cs, LOW);
  tft.fillScreen(TFT_BLACK);
  digitalWrite(eye[0].tft_cs, HIGH);
  
  if (NUM_EYES > 1) {
    digitalWrite(eye[1].tft_cs, LOW);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(eye[1].tft_cs, HIGH);
  }
}

/**
 * OTA End callback
 */
void onOTAEnd() {
  Serial.println("\nOTA Update complete!");
  displayOTAStatus("OTA Complete");
  ota_in_progress = false;
}

/**
 * OTA Progress callback
 */
void onOTAProgress(unsigned int progress, unsigned int total) {
  unsigned int percent = (progress / (total / 100));
  
  // Only update every 10% to avoid spam
  if (percent % 10 == 0 && millis() - ota_progress_millis > 1000) {
    Serial.printf("Progress: %u%% (%u/%u)\n", percent, progress, total);
    displayOTAStatus(("OTA: " + String(percent) + "%").c_str());
    ota_progress_millis = millis();
  }
}

/**
 * OTA Error callback
 */
void onOTAError(ota_error_t error) {
  ota_in_progress = false;
  
  Serial.printf("Error[%u]: ", error);
  String errorMsg = "OTA Error: ";
  
  if (error == OTA_AUTH_ERROR) {
    Serial.println("Auth Failed");
    errorMsg += "Auth Failed";
  } else if (error == OTA_BEGIN_ERROR) {
    Serial.println("Begin Failed");
    errorMsg += "Begin Failed";
  } else if (error == OTA_CONNECT_ERROR) {
    Serial.println("Connect Failed");
    errorMsg += "Connect Failed";
  } else if (error == OTA_RECEIVE_ERROR) {
    Serial.println("Receive Failed");
    errorMsg += "Receive Failed";
  } else if (error == OTA_END_ERROR) {
    Serial.println("End Failed");
    errorMsg += "End Failed";
  }
  
  displayOTAStatus(errorMsg.c_str());
}