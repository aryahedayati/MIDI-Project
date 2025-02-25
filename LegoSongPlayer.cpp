#include "MidiFile.h"
#include "portaudio.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <cmath>

#define SAMPLE_RATE 44100
#define AMPLITUDE 0.5
#define TEMPO_SCALING 0.7  // Safer tempo scaling factor
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct NoteData {
    double phase;
    double frequency;
    int samplesRemaining;
};

int midiToFrequency(int midiNote) {
    return static_cast<int>(440.0 * pow(2.0, (midiNote - 69) / 12.0));
}

std::vector<NoteData> convertMidiToNotes(const smf::MidiFile& midiFile) {
    std::vector<NoteData> notes;

    for (int track = 0; track < midiFile.getTrackCount(); ++track) {
        for (int event = 0; event < midiFile[track].size(); ++event) {
            if (midiFile[track][event].isNoteOn()) {
                int midiNote = midiFile[track][event][1];
                double duration = midiFile[track][event].getDurationInSeconds();
                double frequency = midiToFrequency(midiNote);

                if (duration <= 0.0) {
                    std::cerr << "Skipping note with zero or invalid duration." << std::endl;
                    continue;
                }

                int adjustedSamples = static_cast<int>(duration * SAMPLE_RATE * TEMPO_SCALING);
                std::cout << "MIDI Note: " << midiNote
                    << ", Frequency: " << frequency
                    << ", Duration: " << duration
                    << " seconds (Adjusted Samples: " << adjustedSamples << ")" << std::endl;

                notes.push_back({ 0.0, frequency, adjustedSamples });
            }
        }
    }

    return notes;
}

static int audioCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void* userData) {
    float* out = (float*)outputBuffer;
    NoteData* note = (NoteData*)userData;

    for (unsigned int i = 0; i < framesPerBuffer; ++i) {
        if (note->samplesRemaining > 0) {
            *out++ = AMPLITUDE * sin(note->phase);
            note->phase += 2.0 * M_PI * note->frequency / SAMPLE_RATE;
            if (note->phase >= 2.0 * M_PI) note->phase -= 2.0 * M_PI;
            note->samplesRemaining--;
        }
        else {
            *out++ = 0.0f;
        }
    }

    std::cout << "Audio Callback - Samples Remaining: " << note->samplesRemaining << std::endl;

    return note->samplesRemaining > 0 ? paContinue : paComplete;
}

void playNotesWithPortAudio(const std::vector<NoteData>& notes) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    for (const auto& note : notes) {
        NoteData currentNote = note;

        PaStream* stream;
        err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 256, audioCallback, &currentNote);
        if (err != paNoError) {
            std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
            continue;
        }

        err = Pa_StartStream(stream);
        if (err != paNoError) {
            std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
            Pa_CloseStream(stream);
            continue;
        }

        while (Pa_IsStreamActive(stream) == 1) {
            Pa_Sleep(10);  // Adjusted sleep for faster transitions
        }

        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }

    Pa_Terminate();
}

int main() {
    smf::MidiFile midiFile;

    if (!std::filesystem::exists("C:\\Users\\Asus\\Desktop\\MIDI_Project\\LegoSongPlayer\\song.mid")) {
        std::cerr << "Error: MIDI file does not exist!" << std::endl;
        return 1;
    }

    if (!midiFile.read("C:\\Users\\Asus\\Desktop\\MIDI_Project\\LegoSongPlayer\\song.mid")) {
        std::cerr << "Failed to read MIDI file!" << std::endl;
        return 1;
    }

    midiFile.doTimeAnalysis();
    midiFile.linkNotePairs();

    auto notes = convertMidiToNotes(midiFile);
    if (notes.empty()) {
        std::cerr << "No playable notes found in the MIDI file!" << std::endl;
        return 1;
    }

    playNotesWithPortAudio(notes);
    return 0;
}
