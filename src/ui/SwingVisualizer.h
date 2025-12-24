#ifndef SWING_VISUALIZER_H
#define SWING_VISUALIZER_H

#include <cstdint>

namespace DrumMachine {

class Sequencer;

/**
 * SwingVisualizer
 * 
 * Renders a visual representation of swing timing offsets.
 * Shows which steps are delayed by swing and by how much.
 * Helps validate swing logic visually.
 */
class SwingVisualizer {
public:
    SwingVisualizer();

    // Render the swing visualization timeline
    void render(Sequencer* sequencer);

private:
    // Helper to calculate swing delay for a given step
    float calculateStepOffset(Sequencer* sequencer, uint32_t stepIndex);
};

} // namespace DrumMachine

#endif // SWING_VISUALIZER_H
