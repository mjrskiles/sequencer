#include <Arduino.h>
#include "midi_reader.h"

const char *file_path = "zelda-overworld.mid";

const int chipSelect = BUILTIN_SDCARD;

File glob_file;

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
//    MidiFile mf = MidiFile(file_path);
    HeaderChunk header = HeaderChunk();
    header.gobble(glob_file);
    header.processHeader();
}

void loop() {
// write your code here
}