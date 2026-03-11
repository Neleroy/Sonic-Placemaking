// Simple Mozzi Resonant Filter for Arduino Diecimila (ATmega168)
// Demonstrates a basic resonant low-pass filter sweeping cutoff frequency via potentiometer.
// Uses Mozzi's audio engine at reduced sample rate for compatibility.
// Requires Mozzi library installed (test with minimal sketch first).
// If compilation fails, stick with standalone code due to ATmega168 limitations.

#include <MozziGuts.h>        // Mozzi core
#include <ResonantFilter.h>   // Resonant filter

// Mozzi setup (reduced rate for ATmega168)
#define CONTROL_RATE 64

// Resonant filter (simple low-pass)
ResonantFilter<LOWPASS> filter;

// Potentiometer pin
#define POT_PIN A0

// Sawtooth oscillator simulation (simple phase accumulator)
uint8_t oscPhase = 0;

void setup() {
  pinMode(POT_PIN, INPUT);
  // Start Mozzi at lower sample rate for Diecimila (may default to 8kHz)
  startMozzi(CONTROL_RATE);
  filter.setResonance(200);  // Higher Q for exaggerated vowel resonance
  filter.setCutoffFreq(150); // Initial cutoff in vocal range
}

void updateControl() {
  // Read pot and map to cutoff frequency (expanded range for more dramatic sweep)
  int potValue = mozziAnalogRead(POT_PIN);
  int cutoff = map(potValue, 0, 1023, 80, 1200); // 80-1200 Hz (full vowel formant spectrum)
  filter.setCutoffFreq(cutoff);
  filter.setResonance(200 + potValue / 5); // Vary Q dynamically for emphasis
}

int updateAudio() {
  // Enriched sawtooth source with harmonics for vowel-like timbre
  oscPhase += 2;  // ~200 Hz at 8kHz
  int fundamental = (oscPhase < 128) ? oscPhase * 2 - 128 : (255 - oscPhase) * 2 - 128;
  int harmonic = ((oscPhase * 3) % 256 < 128) ? (oscPhase * 3 % 256) * 2 - 128 : (255 - oscPhase * 3 % 256) * 2 - 128; // 3rd harmonic
  int source = fundamental + harmonic / 3; // Mix for richer sound

  // Apply filter with higher gain for exaggerated effect
  int filtered = filter.next(source) * 1.5; // Boost resonance
  return constrain(filtered, -180, 180); // Limit to prevent overdriving
}

void loop() {
  audioHook();
}