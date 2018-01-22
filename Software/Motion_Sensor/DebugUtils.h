#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#ifdef DEBUG
 #define DEBUG_PRINT(x)         Serial.print(x);
 #define DEBUG_PRINTF(x, ...)   Serial.printf(x,  __VA_ARGS__);
 #define SERIAL_BEGIN(x)        Serial.begin(x); delay(50);
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTF(x, ...)
 #define SERIAL_BEGIN(x)
#endif

#endif

