//
// Created by mjrsk on 10/9/2021.
//

#ifndef SEQUENCER_MIDI_STREAM_PARSER_H
#define SEQUENCER_MIDI_STREAM_PARSER_H

#define MIDI_MSG_MAX_DATA_LENGTH 256

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
    MidiParser(ByteStream stream, ByteStream *trackStreamBuffer, uint16_t size, MidiEvents *lastEventsBuffer);

    void init();
    bool isAvailable() const;
    void printHeaderInfo();
    uint16_t getNumTracks() const;
    bool trackHasNextEvent(uint16_t trackNum);
    TrackEvent readEventAndPrint(uint16_t trackNum);

    static void prepTrack(ByteStream &trackStream, ByteStream &workerStream);
    static uint32_t readVariableLengthQuantity(ByteStream &stream);
    static uint32_t readInt32(ByteStream &stream);
    static void advanceBy(uint32_t numBytes, ByteStream &stream);

protected:
    static TrackEvent readEvent(ByteStream &stream, MidiEvents &lastEventBuffer);

    static TrackEvent readEventAndPrint(ByteStream &stream, MidiEvents &lastEventBuffer);
    ByteStream *trackStreams;
    void readHeader();
    ByteStream _startOfStream;
    ByteStream _workerStream;
    uint16_t _numAvailableTrackStreams;
    bool _available = false;
    uint16_t _format = 0;
    uint16_t _numTracks = 0;
    uint16_t _divisionType = 0;
    MidiEvents *_lastEvent;
    //utility
    static bool compareArrays(uint8_t *a1, uint8_t *a2, int commonSize);
};

#endif //SEQUENCER_MIDI_STREAM_PARSER_H
