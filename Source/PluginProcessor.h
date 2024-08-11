/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
 DSP Roadmap
 1) Figure out how to split the audio into 3 bands
 2) Create parameters to control where the split happens
 3) Prove that splitting into 3 bands produces no audible artifacts
 4) Create audio parameters for the 3 compressor bands. These need to live on each band instance
 5) Create the 2 remaining compressors
 6) Add ability to mute/solo/bypass individual compressors
 7) Add input and output gain to offset changes in output level
 8) Clean up
 */

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
    
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }
    
    void updateCompressorSettings()
    {
        compressor.setAttack(attack -> get());
        compressor.setRelease(release -> get());
        compressor.setThreshold(threshold -> get());
        // Use a helper function to get the ratio value from the choicebox string array
        compressor.setRatio(ratio -> getCurrentChoiceName().getFloatValue());
    }
    
    void process(juce::AudioBuffer<float>& buffer)
    {
        // The compressor needs a context to process audio and the context needs an audio block to be constructed
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        context.isBypassed = bypassed -> get();
        compressor.process(context);
    }
private:
    juce::dsp::Compressor<float> compressor;
};

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
    
    CompressorBand compressor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
