# MIDI-Project
  In this Project the main goal is to create an executable file (play_notes) so that it can play the Mario song in the ev3 Brainstorm environment for the Lego robot , which was my Team's project at university , and in order to do so i had to first write a code in C++ (midi_to_notes.cpp) that wrote all the notes of that song from a midi file (mario.mid) with all their pitch , velocity and delay time into a txt file (notes.txt).

It is also worth mentioning that i wrote the LegoSongPlayer.cpp to check the midi file and play it on my computer with using the right libraries and modules.

External Libraries:
_MidiFile.h (from the midifile library):
Parses and extracts data from MIDI files.
Provides functions for time analysis and linking note pairs.

_portaudio.h (PortAudio library):
Cross-platform audio library used for real-time audio playback.
Manages audio streams and handles callbacks for sound output.

C++ Standard Libraries:
<iostream>: Handles input/output for error messages and logs.

<vector>: Manages the list of notes to be played.

<filesystem>: Checks the existence of the MIDI file.

<cmath>: Calculates frequencies using mathematical functions (e.g., pow, sin)
