#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <MIDI.h>
#include "stream_buffers.h"
#include "midi_clock.h"

uint32_t sz = ZELDA_FILE_BYTES_SIZE;
uint32_t marioSize = MARIO_CASTLE_BYTES_SIZE;
ByteStream midiStream = ByteStream(mario_castle_theme, marioSize);
MidiParser midiParser = MidiParser(midiStream, track_streams, 16, last_event);
elapsedMicros pgmTime;
MidiClock midiClock = MidiClock(track_tickers);
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
    for (uint16_t i = 0; i < midiParser.getNumTracks(); i++) {
        waiting_track_events[i] = midiParser.readEventAndPrint(i);
        midiClock.resetTrackTicker(i);
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
    for (uint16_t i = 0; i < midiParser.getNumTracks(); i++) {
        if (midiParser.trackHasNextEvent(i) && waiting_track_events[i].deltaT <= midiClock.getTicksSinceLastEvent(i)) {
            waiting_track_events[i] = midiParser.readEventAndPrint(i);
            if (waiting_track_events[i].event.status == NOTE_ON) {
                Serial.println("Send note On");
                uint8_t vel = waiting_track_events[i].event.data[1];
                vel = vel > 127 ? 127 : vel;
                Serial.printf("vvv Note velocity %x\n", vel);
                MIDI.sendNoteOn(waiting_track_events[i].event.data[0], vel, 1);
            }
            else if (waiting_track_events[i].event.status == NOTE_OFF) {
                MIDI.sendNoteOff(waiting_track_events[i].event.data[0], 0, 1);
            }
            midiClock.resetTrackTicker(i);
        }
    }
}