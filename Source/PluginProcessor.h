/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
 DSP Roadmap
 1) Figure out how to split the audio into 3 bands (v)
 2) Create parameters to control where the split happens (v)
 3) Prove that splitting into 3 bands produces no audible artifacts (v)
 4) Create audio parameters for the 3 compressor bands. These need to live on each band instance (v)
 5) Create the 2 remaining compressors (v)
 6) Add ability to mute/solo/bypass individual compressors (v)
 7) Add input and output gain to offset changes in output level
 8) Clean up
 */

namespace Params
{

// Names is an enumerator. For example, we access a "Low Mid Crossover Freq"'s number through "Names::Low_Mid_Crossover_Freq"
// params is a map that maps the enumerated parameter to a string that is the parameter ID

enum Names
{
    Low_Mid_Crossover_Freq,
    Mid_High_Crossover_Freq,
    
    Threshold_Low_Band,
    Threshold_Mid_Band,
    Threshold_High_Band,
    
    Attack_Low_Band,
    Attack_Mid_Band,
    Attack_High_Band,
    
    Release_Low_Band,
    Release_Mid_Band,
    Release_High_Band,
    
    Ratio_Low_Band,
    Ratio_Mid_Band,
    Ratio_High_Band,
    
    Bypassed_Low_Band,
    Bypassed_Mid_Band,
    Bypassed_High_Band,
    
    Mute_Low_Band,
    Mute_Mid_Band,
    Mute_High_Band,
    
    Solo_Low_Band,
    Solo_Mid_Band,
    Solo_High_Band,
};

inline const std::map<Names, juce::String>& GetParams()
{
    static std::map<Names, juce::String> params =
    {
        {Low_Mid_Crossover_Freq, "Low Mid Crossover Freq"},
        {Mid_High_Crossover_Freq, "Mid High Crossover Freq"},
        {Threshold_Low_Band,"Threshold Low Band"},
        {Threshold_Mid_Band, "Threshold Mid Band"},
        {Threshold_High_Band, "Threshold High Band"},
        {Attack_Low_Band, "Attack Low Band"},
        {Attack_Mid_Band, "Attack Mid Band"},
        {Attack_High_Band, "Attack High Band"},
        {Release_Low_Band, "Release Low Band"},
        {Release_Mid_Band, "Release Mid Band"},
        {Release_High_Band, "Release High Band"},
        {Ratio_Low_Band, "Ratio Low Band"},
        {Ratio_Mid_Band, "Ratio Mid Band"},
        {Ratio_High_Band, "Ratio High Band"},
        {Bypassed_Low_Band, "Bypassed Low Band"},
        {Bypassed_Mid_Band, "Bypassed Mid Band"},
        {Bypassed_High_Band, "Bypassed High Band"},
        
        {Mute_Low_Band, "Mute Low Band"},
        {Mute_Mid_Band, "Mute Mid Band"},
        {Mute_High_Band, "Mute High Band"},
        
        {Solo_Low_Band, "Solo Low Band"},
        {Solo_Mid_Band, "Solo Mid Band"},
        {Solo_High_Band, "Solo High Band"},
    };
    
    return params;
}
}

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
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};