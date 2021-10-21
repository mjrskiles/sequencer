#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <MIDI.h>

const char *file_path = "midi-files/zelda-overworld.mid";
uint32_t sz = ZELDA_FILE_BYTES_SIZE;
MidiStream midiStream = MidiStream(zelda_theme, sz);
MidiParser midiParser = MidiParser(midiStream);
bool flipper = true;
elapsedMillis flipperTimer;
const int channel = 1;

//MIDI comm
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(40, INPUT_PULLUP);
    delay(3000);
    Serial.printf("Buffer size: %d\n", midiStream.getSize());
    if (midiParser.isAvailable()) {
        midiParser.printHeaderInfo();
        midiParser.readTrackStart();
        while (midiParser.runningNumBytesRead() < midiParser.getCurrentChunkLength()) {
            midiParser.readEventAndPrint();
            Serial.printf("Running bytes: %d\n", midiParser.runningNumBytesRead());
        }
        midiParser.readTrackStart();
    }

}

void loop() {
    // Basic indicator that the program is running
    if (flipper && flipperTimer > 1000) {
        digitalWrite(LED_BUILTIN, HIGH);
        flipperTimer = 0;
        flipper = false;
    }
    if (!flipper && flipperTimer > 1000) {
        digitalWrite(LED_BUILTIN, LOW);
        flipperTimer = 0;
        flipper = true;
    }
    if (midiParser.runningNumBytesRead() < midiParser.getCurrentChunkLength()) {
        midiParser.readEventAndPrint();
        Serial.printf("Running bytes: %d\n", midiParser.runningNumBytesRead());
    }
}