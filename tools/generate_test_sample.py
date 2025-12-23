#!/usr/bin/env python3
"""
Generate a simple test WAV sample for Milestone 1.
Creates a 100ms kick drum-like sound at 44.1 kHz.
"""

import struct
import math

def generate_kick_sample(filename, duration_ms=100, sample_rate=44100):
    """Generate a simple kick drum sample."""
    
    num_samples = int(sample_rate * duration_ms / 1000.0)
    samples = []
    
    # Simple kick: sine wave sweeping down in frequency with exponential decay
    freq_start = 150.0  # Hz
    freq_end = 50.0     # Hz
    
    for i in range(num_samples):
        # Exponential decay envelope
        t = i / sample_rate
        envelope = math.exp(-5.0 * t)  # Decay time constant
        
        # Frequency sweep (linear in time)
        progress = i / num_samples
        freq = freq_start + (freq_end - freq_start) * progress
        
        # Generate sine wave
        phase = 2.0 * math.pi * freq * t
        sample = math.sin(phase) * envelope * 0.8  # 0.8 = volume
        
        samples.append(sample)
    
    # Write WAV file (PCM 16-bit mono)
    with open(filename, 'wb') as f:
        # WAV header
        num_channels = 1
        bits_per_sample = 16
        byte_rate = sample_rate * num_channels * bits_per_sample // 8
        block_align = num_channels * bits_per_sample // 8
        
        # RIFF header
        f.write(b'RIFF')
        f.write(struct.pack('<I', 36 + num_samples * block_align))
        f.write(b'WAVE')
        
        # fmt sub-chunk
        f.write(b'fmt ')
        f.write(struct.pack('<I', 16))  # Subchunk1Size
        f.write(struct.pack('<H', 1))   # AudioFormat (1 = PCM)
        f.write(struct.pack('<H', num_channels))
        f.write(struct.pack('<I', sample_rate))
        f.write(struct.pack('<I', byte_rate))
        f.write(struct.pack('<H', block_align))
        f.write(struct.pack('<H', bits_per_sample))
        
        # data sub-chunk
        f.write(b'data')
        f.write(struct.pack('<I', num_samples * block_align))
        
        # Write audio data (convert float to int16)
        for sample in samples:
            # Clamp to [-1.0, 1.0]
            sample = max(-1.0, min(1.0, sample))
            # Convert to 16-bit int
            int_sample = int(sample * 32767)
            f.write(struct.pack('<h', int_sample))
    
    print(f"Generated {filename}")
    print(f"  Duration: {duration_ms} ms")
    print(f"  Sample rate: {sample_rate} Hz")
    print(f"  Samples: {num_samples}")

if __name__ == '__main__':
    generate_kick_sample('assets/samples/test_kick.wav', duration_ms=100)
    print("\nTo use this in the audio engine, set the file path in main.cpp")
