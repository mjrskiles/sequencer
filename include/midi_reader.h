//
// Created by mjrsk on 10/8/2021.
//

#ifndef SEQUENCER_MIDI_READER_H
#define SEQUENCER_MIDI_READER_H
/*
 * Based on the midi file spec found at https://midimusic.github.io/tech/midispec.html
 */

#include <SD.h>
#include "strings.h"
#include "cstdint"

#define CHUNK_TYPE_HEADER 0
#define CHUNK_TYPE_TRACK 1

#define STATUS_NOTE_OFF 0x80
#define STATUS_NOTE_ON 0x90
#define STATUS_CONTROL_CHANGE 0xb0
#define STATUS_PROGRAM_CHANGE 0xc0

using namespace std;

struct MidiMessage {
    const char *description = str_descNotSet;
    uint8_t status = 0;
    uint8_t *data = nullptr;
    uint32_t len = 0;
    uint8_t channel;
    void print();
};

struct TrackEvent {
    uint32_t deltaT;
    MidiMessage event;
    void print();
};

class MidiChunk {
public:
    /*
     * Read bytes and put them in the buffer
     * the first 8 bytes of a midi chunk are in the format
     * tttt llll, aka <chunk type> <length>
     */
    void gobble(File file);

    virtual ~MidiChunk();

protected:
    int _chunkType;

    // The buffer contains a chunk's data, that is, everything EXCEPT the chunk type and length. It should therefore
    // contain <length> bytes
    uint8_t *_buffer;
    uint32_t _chunkLength = 0;
};

/*
<Header Chunk> = <chunk type><length><format><ntrks><division>
 */
class HeaderChunk : public MidiChunk {
public:
    HeaderChunk() : MidiChunk() {
        _chunkType = CHUNK_TYPE_HEADER;
    }

    /*
     * Take the byte array and parse it into objects
     */
    void processHeader();
protected:
    uint16_t _format = 0;
    uint16_t _numTracks = 0;
public:
    uint16_t getFormat() const;

    uint16_t getNumTracks() const;

    uint16_t getDivision() const;

protected:
    uint16_t _division = 0;
};

/*
 *
<Track Chunk> = <chunk type><length><MTrk event>+
<MTrk event> = <delta-time><event>
<event> = <MIDI event> | <sysex event> | <meta-event>
 */
class TrackChunk : public MidiChunk {
public:
    TrackChunk() : MidiChunk(){
        _chunkType = CHUNK_TYPE_TRACK;
    }

    void processTrack();
    TrackEvent readTrackEvent();
    MidiMessage readMidiEvent(uint8_t command);
    MidiMessage readSysexEvent(uint8_t command);
    MidiMessage readMetaEvent(uint8_t command);
    uint8_t readByte();
    uint8_t* readBytes(uint32_t amount);
protected:
    TrackEvent *_trackEvents;
    uint32_t _numTrackEvents;
    uint32_t _bufferOffset = 0; // This needs to be manually incremented whenever a byte is read out of the buffer!
    bool _reachedEOT = false;
    uint8_t _runningStatus = 0;
};

class MidiFile {
public:
    MidiFile(File *file);

    static uint32_t parseVariableLengthQuantity(uint8_t *buffer, uint32_t& offset);

    const HeaderChunk &getHeader() const;

    void setHeader(const HeaderChunk &header);

    TrackChunk *getTracks() const;

    void setTracks(TrackChunk *tracks);

    int getNumTracks() const;

    void setNumTracks(int numTracks);

    File getFile() const;

protected:
    File *_file;
    HeaderChunk _header;
    TrackChunk *_tracks;
    int _numTracks;
};


#endif //SEQUENCER_MIDI_READER_H