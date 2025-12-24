#include "MidiManager.h"
#include "../core/ParameterBus.h"
#include <RtMidi.h>
#include <iostream>
#include <cstring>

namespace DrumMachine {

// RtMidi wrapper to avoid exposing RtMidi.h in header
class MidiManager::RtMidiWrapper {
public:
    RtMidiWrapper() : midiIn(nullptr) {}
    ~RtMidiWrapper() {
        if (midiIn) {
            delete midiIn;
        }
    }

    RtMidiIn* midiIn;
};

MidiManager::MidiManager()
    : isActive_(false), rtMidiIn_(std::make_unique<RtMidiWrapper>())
{
    std::memset(controlValues_, 0, sizeof(controlValues_));
}

MidiManager::~MidiManager()
{
    shutdown();
}

bool MidiManager::initialize()
{
    try {
        rtMidiIn_->midiIn = new RtMidiIn();
        std::cout << "MIDI Manager initialized. Found " 
                  << rtMidiIn_->midiIn->getPortCount() << " input ports." << std::endl;
        return true;
    } catch (const RtMidiError& e) {
        std::cerr << "MIDI Error: " << e.what() << std::endl;
        return false;
    }
}

void MidiManager::shutdown()
{
    if (rtMidiIn_ && rtMidiIn_->midiIn) {
        rtMidiIn_->midiIn->closePort();
        isActive_ = false;
    }
}

bool MidiManager::isActive() const
{
    return isActive_ && rtMidiIn_ && rtMidiIn_->midiIn;
}

std::vector<std::string> MidiManager::getInputPorts() const
{
    std::vector<std::string> ports;
    
    if (!rtMidiIn_ || !rtMidiIn_->midiIn) {
        return ports;
    }

    uint32_t portCount = rtMidiIn_->midiIn->getPortCount();
    for (uint32_t i = 0; i < portCount; ++i) {
        try {
            ports.push_back(rtMidiIn_->midiIn->getPortName(i));
        } catch (const RtMidiError& e) {
            std::cerr << "Error getting port name: " << e.what() << std::endl;
        }
    }

    return ports;
}

bool MidiManager::openPort(uint32_t portIndex)
{
    try {
        if (!rtMidiIn_ || !rtMidiIn_->midiIn) {
            return false;
        }

        if (portIndex >= rtMidiIn_->midiIn->getPortCount()) {
            std::cerr << "Invalid MIDI port index: " << portIndex << std::endl;
            return false;
        }

        rtMidiIn_->midiIn->openPort(portIndex);
        isActive_ = true;
        std::cout << "Opened MIDI port: " << rtMidiIn_->midiIn->getPortName(portIndex) << std::endl;
        return true;
    } catch (const RtMidiError& e) {
        std::cerr << "MIDI Error: " << e.what() << std::endl;
        return false;
    }
}

bool MidiManager::openVirtualPort(const std::string& portName)
{
    try {
        if (!rtMidiIn_ || !rtMidiIn_->midiIn) {
            return false;
        }

        rtMidiIn_->midiIn->openVirtualPort(portName);
        isActive_ = true;
        std::cout << "Opened virtual MIDI port: " << portName << std::endl;
        return true;
    } catch (const RtMidiError& e) {
        // Virtual ports may not be supported on all platforms
        std::cerr << "Virtual MIDI port not supported: " << e.what() << std::endl;
        return false;
    }
}

uint32_t MidiManager::processMessages()
{
    if (!isActive()) {
        return 0;
    }

    uint32_t messagesProcessed = 0;

    try {
        std::vector<uint8_t> message;
        double stamp = 0.0;

        while (true) {
            stamp = rtMidiIn_->midiIn->getMessage(&message);
            if (message.empty()) {
                break;
            }

            if (message.size() < 1) {
                continue;
            }

            uint8_t status = message[0];
            uint8_t type = status & 0xF0;
            uint8_t channel = status & 0x0F;

            MidiMessage midiMsg;
            midiMsg.channel = channel;
            midiMsg.timestamp = static_cast<uint64_t>(stamp * 1000000);  // Convert to microseconds

            // Parse MIDI message
            if (type == 0x90 && message.size() >= 3) {  // NOTE_ON
                midiMsg.type = MidiMessage::Type::NOTE_ON;
                midiMsg.note = message[1];
                midiMsg.velocity = message[2];
            } else if (type == 0x80 && message.size() >= 3) {  // NOTE_OFF
                midiMsg.type = MidiMessage::Type::NOTE_OFF;
                midiMsg.note = message[1];
                midiMsg.velocity = message[2];
            } else if (type == 0xB0 && message.size() >= 3) {  // CONTROL_CHANGE
                midiMsg.type = MidiMessage::Type::CONTROL_CHANGE;
                midiMsg.controller = message[1];
                midiMsg.value = message[2];
                controlValues_[midiMsg.controller] = midiMsg.value;
            } else if (type == 0xC0 && message.size() >= 2) {  // PROGRAM_CHANGE
                midiMsg.type = MidiMessage::Type::PROGRAM_CHANGE;
                midiMsg.value = message[1];
            } else if (type == 0xE0 && message.size() >= 3) {  // PITCH_BEND
                midiMsg.type = MidiMessage::Type::PITCH_BEND;
                midiMsg.value = ((message[2] << 7) | message[1]) & 0x3FFF;
            } else {
                midiMsg.type = MidiMessage::Type::UNKNOWN;
            }

            processMidiMessage(midiMsg);
            messagesProcessed++;
            message.clear();
        }
    } catch (const RtMidiError& e) {
        std::cerr << "MIDI Error during message processing: " << e.what() << std::endl;
    }

    return messagesProcessed;
}

void MidiManager::setMidiCallback(MidiCallback callback)
{
    midiCallback_ = callback;
}

uint8_t MidiManager::getControlValue(uint8_t controller) const
{
    if (controller < 120) {
        return controlValues_[controller];
    }
    return 0;
}

void MidiManager::processMidiMessage(const MidiMessage& msg)
{
    // Execute user callback if set
    if (midiCallback_) {
        midiCallback_(msg);
    }

    // Publish to ParameterBus
    ParameterChange change;
    change.moduleId = "midi_manager";

    switch (msg.type) {
        case MidiMessage::Type::NOTE_ON:
            change.type = ParameterType::BUTTON_PRESSED;
            change.value = static_cast<uint32_t>(msg.note);
            change.trackIndex = msg.channel;
            ParameterBus::getInstance().publish(change);
            std::cout << "MIDI NOTE_ON: Ch=" << static_cast<int>(msg.channel)
                      << " Note=" << static_cast<int>(msg.note)
                      << " Vel=" << static_cast<int>(msg.velocity) << std::endl;
            break;

        case MidiMessage::Type::NOTE_OFF:
            change.type = ParameterType::BUTTON_PRESSED;
            change.value = 0u;  // Release signal
            change.trackIndex = msg.channel;
            ParameterBus::getInstance().publish(change);
            break;

        case MidiMessage::Type::CONTROL_CHANGE:
            // Map common CCs to parameters
            if (msg.controller == 7) {  // Volume CC
                change.type = ParameterType::TRACK_VOLUME;
                change.value = msg.value / 127.0f;
                change.trackIndex = msg.channel;
                ParameterBus::getInstance().publish(change);
            } else if (msg.controller == 10) {  // Pan CC
                change.type = ParameterType::TRACK_PAN;
                change.value = (msg.value - 64) / 64.0f;
                change.trackIndex = msg.channel;
                ParameterBus::getInstance().publish(change);
            }
            break;

        default:
            break;
    }
}

} // namespace DrumMachine
