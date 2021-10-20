#include <Arduino.h>
#include "midi_stream.h"
#include "midi_stream_parser.h"
#include "midi_file_dumps.h"
#include <USB-MIDI.h>
#include <USBHost_t36.h>

const char *file_path = "midi-files/zelda-overworld.mid";
uint32_t sz = ZELDA_FILE_BYTES_SIZE;
MidiStream midiStream = MidiStream(zelda_theme, sz);
MidiParser midiParser = MidiParser(midiStream);
elapsedMillis ms;
bool flip = false;
const int channel = 1;
uint32_t trackLen = 0;

//MIDI comm
USBHost usbHost;
MIDIDevice teensyMidi(usbHost);
USBMIDI_CREATE_DEFAULT_INSTANCE();
void setup() {
    pinMode(40, INPUT_PULLUP);
    delay(3000);
    Serial.printf("Buffer size: %d\n", midiStream.getSize());
    usbMIDI.begin();
    usbHost.begin();
    if (midiParser.isAvailable()) {
        Serial.println("Midi header read. Parser available");
        midiParser.advanceBy(62);
//        uint32_t chunk1Len = midiParser.readInt32();
//        Serial.printf("Track chunk 1 Length: %d\n", chunk1Len);
//        midiParser.advanceBy(chunk1Len);
//        midiParser.advanceBy(4);
//        trackLen = midiParser.readInt32();
        Serial.printf("Track 1 status: %x\n");
    }

}

elapsedMicros eventTimer;
uint32_t timerTarget;
bool waiting = false;
TrackEvent nextEvent;

void loop() {
    if (!waiting && midiStream.hasNext()) {
        nextEvent = midiParser.readEvent();
        timerTarget = nextEvent.deltaT * 1000; //delta T is in microseconds
        eventTimer = 0;
        waiting = true;


        teensyMidi.sendNoteOn(40, 127, 1);
    }

    if (waiting && eventTimer >= timerTarget) {
        if (nextEvent.event.status == NOTE_ON || nextEvent.event.status == NOTE_OFF)
        Serial.println("--Event--");
        Serial.printf("Status %x | data 1 %x | data 2 %x | chan %d\n", nextEvent.event.status, nextEvent.event.data[0],
                      nextEvent.event.data[1], nextEvent.event.channel);
        waiting = false;

        teensyMidi.sendNoteOff(40, 127, 1);
    }
}