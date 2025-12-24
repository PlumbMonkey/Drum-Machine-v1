#!/usr/bin/env python3
"""
Generate a complete drum kit with 10 samples.
Each drum has a unique frequency signature for quick identification.
"""

import wave
import struct
import math
import os

def generate_drum_sample(frequency, duration_ms, sample_rate=44100, decay=True):
    """Generate a simple drum sample with exponential decay."""
    samples = int(sample_rate * duration_ms / 1000)
    data = []
    
    for i in range(samples):
        # Exponential decay envelope
        if decay:
            decay_factor = math.exp(-5 * i / samples)
        else:
            decay_factor = 1.0
        
        # Simple sine wave with decay
        t = i / sample_rate
        amplitude = 0.8 * decay_factor
        value = amplitude * math.sin(2 * math.pi * frequency * t)
        
        # Quantize to 16-bit
        quantized = int(max(-32768, min(32767, value * 32767)))
        data.append(quantized)
    
    return data

def save_wav(filename, samples, sample_rate=44100, channels=1):
    """Save samples as a WAV file."""
    with wave.open(filename, 'wb') as f:
        f.setnchannels(channels)
        f.setsampwidth(2)
        f.setframerate(sample_rate)
        
        for sample in samples:
            f.writeframes(struct.pack('<h', sample))

# Define drum kit
drum_kit = {
    'kick': {'freq': 60, 'duration': 150, 'decay': True},
    'snare': {'freq': 200, 'duration': 100, 'decay': True},
    'closed_hihat': {'freq': 8000, 'duration': 50, 'decay': True},
    'open_hihat': {'freq': 10000, 'duration': 200, 'decay': True},
    'tom_high': {'freq': 500, 'duration': 80, 'decay': True},
    'tom_mid': {'freq': 300, 'duration': 100, 'decay': True},
    'tom_low': {'freq': 150, 'duration': 120, 'decay': True},
    'tom_floor': {'freq': 100, 'duration': 140, 'decay': True},
    'ride': {'freq': 6000, 'duration': 300, 'decay': True},
    'crash': {'freq': 12000, 'duration': 400, 'decay': True},
}

# Create assets/samples directory if it doesn't exist
samples_dir = 'assets/samples'
os.makedirs(samples_dir, exist_ok=True)

# Generate each drum sample
print("Generating drum kit...")
for name, params in drum_kit.items():
    filename = f"{samples_dir}/{name}.wav"
    samples = generate_drum_sample(
        params['freq'],
        params['duration'],
        decay=params['decay']
    )
    save_wav(filename, samples)
    print(f"  ✓ Generated {name}.wav ({params['duration']}ms @ {params['freq']}Hz)")

print("\nDrum kit generation complete!")
print(f"All samples saved to {samples_dir}/")
