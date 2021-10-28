//
// Created by mjrsk on 10/9/2021.
//

#ifndef SEQUENCER_MIDI_STREAM_PARSER_H
#define SEQUENCER_MIDI_STREAM_PARSER_H

#define MIDI_MSG_MAX_DATA_LENGTH 16

#include <cstdint>
#include "midi_stream.h"
#include "utils.h"

// Testing
#include <Arduino.h>

enum MidiEvents : uint8_t {
    NOTE_ON = 0x90,
    NOTE_OFF = 0x80,
    AFTERTOUCH_PRESSURE = 0xa0,
    CONTROL_CHANGE = 0xb0,
    PROGRAM_CHANGE = 0xc0,
    CHANNEL_PRESSURE = 0xd0,
    PITCH_WHEEL_CHANGE = 0xe0,
    SYSEX_META = 0xff
};

enum SysexMetaEvents : uint8_t {
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

enum MidiChannel : uint8_t {
    CH_0, CH_1, CH_2, CH_3,
    CH_4, CH_5, CH_6, CH_7,
    CH_8, CH_9, CH_A, CH_B,
    CH_C, CH_D, CH_E, CH_F
};

struct MidiMessage {
    MidiEvents status;
    MidiChannel channel;
    bool isMeta = false;
    uint8_t metaWord = 0;
    uint8_t data[MIDI_MSG_MAX_DATA_LENGTH];
    uint32_t len = 0;
    void print();
};

struct TrackEvent {
    uint32_t deltaT;
    MidiMessage event;
    void print();
};

class MidiParser {
public:
    explicit MidiParser(MidiStream stream, MidiStream *trackStreamBuffer, uint16_t size);

    void init();
    bool isAvailable() const;
    void prepTrack(MidiStream &trackStream);
    void printHeaderInfo();
    TrackEvent readEvent();
    TrackEvent readEventAndPrint();
    uint32_t readVariableLengthQuantity();
    uint32_t readInt32();
    // Testing only
    void advanceBy(uint8_t numBytes);
    uint32_t runningNumBytesRead();
    void resetRunningNum();

    uint32_t getCurrentChunkLength() const;

protected:
    void readHeader();
    MidiStream _startOfStream;
    MidiStream _workerStream;
    MidiStream *_trackStreams;
    uint16_t _numAvailableTrackStreams;
    bool _available = false;
    uint16_t _format = 0;
    uint16_t _numTracks = 0;
    uint16_t _divisionType = 0;
    uint32_t _currentChunkLength;
    MidiEvents _lastEvent;
    //utility
    bool compareArrays(uint8_t *a1, uint8_t *a2, int commonSize);
};

#endif //SEQUENCER_MIDI_STREAM_PARSER_H
