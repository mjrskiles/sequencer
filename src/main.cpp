#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <USB-MIDI.h>

const char *file_path = "midi-files/zelda-overworld.mid";
uint32_t sz = ZELDA_FILE_BYTES_SIZE;
MidiStream midiStream = MidiStream(zelda_theme, sz);
MidiParser midiParser = MidiParser(midiStream);
elapsedMillis ms;
bool flip = false;
const int channel = 1;

//MIDI comm
USBMIDI_CREATE_DEFAULT_INSTANCE();
void setup() {
    pinMode(40, INPUT_PULLUP);
    delay(1000);
    Serial.printf("Buffer size: %d\n", midiStream.getSize());
    usbMIDI.begin();
    if (midiParser.isAvailable()) {
        Serial.println("Midi header read. Parser available");
        midiParser.advanceBy(8);
    //    uint32_t chunk1Len = midiParser.readVariableLengthQuantity();
    //    Serial.printf("Track chunk 1 Length: %d\n", chunk1Len);
        TrackEvent trkE1 = midiParser.readEvent();
        Serial.printf("Track 1 status: %x\n");
    }

}

void loop() {
// write your code
if (ms > 500) {
        ms = 0;
        if (flip) {
            usbMIDI.sendNoteOff(44, 127, 1);
            flip = false;
        } else {
            usbMIDI.sendNoteOn(44, 127, 1);
            Serial.println("Sending note on");
            flip = true;
        }

    }
}