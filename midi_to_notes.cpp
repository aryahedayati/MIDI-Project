#include "MidiFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cmath>

#define SAMPLE_RATE 44100
#define TEMPO_SCALING 0.8  // Adjust for tempo scaling
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Note {
    int frequency;   // Frequency in Hz
    int duration;    // Duration in milliseconds
};

// MIDI note to frequency conversion (MIDI note 69 = A440)
int midiToFrequency(int midiNote) {
    return static_cast<int>(440.0 * pow(2.0, (midiNote - 69) / 12.0));
}

// Convert MIDI file to notes and save them to a file
void convertMidiToFile(const std::string& inputMidiPath, const std::string& outputFilePath) {
    smf::MidiFile midiFile;

    // Check if the MIDI file exists
    if (!std::filesystem::exists(inputMidiPath)) {
        std::cerr << "Error: MIDI file does not exist at path: " << inputMidiPath << std::endl;
        return;
    }

    // Read the MIDI file
    if (!midiFile.read(inputMidiPath)) {
        std::cerr << "Failed to read MIDI file!" << std::endl;
        return;
    }

    // Analyze timing and link note pairs
    midiFile.doTimeAnalysis();
    midiFile.linkNotePairs();

    // Open output file for writing
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file: " << outputFilePath << std::endl;
        return;
    }

    std::cout << "Output file opened successfully at: " << outputFilePath << std::endl;

    // Process MIDI tracks and events
    for (int track = 0; track < midiFile.getTrackCount(); ++track) {
        for (int event = 0; event < midiFile[track].size(); ++event) {
            if (midiFile[track][event].isNoteOn()) {
                int midiNote = midiFile[track][event][1];
                double duration = midiFile[track][event].getDurationInSeconds();
                int frequency = midiToFrequency(midiNote);

                if (duration <= 0.0) {
                    std::cerr << "Skipping note with zero or invalid duration." << std::endl;
                    continue;
                }

                int adjustedDuration = static_cast<int>(duration * 1000 * TEMPO_SCALING);

                // Debugging output
                std::cout << "MIDI Note: " << midiNote
                    << ", Frequency: " << frequency
                    << ", Duration: " << duration
                    << " seconds (Adjusted: " << adjustedDuration << " ms)" << std::endl;

                // Save the note data to the output file
                outputFile << frequency << " " << adjustedDuration << std::endl;
            }
        }
    }

    // Close the output file
    outputFile.close();
    std::cout << "Notes have been successfully saved to " << outputFilePath << std::endl;
}

int main() {
    // Define paths for input MIDI and output notes file
    const std::string inputMidiPath = "C:\\Users\\Asus\\Desktop\\MIDI_Project\\LegoSongPlayer\\song.mid";
    const std::string outputFilePath = "C:\\Users\\Asus\\Desktop\\MIDI_Project\\LegoSongPlayer\\notes.txt";

    // Convert MIDI to notes and save to file
    convertMidiToFile(inputMidiPath, outputFilePath);

    return 0;
}