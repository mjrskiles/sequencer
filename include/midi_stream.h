//
// Created by Michael Skiles on 10/10/21.
//

#ifndef SEQUENCER_MIDISTREAM_H
#define SEQUENCER_MIDISTREAM_H

/*
 * Midi stream class is kinda like a file class, maintains a pointer into a byte array
 */

#include <cstdint>

class MidiStream {
public:
    /*
     * Initialize the pointer into the uint8_t array
     */
    MidiStream(uint8_t *fileBytes, uint32_t size);

    bool hasNext();
    uint8_t nextByte();

    uint32_t getSize() const;

protected:
    uint8_t *_stream;
    uint32_t _size;
    uint32_t _indexOffset;

};

#endif //SEQUENCER_MIDISTREAM_H
