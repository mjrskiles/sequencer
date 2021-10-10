//
// Created by mjrsk on 10/9/2021.
//

#include "utils.h"
#include <Arduino.h>

void printBuffer(uint8_t *buf, int size) {
    Serial.printf("Buffer:\n");
    for (int i = 0; i < size; i++) {
        buf[i] > 15 ? Serial.printf("%x ", buf[i]) : Serial.printf("0%x ", buf[i]);
        if ((i+1) % 4 == 0) {
            Serial.printf("\n");
        }
    }
    Serial.printf("\n");
}