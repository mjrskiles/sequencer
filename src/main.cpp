#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <MIDI.h>
#include "stream_buffers.h"

const char *file_path = "midi-files/zelda-overworld.mid";
uint32_t sz = ZELDA_FILE_BYTES_SIZE;
MidiStream midiStream = MidiStream(zelda_theme, sz);
MidiParser midiParser = MidiParser(midiStream, track_streams, 16);
bool flipper = true;
elapsedMillis flipperTimer;
const int channel = 1;

//MIDI comm
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(40, INPUT_PULLUP);
    pinMode(14, INPUT);
    MIDI.begin(1);
    delay(3000);
    Serial.printf("Buffer size: %d\n", midiStream.getSize());
    midiParser.init();
    while(1) {
        if (digitalRead(14) == LOW) {
            break;
        }
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
        Serial.println("> > > E V E N T < < <");
        Serial.printf("Running bytes: %d\n", midiParser.runningNumBytesRead());
        TrackEvent nextEvent = midiParser.readEventAndPrint();
        delayMicroseconds(nextEvent.deltaT * 600);
    if (nextEvent.event.status == NOTE_ON) {
        Serial.println("Send note On");
        uint8_t vel = nextEvent.event.data[1];
        vel = vel > 127 ? 127 : vel;
        Serial.printf("vvv Note velocity %x\n", vel);
        MIDI.sendNoteOn(nextEvent.event.data[0], 65, 1);
    } else if (nextEvent.event.status == NOTE_OFF) {
        Serial.println("Send note Off");
        MIDI.sendNoteOff(nextEvent.event.data[0], 0, 1);
    }
    } else {
        MIDI.sendStop();
    }
}