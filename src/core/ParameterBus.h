#ifndef PARAMETER_BUS_H
#define PARAMETER_BUS_H

#include <functional>
#include <map>
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <cstdint>

namespace DrumMachine {

/**
 * ParameterBus
 * 
 * Central event system for all parameter changes.
 * Decouples UI, sequencer state, and 3D rendering.
 * Allows modules to publish/subscribe to parameter changes.
 * Foundation for modular DAW architecture.
 * 
 * Milestone 4: Parameter Bus infrastructure
 */

// Parameter value types
using ParameterValue = std::variant<
    bool,           // Toggle/mute flags
    float,          // Continuous values (tempo, swing, volume)
    uint32_t,       // Indices, counts
    std::string     // File paths, names
>;

// Parameter types that can be published
enum class ParameterType {
    // Transport
    TEMPO,
    SWING,
    TIME_SIGNATURE,
    PLAY_STATE,
    
    // Pattern/Sequencer
    CURRENT_STEP,
    STEP_ACTIVE,
    TRACK_MUTED,
    TRACK_VOLUME,
    TRACK_PAN,
    TRACK_SAMPLE,
    
    // UI/Rendering
    PLAYHEAD_POSITION,
    FRAME_COUNT,
    
    // 3D Integration
    BUTTON_PRESSED,
    KNOB_TURNED,
    FADER_MOVED,
    LED_COLOR_CHANGED
};

struct ParameterChange {
    ParameterType type;
    ParameterValue value;
    uint32_t trackIndex;  // For track-specific parameters
    uint32_t stepIndex;   // For step-specific parameters
    std::string moduleId; // Which module published this (e.g., "drum_machine", "ui", "3d_renderer")
};

class ParameterBus {
public:
    static ParameterBus& getInstance();

    // Publish a parameter change to all subscribers
    void publish(const ParameterChange& change);

    // Subscribe to parameter changes
    using ParameterCallback = std::function<void(const ParameterChange&)>;
    void subscribe(ParameterType type, ParameterCallback callback);
    void subscribeToAll(ParameterCallback callback);

    // Unsubscribe (typically called by destructors)
    void unsubscribe(ParameterType type, ParameterCallback* callback);

    // Query current parameter value (state cache)
    ParameterValue getParameterValue(ParameterType type, uint32_t trackIndex = 0, uint32_t stepIndex = 0) const;
    void setParameterValue(ParameterType type, const ParameterValue& value, uint32_t trackIndex = 0, uint32_t stepIndex = 0);

    // Clear all subscribers and state (for testing)
    void reset();

private:
    ParameterBus() = default;
    ~ParameterBus() = default;

    // Prevent copying
    ParameterBus(const ParameterBus&) = delete;
    ParameterBus& operator=(const ParameterBus&) = delete;

    // Subscribers map: ParameterType -> list of callbacks
    std::map<ParameterType, std::vector<ParameterCallback>> subscribers_;
    std::vector<ParameterCallback> allSubscribers_;

    // State cache: Store current parameter values
    std::map<std::string, ParameterValue> stateCache_;

    // Helper to create cache key
    std::string getCacheKey(ParameterType type, uint32_t trackIndex, uint32_t stepIndex) const;
};

} // namespace DrumMachine

#endif // PARAMETER_BUS_H
