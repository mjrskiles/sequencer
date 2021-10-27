//
// Created by mjrsk on 10/27/2021.
//

#ifndef SEQUENCER_STREAM_BUFFERS_H
#define SEQUENCER_STREAM_BUFFERS_H

#include "midi_stream.h"

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

#endif //SEQUENCER_STREAM_BUFFERS_H
