#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// Constants that need to be shared
#define BUFFER_SIZE 1024

// Blink state constants
#define NOBLINK 0
#define ENBLINK 1
#define DEBLINK 2

// Constants from config.h needed by eye_functions.cpp (avoid including full config.h)
#ifndef NUM_EYES
#define NUM_EYES 2
#endif

#ifndef BLINK_PIN
#define BLINK_PIN -1
#endif

#ifndef LIGHT_PIN
#define LIGHT_PIN -1
#endif

// Constants from data files (will be overridden by actual values in config.h/data files)
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 240
#endif
#ifndef SCREEN_HEIGHT  
#define SCREEN_HEIGHT 240
#endif

// Eye data constants (fallback values, will be overridden by actual data files)
#ifndef IRIS_MIN
#define IRIS_MIN 150
#endif
#ifndef IRIS_MAX
#define IRIS_MAX 250
#endif
#ifndef SCLERA_WIDTH
#define SCLERA_WIDTH 375
#endif
#ifndef SCLERA_HEIGHT
#define SCLERA_HEIGHT 375
#endif
#ifndef IRIS_WIDTH
#define IRIS_WIDTH 180
#endif
#ifndef IRIS_HEIGHT
#define IRIS_HEIGHT 180
#endif
#ifndef IRIS_MAP_WIDTH
#define IRIS_MAP_WIDTH 489
#endif
#ifndef IRIS_MAP_HEIGHT
#define IRIS_MAP_HEIGHT 122
#endif

// Eye data arrays (defined in data/*.h files)
extern const uint16_t sclera[];
extern const uint16_t iris[];
extern const uint16_t polar[];
extern const uint8_t upper[];
extern const uint8_t lower[];

// Type definitions (shared between files)
typedef struct {
  uint8_t state;
  uint32_t duration;
  uint32_t startTime;
} eyeBlink;

typedef struct {
  int8_t  select;
  int8_t  wink;
  uint8_t rotation;
  int16_t xposition;
} eyeInfo_t;

// Eye structure type 
typedef struct {
  int16_t tft_cs;
  eyeBlink blink;
  int16_t xposition;
} eyeStruct;

// Global variables from main file
extern eyeStruct eye[];

extern eyeInfo_t eyeInfo[];
extern TFT_eSPI tft;
extern uint16_t pbuffer[][BUFFER_SIZE];
extern bool dmaBuf;
extern uint32_t startTime;

// Function declarations
extern void user_setup(void);
extern void user_loop(void);
extern void split(int16_t startValue, int16_t endValue, uint32_t startTime, int32_t duration, int16_t range);

#endif // GLOBALS_H