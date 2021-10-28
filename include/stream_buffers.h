//
// Created by mjrsk on 10/27/2021.
//

#ifndef SEQUENCER_STREAM_BUFFERS_H
#define SEQUENCER_STREAM_BUFFERS_H

#include "midi_stream.h"
#include "midi_stream_parser.h"

struct EventPayload {
    TrackEvent event;
    unsigned long timeTarget = 0;
};

MidiStream track_streams[16] {
    MidiStream(),
    MidiStream(),
    MidiStream(),
    MidiStream(),

    MidiStream(),
    MidiStream(),
    MidiStream(),
    MidiStream(),

    MidiStream(),
    MidiStream(),
    MidiStream(),
    MidiStream(),

    MidiStream(),
    MidiStream(),
    MidiStream(),
    MidiStream(),
};

EventPayload payloads[16];

#endif //SEQUENCER_STREAM_BUFFERS_H
