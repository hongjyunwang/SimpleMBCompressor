/*
  ==============================================================================

    CompressorBand.h
    Created: 15 Aug 2024 8:23:37am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../GUI/Utilities.h"

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
    
    float getRMSInputLevelDb() const {return rmsInputLevelDb;};
    float getRMSOutputLevelDb() const {return rmsOutputLevelDb;};

private:
    juce::dsp::Compressor<float> compressor;
    
    std::atomic<float> rmsInputLevelDb {NEGATIVE_INFINITY};
    std::atomic<float> rmsOutputLevelDb {NEGATIVE_INFINITY};

    template<typename T>
    float computeRMSLevel(const T& buffer)
    {
        int numChannels = static_cast<int>(buffer.getNumChannels());
        int numSamples = static_cast<int>(buffer.getNumSamples());
        auto rms = 0.f;
        for(int chan = 0; chan < numChannels; ++chan)
        {
            rms += buffer.getRMSLevel(chan, 0, numSamples);
        }
        
        rms /= static_cast<float>(numChannels);
        return rms;
    }
    
};
