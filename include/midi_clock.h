//
// Created by mjrsk on 10/28/2021.
//

#ifndef SEQUENCER_MIDI_CLOCK_H
#define SEQUENCER_MIDI_CLOCK_H

#define NUM_MIDI_CLOCK_CHANNELS 16

#include "Arduino.h"
#include "clock_buffers.h"

typedef unsigned long MidiTicks;

class MidiClock {
public:
    MidiClock(elapsedMicros *trackTickers) : trackTickers(trackTickers) {}

    MidiTicks getTicksSinceLastEvent(uint16_t channel) { return microsToTicks(trackTickers[channel]); }
    void resetTrackTicker(uint16_t channel) { trackTickers[channel] = 0; }
protected:
    MidiTicks microsToTicks(elapsedMicros us) {
        return us / (tempo/div);
    }
    elapsedMicros *trackTickers;
    uint32_t tempo = 50000; // microseconds per beat, 120bpm default
    uint32_t div = 0x78;
};

#endif //SEQUENCER_MIDI_CLOCK_H
