/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

/*
GUI Roadmap:
 1) Global Controls (x-over sliders, gain sliders)
 2) Main Band Controls (attack, release, threshold, ratio)
 3) Add solo/mute/bypass buttons
 4) Band Select Functionality
 5) Band Select Buttons reflect the solo/mute/bypass state
 6) Custom Look and Feel for Sliders and Toggle Buttons (adapt from Simple EQ)
 7) Spectrum Analyzer Overview
 8) Data structures for spectrum analyzer (adapt from Simple EQ)
 9) Fifo usage in pluginProcessor::procesBlock
 10) implementation of the analyzer rendering pre-computed paths (adapt from Simple EQ)
 11) drawing crossover on top of the analyzer plot
 12) Drawing gain reduction on top of the analyzer
 13) Analyzer Bypass (adapt from Simple EQ)
 14) Global Bypass button
 */

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
struct Placeholder : juce::Component
{
    //Constructor
    Placeholder();
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }
    
    juce::Colour customColor;
};

struct GlobalControls : juce::Component
{
    void paint(juce::Graphics& g) override;
};

class SimpleMBCompAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor&);
    ~SimpleMBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleMBCompAudioProcessor& audioProcessor;
    
    Placeholder controlBar, analyzer, /*globalControls,*/ bandControls;
    GlobalControls globalControls;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessorEditor)
};
