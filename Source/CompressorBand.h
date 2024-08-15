/*
  ==============================================================================

    CompressorBand.h
    Created: 15 Aug 2024 8:23:37am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct CompressorBand
{
    
    // Note that the "compressor" instance used in this struct's member functions is the private member that is actually a juce::dsp::Compressor<float> object
    // The "compressor" instance used outside of this struct are all instances of the CompressorBand struct
    
    // Create caching pointers to the audio parameters
    juce::AudioParameterFloat* attack {nullptr};
    juce::AudioParameterFloat* release {nullptr};
    juce::AudioParameterFloat* threshold {nullptr};
    juce::AudioParameterChoice* ratio {nullptr};
    juce::AudioParameterBool* bypassed {nullptr};
    juce::AudioParameterBool* mute {nullptr};
    juce::AudioParameterBool* solo {nullptr};
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void updateCompressorSettings();
    void process(juce::AudioBuffer<float>& buffer);
    
private:
    juce::dsp::Compressor<float> compressor;
};
