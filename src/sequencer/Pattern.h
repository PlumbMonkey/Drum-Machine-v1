#ifndef PATTERN_H
#define PATTERN_H

#include <cstdint>
#include <string>
#include <array>

namespace DrumMachine {

/**
 * Pattern
 * 
 * A single drum pattern with 8 tracks and 16 steps.
 * Data model for storage and playback.
 * Milestone 2: Pattern data model and step management
 */
class Pattern {
public:
    static constexpr uint32_t NUM_TRACKS = 8;
    static constexpr uint32_t STEPS_PER_BAR = 16;
    static constexpr uint32_t MAX_BARS = 5;

    enum class TrackType {
        Kick,
        Snare,
        HatClosed,
        HatOpen,
        Perc1,
        Perc2,
        Tom1,
        Tom2
    };

    struct Track {
        std::string name;
        TrackType type;
        std::string samplePath;
        float volume;
        bool muted;
        std::array<uint8_t, STEPS_PER_BAR> steps; // 1 = active, 0 = inactive
    };

    Pattern();

    // Track management
    Track& getTrack(uint32_t trackIndex);
    const Track& getTrack(uint32_t trackIndex) const;

    // Step management
    bool isStepActive(uint32_t trackIndex, uint32_t stepIndex) const;
    void setStepActive(uint32_t trackIndex, uint32_t stepIndex, bool active);

    // Track volume and mute
    void setTrackVolume(uint32_t trackIndex, float volume);
    float getTrackVolume(uint32_t trackIndex) const;

    void setTrackMuted(uint32_t trackIndex, bool muted);
    bool isTrackMuted(uint32_t trackIndex) const;

    // Sample assignment
    void setTrackSample(uint32_t trackIndex, const std::string& samplePath);
    std::string getTrackSample(uint32_t trackIndex) const;

private:
    std::array<Track, NUM_TRACKS> tracks_;

    // Initialize default tracks with GM drum mapping
    void initializeDefaultTracks();
};

} // namespace DrumMachine

#endif // PATTERN_H
