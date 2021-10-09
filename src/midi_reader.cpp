//
// Created by mjrsk on 10/8/2021.
//

#include "midi_reader.h"

uint8_t header_chunk_byte_pattern[] {0x4d, 0x54, 0x68, 0x64};
uint8_t track_chunk_byte_pattern[] {0x4d, 0x54, 0x72, 0x6b};

bool compareArrays(uint8_t *a1, uint8_t *a2, int commonSize) {
    for (int i = 0; i < commonSize; i++) {
        if (a1[i] != a2[i]) {
            return false;
        }
    }
    return true;
}

void printBuffer(uint8_t *buf, int size) {
    Serial.printf("Buffer:\n");
    for (int i = 0; i < size; i++) {
        buf[i] > 15 ? Serial.printf("%x ", buf[i]) : Serial.printf("0%x ", buf[i]);
        if ((i+1) % 4 == 0) {
            Serial.printf("\n");
        }
    }
    Serial.printf("\n");
}

void MidiMessage::print() {
    Serial.printf("%s %s\n", "Midi Event: ", description);
    Serial.printf("  Status byte: %x\n", status);
    Serial.printf("  Channel: %d\n", channel);
    Serial.printf("  # of data bytes: %d\n", len);
    Serial.printf("  Data: \n");
    printBuffer(data, len);
}

void TrackEvent::print() {
    Serial.printf("Delta Time: %d\n", deltaT);
    event.print();
}

void MidiChunk::gobble(File file) {
    uint8_t chunk_type_bytes[4];
    uint8_t *bytes_ptr = chunk_type_bytes;
    uint8_t *hpattern_ptr = header_chunk_byte_pattern;
    uint8_t *tpattern_ptr = track_chunk_byte_pattern;
    // buffer, byte size, num bytes, file pointer
    //    fread(chunk_type_bytes, sizeof(uint8_t), 4, file);
    file.read(chunk_type_bytes, 4);
    if (compareArrays(bytes_ptr, hpattern_ptr, 4)) {
        _chunkType = CHUNK_TYPE_HEADER;
    } else if (compareArrays(bytes_ptr, tpattern_ptr, 4)) {
        _chunkType = CHUNK_TYPE_TRACK;
    }

    uint8_t chunk_length_bytes[4];
    file.read(chunk_length_bytes, 4);
    _chunkLength |= chunk_length_bytes[0] << 24;
    _chunkLength |= chunk_length_bytes[1] << 16;
    _chunkLength |= chunk_length_bytes[2] << 8;
    _chunkLength |= chunk_length_bytes[3] << 0;

    Serial.printf("Read %d as chunk length\n", _chunkLength);

    uint8_t *buffer = new uint8_t[_chunkLength];
    file.read(buffer, _chunkLength);
    _buffer = buffer;
}

MidiChunk::~MidiChunk() {
    delete _buffer;
}

const HeaderChunk &MidiFile::getHeader() const {
    return _header;
}

void HeaderChunk::processHeader() {
    if (_chunkLength != 6) {
        Serial.printf("Header chunk was not 6 as expected.");
    } else {
        _format |= _buffer[0] << 8;
        _format |= _buffer[1];
        _numTracks |= _buffer[2] << 8;
        _numTracks |= _buffer[3];
        _division |= _buffer[4] << 8;
        _division |= _buffer[5];
    }
    Serial.printf("Header:\n");
    Serial.printf("  Format: %x\n", _format);
    Serial.printf("  Number of tracks: %d\n", _numTracks);
    Serial.printf("  Division Type: %x\n", _division);
}

uint16_t HeaderChunk::getFormat() const {
    return _format;
}

uint16_t HeaderChunk::getNumTracks() const {
    return _numTracks;
}

uint16_t HeaderChunk::getDivision() const {
    return _division;
}

TrackChunk *MidiFile::getTracks() const {
    return _tracks;
}

void TrackChunk::processTrack() {
    Serial.printf(">>>> Track <<<<\n");
    while (_bufferOffset < _chunkLength && !_reachedEOT) {
        TrackEvent event = readTrackEvent();
        event.print();
    }
}

TrackEvent TrackChunk::readTrackEvent() {
    uint32_t time = MidiFile::parseVariableLengthQuantity(_buffer, _bufferOffset);
    uint8_t nextByte = readByte();
    MidiMessage message = MidiMessage();
    // status words have the leftmost bit set
    if (nextByte & 128) {
        uint8_t status = nextByte;
        _runningStatus = status;
        if (status == 0xff) {
            message = readMetaEvent(status);
        }
        else if (status == 0xf0) {
            message = readSysexEvent(status);
        } else {
            message = readMidiEvent(status);
        }
    } else {
        // It's a data word using running status!
        message = readMidiEvent(_runningStatus);
    }

    TrackEvent trackEvent = TrackEvent();
    trackEvent.deltaT = time;
    trackEvent.event = message;
    return trackEvent;
}

