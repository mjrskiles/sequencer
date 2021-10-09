#include <Arduino.h>
#include "midi_reader.h"

const char *file_path = "zelda-overworld.mid";

const int chipSelect = BUILTIN_SDCARD;

File glob_file;
uint8_t buf[] = {0,0,0,0};

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

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect.
    }
    Serial.print("Initializing SD card...");

    if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

// write your initialization code here
    Serial.println("Midi Sequencer");
    delay(2000);
    glob_file = SD.open(file_path, FILE_WRITE);
    Serial.printf("Opened file at %s\n", glob_file.name());
    size_t sizeRead = glob_file.read(buf, 4);
    printBuffer(buf, 4);
}

void loop() {
// write your code here
}