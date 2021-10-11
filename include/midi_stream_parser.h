//
// Created by mjrsk on 10/9/2021.
//

#ifndef SEQUENCER_MIDI_STREAM_PARSER_H
#define SEQUENCER_MIDI_STREAM_PARSER_H

enum MidiEvents : byte {
    NOTE_ON,
    NOTE_OFF,
    AFTERTOUCH_PRESSURE,
    CONTROL_CHANGE,
    PROGRAM_CHANGE,
    CHANNEL_PRESSURE,
    PITCH_WHEEL_CHANGE,
    SYSEX_META
};

enum SysexMetaEvents : byte {
    SEQUENCE_NUMBER,
    TEXT,
    COPYRIGHT_NOTICE,
    SEQUENCE_OR_TRACK_NAME,
    INSTRUMENT_NAME,
    LYRIC,
    MARKER,
    CUE_POINT,
    CHANNEL_PREFIX = 0x20,
    END_OF_TRACK = 0x2f,
    SET_TEMPO = 0x51,
    SMPTE_OFFSET = 0x54,
    TIME_SIGNATURE = 0x58,
    KEY_SIGNATURE = 0x59,
    SEQUENCER_SPECIFIC = 0x7f
};

class MidiParser {
public:
    MidiParser(MidiStream stream);
    MidiEvents readEvent();
    uint32_t readVariableLengthQuantity();

protected:
    MidiStream _midiStream;
};

#endif //SEQUENCER_MIDI_STREAM_PARSER_H
