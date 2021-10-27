#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <MIDI.h>

uint32_t sz = ZELDA_FILE_BYTES_SIZE;
uint32_t marioSize = MARIO_CASTLE_BYTES_SIZE;
MidiStream midiStream = MidiStream(mario_castle_theme, marioSize);
MidiParser midiParser = MidiParser(midiStream);
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
    if (midiParser.isAvailable()) {
        midiParser.printHeaderInfo();
    }
    while(1) {
        if (digitalRead(14) == LOW) {
            break;
        }
    }
    midiParser.readTrackStart();
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
        delayMicroseconds(nextEvent.deltaT * 7800);
        if (nextEvent.event.status == NOTE_ON) {
            if (nextEvent.event.data[1] > 0) {

                Serial.println("Send note On");
                uint8_t vel = nextEvent.event.data[1];
                vel = vel > 127 ? 127 : vel;
                Serial.printf("vvv Note velocity %x\n", vel);
                MIDI.sendNoteOn(nextEvent.event.data[0], 65, 1);
            } else {
                Serial.println("Send note Off");
                MIDI.sendNoteOff(nextEvent.event.data[0], 0, 1);
            }
        } else if (nextEvent.event.status == NOTE_OFF) {
            Serial.println("Send note Off");
            MIDI.sendNoteOff(nextEvent.event.data[0], 0, 1);
        }
    }
}