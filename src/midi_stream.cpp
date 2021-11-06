//
// Created by Michael Skiles on 10/10/21.
//

#include "midi_stream.h"

ByteStream::ByteStream(uint8_t *fileBytes, uint32_t size) {
    _stream = fileBytes;
    _size = size;
    _indexOffset = 0;
}

bool ByteStream::hasNext() {
    return _indexOffset < _size;
}

uint8_t ByteStream::nextByte() {
    _runningNumRead++;
    return _stream[_indexOffset++];
}

uint32_t ByteStream::getSize() const {
    return _size;
}

uint32_t ByteStream::getRunningNumRead() const {
    return _runningNumRead;
}

void ByteStream::resetRunningNumBytes() {
    _runningNumRead = 0;
}

void ByteStream::setSize(uint32_t size) {
    _size = size;
}

void ByteStream::setStream(uint8_t *stream, uint16_t size) {
    _stream = stream;
    _size = size;
}

uint8_t *ByteStream::getStream() const {
    return _stream;
}

uint32_t ByteStream::getIndexOffset() const {
    return _indexOffset;
}
