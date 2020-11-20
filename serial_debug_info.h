#ifndef SERIAL_DEBUG_INFO_H
#define SERIAL_DEBUG_INFO_H

#define NAMEOF(name) #name

#ifdef SERIAL_DEBUG_ENABLE
    #define SERIAL_BEGIN(speed) Serial.begin((speed))
    #define SERIAL_DEBUG(x) Serial.print(NAMEOF(x)); Serial.print(" = "); Serial.println(x)
    #define SERIAL_DEBUGSTR(x) Serial.println(x)
#else
    #define SERIAL_BEGIN(speed)
    #define SERIAL_DEBUG(x)
    #define SERIAL_DEBUGSTR(x)
#endif

#endif
