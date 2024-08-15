/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/CompressorBand.h"

/*
 DSP Roadmap
 1) Figure out how to split the audio into 3 bands (v)
 2) Create parameters to control where the split happens (v)
 3) Prove that splitting into 3 bands produces no audible artifacts (v)
 4) Create audio parameters for the 3 compressor bands. These need to live on each band instance (v)
 5) Create the 2 remaining compressors (v)
 6) Add ability to mute/solo/bypass individual compressors (v)
 7) Add input and output gain to offset changes in output level (v)
 8) Clean up
 */

//==============================================================================
/**
*/
class SimpleMBCompAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleMBCompAudioProcessor();
    ~SimpleMBCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    
    // A static member function belongs to the class itself rather than to any particular object of the class. This means it can be called without creating an instance of the class.
    static APVTS::ParameterLayout createParameterLayout();
    
    // Initialize tree state apvts with all the necessary parameters added here
    APVTS apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:
    
    // Array of CompressorBand objects
    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];
    
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    // Since filters are constructed through delays, we need to make sure the timing of all bands are the same
    // The specific scheme is described in the tutorial. It is also roughly shown here visually
    //     fc0  fc1
    Filter LP1, AP2,
           HP1, LP2,
                HP2;
    
    juce::AudioParameterFloat* lowMidCrossover {nullptr};
    juce::AudioParameterFloat* midHighCrossover {nullptr};
    
    // We need to create three separate audio buffers for the each band to correctly handle the multi-band processing
    // We store the buffers here in the array filterBuffers
    // Array of AudioBuffer<float> objects
    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam {nullptr};
    juce::AudioParameterFloat* outputGainParam {nullptr};
    
    template<typename T, typename U>
    void applyGain(T& buffer, U& gain){
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
    
    void updateState(); 
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
