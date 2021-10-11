#include <Arduino.h>
#include "midi_stream.h"
#include "midi_file_dumps.h"

const char *file_path = "midi-files/zelda-overworld.mid";

uint32_t sz = ZELDA_FILE_BYTES_SIZE;
MidiStream midiStream = MidiStream(zelda_theme, sz);

void setup() {
    delay(1000);
    Serial.printf("Buffer size: %d\n", midiStream.getSize());

}

void loop() {
// write your code here
}