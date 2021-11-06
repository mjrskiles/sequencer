//
// Created by Michael Skiles on 10/10/21.
//

#ifndef SEQUENCER_MIDISTREAM_H
#define SEQUENCER_MIDISTREAM_H

/*
 * Midi stream class is kinda like a file class, maintains a pointer into a byte array
 */

#include <cstdint>

class ByteStream {
public:
    /*
     * Initialize the pointer into the uint8_t array
     */
    ByteStream(uint8_t *fileBytes, uint32_t size);
    ByteStream() { _stream = nullptr; }

    bool hasNext();
    uint8_t nextByte();

    void setSize(uint32_t size);
    uint32_t getSize() const;

    void resetRunningNumBytes();

    uint32_t getRunningNumRead() const;
    void setStream( uint8_t *stream, uint16_t size);

    uint8_t *getStream() const;

protected:


    uint8_t *_stream;
    uint32_t _size = 0;
    uint32_t _indexOffset = 0;
public:
    uint32_t getIndexOffset() const;

protected:
    uint32_t _runningNumRead = 0;
};

#endif //SEQUENCER_MIDISTREAM_H
