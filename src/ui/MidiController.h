#ifndef MIDI_CONTROLLER_H
#define MIDI_CONTROLLER_H

#include <memory>
#include <cstdint>

namespace DrumMachine {

class MidiManager;

/**
 * MidiController
 * 
 * UI panel for MIDI port selection and configuration.
 * Displays available MIDI ports and allows user to connect/disconnect.
 * Shows MIDI activity and incoming note/CC data.
 * 
 * Milestone 5: MIDI UI
 */
class MidiController {
public:
    MidiController();

    // Render MIDI control panel
    void render(MidiManager* midiManager);

private:
    int selectedPortIndex_;
    bool showPortList_;
    uint32_t lastMidiMessageTime_;
};

} // namespace DrumMachine

#endif // MIDI_CONTROLLER_H
