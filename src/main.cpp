#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <MIDI.h>
#include "stream_buffers.h"

uint32_t sz = ZELDA_FILE_BYTES_SIZE;
uint32_t marioSize = MARIO_CASTLE_BYTES_SIZE;
MidiStream midiStream = MidiStream(mario_castle_theme, marioSize);
MidiParser midiParser = MidiParser(midiStream, track_streams, 16);
elapsedMicros pgmTime;
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
        payloads[i].event = midiParser.readEventAndPrint(midiParser.trackStreams[i]);
        payloads[i].timeTarget = pgmTime + (payloads[i].event.deltaT);
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
    unsigned long currentTime = pgmTime;
    
}