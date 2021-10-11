//
// Created by mjrsk on 10/9/2021.
//

/*
 * _format |= _buffer[0] << 8;
        _format |= _buffer[1];
        _numTracks |= _buffer[2] << 8;
        _numTracks |= _buffer[3];
        _division |= _buffer[4] << 8;
        _division |= _buffer[5];
 */

#include <midi_stream_parser.h>
#include <midi_stream.h>

// Testing
#include <Arduino.h>

MidiParser::MidiParser(MidiStream stream) : _midiStream(stream) {
    uint8_t header_chunk_byte_pattern[] {0x4d, 0x54, 0x68, 0x64};
    uint8_t first4bytes[4];
    first4bytes[0] = _midiStream.nextByte();
    first4bytes[1] = _midiStream.nextByte();
    first4bytes[2] = _midiStream.nextByte();
    first4bytes[3] = _midiStream.nextByte();
    if (compareArrays(header_chunk_byte_pattern, first4bytes, 4)){
        _available = true;
        readHeader();
    }
}

void MidiParser::readHeader() {
    // the next 4 bytes are always 00 00 00 06, don't need them
    for (int i = 0; i < 4; i++) {
        _midiStream.nextByte();
    }
    _format |= _midiStream.nextByte() << 8;
    _format |= _midiStream.nextByte();
    _numTracks |= _midiStream.nextByte() << 8;
    _numTracks |= _midiStream.nextByte();
    _divisionType |= _midiStream.nextByte() << 8;
    _divisionType |= _midiStream.nextByte();
}

TrackEvent MidiParser::readEvent() {
    MidiMessage message = MidiMessage();
    uint32_t deltaT = readVariableLengthQuantity();
    uint8_t eventType = _midiStream.nextByte();

    if (eventType == SYSEX_META) {
        message.isMeta = true;
        message.metaWord = _midiStream.nextByte();
        message.len = readVariableLengthQuantity();
        for (uint32_t i = 0; i < message.len; i++) {
            message.data[i] = _midiStream.nextByte();
        }
    } else {
        switch ((eventType & 0xf0)) {
            case PROGRAM_CHANGE:
            case CHANNEL_PRESSURE:
                // The only 2 event types with 1 data byte
                message.len = 1;
                message.data[0] = _midiStream.nextByte();
                break;
            default: // TODO might be worth stating the cases explicitly
                message.len = 2;
                message.data[0] = _midiStream.nextByte();
                message.data[1] = _midiStream.nextByte();
                break;
        }
    }
    TrackEvent event = TrackEvent();
    event.deltaT = deltaT;
    event.event = message;
    return event;
}

uint32_t MidiParser::readVariableLengthQuantity() {
    uint8_t buf[4]; // max size of a VLQ is going to be 4 bytes
    int numRead = 0;
    uint8_t lastRead = 0;
    do {
        lastRead = _midiStream.nextByte();
        buf[numRead] = lastRead & 127;
        Serial.printf("VLQ: read byte %x\n", lastRead);
        numRead++;
    } while (lastRead & (uint8_t)128);
    uint32_t vlq = 0;
    for (int i = 0; i < numRead; i++) {
        vlq |= buf[i] << ((numRead - i - 1) * 7);
    }
    return vlq;
}

bool MidiParser::isAvailable() const {
    return _available;
}

bool MidiParser::compareArrays(uint8_t *a1, uint8_t *a2, int commonSize) {
    for (int i = 0; i < commonSize; i++) {
        if (a1[i] != a2[i]) {
            return false;
        }
    }
    return true;
}

void MidiParser::advanceBy(uint8_t numBytes) {
    for(uint8_t i = 0; i < numBytes; i++) {
        _midiStream.nextByte();
    }
}
