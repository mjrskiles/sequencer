MThd_chunk_type = 0
MTrk_chunk_type = 1

msm_note_off = 0x80
msm_note_on  = 0x90
msm_control_change = 0xb0
msm_program_change = 0xc0
msm_sysex = 0xf0

# sysex messages
sysex_meta_event = 0xff
meta_track_name = 0xff_03 # ll_ll text
meta_end_of_track = 0xff_02_00
meta_set_tempo = 0xff_51_03 # tt_tt_tt time in microseconds per quarter note
meta_time_sig = 0xff_58_04 # nn dd cc bb : numerator (dec), denom (dd^-2), midi clocks/metronome tick, 32nd notes per quarter
meta_key_sig = 0xff_59_02 # sf mi : sharp/flats major/minor -7 = 7 flats, 0 = C, 7 = 7 sharps; 0 = major 1 minor

def specify_channel_nibble(message, channel):
    return message | channel

class MidiChunk:
    def __init__(self, f):
        self.gobble(f)

    def __str__(self):
        indent = 0

    # gobble up the bytes until the end of the chunk and put them in a buffer to stream
    # at the point the file is passed to this method the chunk type has been read
    # so it should be pointing to the size
    def gobble(self, f):
        self.chunk_size = f.read(4)
        self.buffer = f.read(self.chunk_size)

    # variable length quantities have bit 7 set on all bytes except the last, which has bit 7 clear.
    # typically they will be 1-4 bytes



class HeaderChunk(MidiChunk):
    def __init__(self, f):
        self.chunk_type = MThd_chunk_type
        super.__init__(f)

class TrackChunk(MidiChunk):
    def __init__(self, f):
        self.chunk_type = MTrk_chunk_type
        super.__init__(f)
