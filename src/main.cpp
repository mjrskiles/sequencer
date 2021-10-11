#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <MIDI.h>

const char *file_path = "midi-files/zelda-overworld.mid";
uint32_t sz = ZELDA_FILE_BYTES_SIZE;
MidiStream midiStream = MidiStream(zelda_theme, sz);
elapsedMillis ms;
bool flip = false;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,  MIDI);
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.printf("Buffer size: %d\n", midiStream.getSize());
    MIDI.begin();
}

void loop() {
// write your code
    if (ms > 1000) {
        ms = 0;
        if (flip) {
            MIDI.sendNoteOff(44, 127, 1);
        } else {
            MIDI.sendNoteOn(44, 127, 1);
        }

    }
}