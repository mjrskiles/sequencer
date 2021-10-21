//
// Created by Michael Skiles on 10/10/21.
//

#include "midi_stream.h"

MidiStream::MidiStream(uint8_t *fileBytes, uint32_t size) {
    _stream = fileBytes;
    _size = size; // This works because it's a byte array
    _indexOffset = 0;
}

bool MidiStream::hasNext() {
    return _indexOffset < _size;
}

uint8_t MidiStream::nextByte() {
    _runningNumRead++;
    return _stream[_indexOffset++];
}

uint32_t MidiStream::getSize() const {
    return _size;
}

uint32_t MidiStream::getRunningNumRead() const {
    return _runningNumRead;
}

void MidiStream::resetRunningNumBytes() {
    _runningNumRead = 0;
}