/*
msm_note_off = 0x80
msm_note_on  = 0x90
msm_control_change = 0xb0
msm_program_change = 0xc0
msm_sysex = 0xf0

# sysex messages
sysex_meta_event = 0xff
meta_track_name = 0xff_03 len text
meta_end_of_track = 0xff_02_00
meta_set_tempo = 0xff_51_03 # tt_tt_tt time in microseconds per quarter note
meta_time_sig = 0xff_58_04 # nn dd cc bb : numerator (dec), denom (dd^-2), midi clocks/metronome tick, 32nd notes per quarter
meta_key_sig = 0xff_59_02 # sf mi : sharp/flats major/minor -7 = 7 flats, 0 = C, 7 = 7 sharps; 0 = major 1 minor
 */

MidiMessage TrackChunk::readMidiEvent(uint8_t command) {
    uint8_t statusMasked = command & 0xf0;
    uint8_t channel = command & 0x0f;

    MidiMessage message = MidiMessage();
    message.status = command;
    message.channel = channel;
    uint32_t len = -1;
    uint8_t *eventBytes = nullptr;
    const char *desc = "Not set desc";
    switch(statusMasked) {
        case STATUS_NOTE_OFF:
            eventBytes = readBytes(2);
            len = 2;
            desc = "Note Off";
            break;
        case STATUS_NOTE_ON:
            eventBytes = readBytes(2);
            len = 2;
            desc = "Note On";
            break;
        case STATUS_CONTROL_CHANGE:
            eventBytes = readBytes(2);
            len = 2;
            desc = "Control Change";
            break;
        case STATUS_PROGRAM_CHANGE:
            eventBytes = readBytes(1);
            len = 1;
            desc = "Program Change";
            break;
        default:
            break;
    }
    message.data = eventBytes;
    message.description = desc;
    message.len = len;
    return message;
}

MidiMessage TrackChunk::readSysexEvent(uint8_t command) {
    return MidiMessage();
}

MidiMessage TrackChunk::readMetaEvent(uint8_t command) {
    uint8_t metaCommand = readByte();
    MidiMessage message = MidiMessage();
    message.status = 0xff;
    uint32_t len = 0;
    uint8_t *eventBytes = nullptr;
    const char * desc = "Not set desc meta";
    uint8_t channel = -1;
    switch (metaCommand) {
        case 0x2f:
            if (readByte() == 0) {
                _reachedEOT = true;
                desc = "End of track";
                len = 0;
            }
            break;
        case 0x03:
            desc = "Meta: Sequence/Track Name";
            len = MidiFile::parseVariableLengthQuantity(_buffer, _bufferOffset);
            eventBytes = readBytes(len);
            break;
        case 0x51:
            desc = "Set Tempo";
            len = readByte(); // This message type has the length in 1 byte and it's always 3
            eventBytes = readBytes(len);
            break;
        case 0x58:
            desc = "Time Signature";
            len = readByte(); // This message type has the length in 1 byte and it's always 4
            eventBytes = readBytes(len);
            break;
        case 0x59:
            desc = "Key Signature";
            len = readByte(); //length is always 2
            eventBytes = readBytes(len);
    }
    message.len = len;
    message.description = desc;
    message.data = eventBytes;
    message.channel = channel;
    return message;
}

uint8_t TrackChunk::readByte() {
    return _buffer[_bufferOffset++];
}

uint8_t *TrackChunk::readBytes(uint32_t amount) {
    uint8_t *temp = new uint8_t [amount];
    for (int i = 0; i < amount; i++) {
        temp[i] = readByte();
    }
    return temp;
}

void MidiFile::setHeader(const HeaderChunk &header) {
    _header = header;
}

void MidiFile::setTracks(TrackChunk *tracks) {
    _tracks = tracks;
}

int MidiFile::getNumTracks() const {
    return _numTracks;
}

void MidiFile::setNumTracks(int numTracks) {
    _numTracks = numTracks;
}

MidiFile::MidiFile(File *file) {
    _file = file;
    Serial.printf("Opened file at %s\n", _file->name());
}

File MidiFile::getFile() const {
    return _file;
}

uint32_t MidiFile::parseVariableLengthQuantity(uint8_t *buffer, uint32_t& offset) {
    uint8_t buf[4]; // max size of a VLQ is going to be 4 bytes
    uint8_t tempBuf[1];
    int numRead = 0;
    uint8_t lastRead = 0;
    do {
        lastRead = buffer[offset];
        buf[numRead] = lastRead & 127;
        numRead++;
        offset++;
    } while (lastRead & 128);
    Serial.printf("Read %d byte VLQ\n", numRead);
    uint32_t vlq = 0;
    for (int i = 0; i < numRead; i++) {
        vlq |= buf[i] << ((numRead - i - 1) * 7);
    }
    return vlq;
}
