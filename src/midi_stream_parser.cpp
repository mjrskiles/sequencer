//
// Created by mjrsk on 10/9/2021.
//

/*
 *      _format |= _buffer[0] << 8;
        _format |= _buffer[1];
        _numTracks |= _buffer[2] << 8;
        _numTracks |= _buffer[3];
        _division |= _buffer[4] << 8;
        _division |= _buffer[5];
 */

#include <midi_stream_parser.h>
#include <midi_stream.h>
#include "utils.h"

// Testing
#include <Arduino.h>

void MidiMessage::print() {
    Serial.printf("Status word: %x | Channel: %x\n", status, channel);
    Serial.println("Data");
    printBuffer(data, len);
}

void TrackEvent::print() {
    Serial.printf("Track Event\nDelta T %d\n", deltaT);
    event.print();
}

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
        if (_format == 1) {
            readTrackStart();
            while (runningNumBytesRead() < getCurrentChunkLength()) {
                readEventAndPrint();
                Serial.printf("Running bytes: %d\n", runningNumBytesRead());
            }
        }
    }
}

// This is called in the constructor
void MidiParser::readHeader() {
    // the next 4 bytes are always 00 00 00 06, don't need them
    uint32_t headerLength = 0;
    headerLength |= _midiStream.nextByte() << 24;
    headerLength |= _midiStream.nextByte() << 16;
    headerLength |= _midiStream.nextByte() << 8;
    headerLength |= _midiStream.nextByte() << 0;
    Serial.printf("Header len: %d\n", headerLength);
    _format |= _midiStream.nextByte() << 8;
    _format |= _midiStream.nextByte();
    _numTracks |= _midiStream.nextByte() << 8;
    _numTracks |= _midiStream.nextByte();
    _divisionType |= _midiStream.nextByte() << 8;
    _divisionType |= _midiStream.nextByte();
}

void MidiParser::readTrackStart() {
    uint8_t track_chunk_byte_pattern[] {0x4d, 0x54, 0x72, 0x6b};
    uint8_t first4bytes[4];
    first4bytes[0] = _midiStream.nextByte();
    first4bytes[1] = _midiStream.nextByte();
    first4bytes[2] = _midiStream.nextByte();
    first4bytes[3] = _midiStream.nextByte();
    if (compareArrays(track_chunk_byte_pattern, first4bytes, 4)){
        Serial.println("Started track");
    } else {
        Serial.println("Expected 'MTrk' but read");
        printBuffer(first4bytes, 4);
    }
    uint32_t trackLength = 0;
    trackLength |= _midiStream.nextByte() << 24;
    trackLength |= _midiStream.nextByte() << 16;
    trackLength |= _midiStream.nextByte() << 8;
    trackLength |= _midiStream.nextByte() << 0;
    _currentChunkLength = trackLength;
    Serial.printf("Read track length %d\n", _currentChunkLength);
    resetRunningNum();
}

void MidiParser::printHeaderInfo() {
    Serial.printf("Track Header\n");
    Serial.printf("Format: %x | # of tracks: %d | Div Type: %x\n", _format, _numTracks, _divisionType);
}

TrackEvent MidiParser::readEvent() {
    MidiMessage message = MidiMessage();
    uint32_t deltaT = readVariableLengthQuantity();
    uint8_t eventType = _midiStream.nextByte();


    if (eventType >= 0x80) {
        // has a 1 in the leftmost bit, is a status word
        if (eventType == SYSEX_META) {
            message.isMeta = true;
            message.metaWord = _midiStream.nextByte();
            message.len = readVariableLengthQuantity();
            Serial.printf("Read meta event with command %x and length %d\n", message.metaWord, message.len);
            for (uint32_t i = 0; i < message.len; i++) {
                message.data[i] = _midiStream.nextByte();
            }
        } else {
            switch ((eventType & 0xf0)) {
                case PROGRAM_CHANGE:
                    message.status = PROGRAM_CHANGE;
                    message.len = 1;
                    message.data[0] = _midiStream.nextByte();
                    break;
                case CHANNEL_PRESSURE:
                    // The only 2 event types with 1 data byte
                    message.status = CHANNEL_PRESSURE;
                    message.len = 1;
                    message.data[0] = _midiStream.nextByte();
                    break;
                case NOTE_OFF:
                    message.status = NOTE_OFF;
                    message.len = 2;
                    message.data[0] = _midiStream.nextByte();
                    message.data[1] = _midiStream.nextByte();
                    break;
                case NOTE_ON:
                    message.status = NOTE_ON;
                    message.len = 2;
                    message.data[0] = _midiStream.nextByte();
                    message.data[1] = _midiStream.nextByte();
                    break;
                default: // TODO might be worth stating the cases explicitly
                    message.status = MidiEvents(eventType & 0xf0);
                    message.len = 2;
                    message.data[0] = _midiStream.nextByte();
                    message.data[1] = _midiStream.nextByte();
                    break;
            }
        }
    } else {
        uint8_t dataByte1 = eventType; // the byte was actually a running status data byte
        message.status = _lastEvent;
        message.len = 2;
        message.data[0] = dataByte1;
        message.data[1] = _midiStream.nextByte();
    }
    TrackEvent event = TrackEvent();
    event.deltaT = deltaT;
    event.event = message;
    _lastEvent = message.status;
    return event;
}

TrackEvent MidiParser::readEventAndPrint() {
    TrackEvent trackEvent = readEvent();
    trackEvent.print();
    Serial.printf("---\n");
    return trackEvent;
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
    Serial.printf("Assembled VLQ value %d\n", vlq);
    return vlq;
}

uint32_t MidiParser::readInt32() {
    uint32_t uint32 = 0;
    uint32 |= _midiStream.nextByte() << 24;
    uint32 |= _midiStream.nextByte() << 16;
    uint32 |= _midiStream.nextByte() << 8;
    uint32 |= _midiStream.nextByte();
    return uint32;
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

uint32_t MidiParser::runningNumBytesRead() {
    return _midiStream.getRunningNumRead();
}

void MidiParser::resetRunningNum() {
    _midiStream.resetRunningNumBytes();
}

uint32_t MidiParser::getCurrentChunkLength() const {
    return _currentChunkLength;
}
