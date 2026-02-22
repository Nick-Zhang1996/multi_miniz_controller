/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Multiprotocol is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Multiprotocol.  If not, see <http://www.gnu.org/licenses/>.
 */

//******************
// Version
//******************
#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

//** Debug messages **

inline void debug(const char* format, ...) {
    char buffer[128];
    
    // Initialize the variadic argument list
    va_list args;
    va_start(args, format);
    
    // vsnprintf safely limits the output to the size of our buffer,
    // guaranteeing we never overwrite adjacent memory.
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    // Clean up the argument list
    va_end(args);
    
    Serial.print(buffer);
}

inline void debugln(const char* format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Serial.print(buffer);
    Serial.println(); // Let Arduino's built-in library handle the \r\n cleanly
}