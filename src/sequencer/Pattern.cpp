#include "Pattern.h"
#include <algorithm>
#include <iostream>

namespace DrumMachine {

Pattern::Pattern()
{
    initializeDefaultTracks();
}

void Pattern::initializeDefaultTracks()
{
    // Initialize 8 tracks with default GM drum mapping
    const std::array<std::pair<std::string, TrackType>, NUM_TRACKS> defaults = {{
        {"Kick", TrackType::Kick},
        {"Snare", TrackType::Snare},
        {"Hat Closed", TrackType::HatClosed},
        {"Hat Open", TrackType::HatOpen},
        {"Perc 1", TrackType::Perc1},
        {"Perc 2", TrackType::Perc2},
        {"Tom 1", TrackType::Tom1},
        {"Tom 2", TrackType::Tom2}
    }};

    for (uint32_t i = 0; i < NUM_TRACKS; ++i) {
        tracks_[i].name = defaults[i].first;
        tracks_[i].type = defaults[i].second;
        tracks_[i].samplePath = "";
        tracks_[i].volume = 0.8f;
        tracks_[i].muted = false;
        tracks_[i].steps.fill(0); // All steps off by default
    }
}

Pattern::Track& Pattern::getTrack(uint32_t trackIndex)
{
    return tracks_[trackIndex];
}

const Pattern::Track& Pattern::getTrack(uint32_t trackIndex) const
{
    return tracks_[trackIndex];
}

bool Pattern::isStepActive(uint32_t trackIndex, uint32_t stepIndex) const
{
    return tracks_[trackIndex].steps[stepIndex] != 0;
}

void Pattern::setStepActive(uint32_t trackIndex, uint32_t stepIndex, bool active)
{
    tracks_[trackIndex].steps[stepIndex] = active ? 1 : 0;
    std::cout << "[PATTERN] Track " << trackIndex << " Step " << stepIndex 
              << " set to " << (active ? "ON" : "OFF") << std::endl;
}

void Pattern::setTrackVolume(uint32_t trackIndex, float volume)
{
    tracks_[trackIndex].volume = std::clamp(volume, 0.0f, 1.0f);
}

float Pattern::getTrackVolume(uint32_t trackIndex) const
{
    return tracks_[trackIndex].volume;
}

void Pattern::setTrackMuted(uint32_t trackIndex, bool muted)
{
    tracks_[trackIndex].muted = muted;
}

bool Pattern::isTrackMuted(uint32_t trackIndex) const
{
    return tracks_[trackIndex].muted;
}

void Pattern::setTrackSample(uint32_t trackIndex, const std::string& samplePath)
{
    tracks_[trackIndex].samplePath = samplePath;
}

std::string Pattern::getTrackSample(uint32_t trackIndex) const
{
    return tracks_[trackIndex].samplePath;
}

} // namespace DrumMachine
