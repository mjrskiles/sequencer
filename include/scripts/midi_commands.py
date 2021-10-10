# list of midi messages implemented in this program
# msm -> midi status message

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





