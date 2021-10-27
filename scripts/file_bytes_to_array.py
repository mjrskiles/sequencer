# take a string and an integer from the command line and produce a text file
# with an enumerated list of the string from 1 to n.

import os
import sys

file_path = "../midi-files/smb_cstl.mid"
list_for_file = ""

midi_file = open(file_path, 'rb')

i = 0
byte = midi_file.read(1)
while byte:
    list_for_file += "0x{}, ".format(byte.hex())
    if (i+1) % 16 == 0:
        list_for_file += "\n"
    byte = midi_file.read(1)
    i += 1

target = 'target/mario-castle-bytes.txt'
if os.path.exists(target):
    os.remove(target)
f = open(target, 'w')
f.write(list_for_file)
f.close()



