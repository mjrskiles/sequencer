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

ByteStream track_streams[16] {
        ByteStream(),
        ByteStream(),
        ByteStream(),
        ByteStream(),

        ByteStream(),
        ByteStream(),
        ByteStream(),
        ByteStream(),

        ByteStream(),
        ByteStream(),
        ByteStream(),
        ByteStream(),

        ByteStream(),
        ByteStream(),
        ByteStream(),
        ByteStream(),
};

MidiEvents last_event[16] {
    NOTE_OFF, NOTE_OFF, NOTE_OFF, NOTE_OFF,
    NOTE_OFF, NOTE_OFF, NOTE_OFF, NOTE_OFF,
    NOTE_OFF, NOTE_OFF, NOTE_OFF, NOTE_OFF,
    NOTE_OFF, NOTE_OFF, NOTE_OFF, NOTE_OFF
};

TrackEvent waiting_track_events[16] {
    TrackEvent(), TrackEvent(), TrackEvent(), TrackEvent(),
    TrackEvent(), TrackEvent(), TrackEvent(), TrackEvent(),
    TrackEvent(), TrackEvent(), TrackEvent(), TrackEvent(),
    TrackEvent(), TrackEvent(), TrackEvent(), TrackEvent(),
};

EventPayload payloads[16];

#endif //SEQUENCER_STREAM_BUFFERS_H
