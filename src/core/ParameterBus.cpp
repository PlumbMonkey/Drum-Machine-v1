#include "ParameterBus.h"
#include <sstream>
#include <iostream>

namespace DrumMachine {

ParameterBus& ParameterBus::getInstance()
{
    static ParameterBus instance;
    return instance;
}

void ParameterBus::publish(const ParameterChange& change)
{
    // Update state cache
    std::string cacheKey = getCacheKey(change.type, change.trackIndex, change.stepIndex);
    stateCache_[cacheKey] = change.value;

    // Notify specific subscribers
    auto it = subscribers_.find(change.type);
    if (it != subscribers_.end()) {
        for (auto& callback : it->second) {
            callback(change);
        }
    }

    // Notify all-subscribers
    for (auto& callback : allSubscribers_) {
        callback(change);
    }
}

void ParameterBus::subscribe(ParameterType type, ParameterCallback callback)
{
    subscribers_[type].push_back(callback);
}

void ParameterBus::subscribeToAll(ParameterCallback callback)
{
    allSubscribers_.push_back(callback);
}

void ParameterBus::unsubscribe(ParameterType type, ParameterCallback* callback)
{
    auto it = subscribers_.find(type);
    if (it != subscribers_.end()) {
        // Note: This is a simplified approach; in production, use std::erase_if or a different mechanism
        // for removing callbacks by pointer
    }
}

ParameterValue ParameterBus::getParameterValue(ParameterType type, uint32_t trackIndex, uint32_t stepIndex) const
{
    std::string cacheKey = getCacheKey(type, trackIndex, stepIndex);
    auto it = stateCache_.find(cacheKey);
    if (it != stateCache_.end()) {
        return it->second;
    }
    // Return default empty variant
    return ParameterValue(0u);
}

void ParameterBus::setParameterValue(ParameterType type, const ParameterValue& value, uint32_t trackIndex, uint32_t stepIndex)
{
    std::string cacheKey = getCacheKey(type, trackIndex, stepIndex);
    stateCache_[cacheKey] = value;
}

void ParameterBus::reset()
{
    subscribers_.clear();
    allSubscribers_.clear();
    stateCache_.clear();
}

std::string ParameterBus::getCacheKey(ParameterType type, uint32_t trackIndex, uint32_t stepIndex) const
{
    std::ostringstream oss;
    oss << static_cast<int>(type) << "_" << trackIndex << "_" << stepIndex;
    return oss.str();
}

} // namespace DrumMachine
