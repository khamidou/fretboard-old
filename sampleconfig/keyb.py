# This config file example shows how to emulate a keyboard

import vkbd   # This module is provided by fretboard

def callback(freq):

	f = round(freq)

	if freq <= 84:
		vkbd.sendkey(65) # sixth string - fret number 0 or 1- space

	if 103 <= freq <= 110:
		vkbd.sendkey(9) # fifth string - fret number 0 or 1 - escape

	if 138 <= freq <= 146:
		vkbd.sendkey(23) # fourth string - fret number 0 or 1 - tab

	if 
