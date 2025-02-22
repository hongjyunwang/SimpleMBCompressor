/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

/*
GUI Roadmap:
 1) Global Controls (x-over sliders, gain sliders) (v)
 2) Main Band Controls (attack, release, threshold, ratio) (v)
 3) Add solo/mute/bypass buttons (v)
 4) Band Select Functionality (v)
 5) Band Select Buttons reflect the solo/mute/bypass state (v)
 6) Custom Look and Feel for Sliders and Toggle Buttons (adapt from Simple EQ) (v)
 7) Spectrum Analyzer Overview (v)
 8) Data structures for spectrum analyzer (adapt from Simple EQ) (v)
 9) Fifo usage in pluginProcessor::procesBlock (v)
 10) implementation of the analyzer rendering pre-computed paths (adapt from Simple EQ) (v)
 11) drawing crossover on top of the analyzer plot (v)
 12) Drawing gain reduction on top of the analyzer (v)
 13) Analyzer Bypass (adapt from Simple EQ) (v)
 14) Global Bypass button (v)
 */

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/GlobalControls.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/UtilityComponents.h"
#include "GUI/SpectrumAnalyzer.h"
#include "GUI/CustomButtons.h"

struct ControlBar : juce::Component
{
    ControlBar();
    void resized() override;
    
    AnalyzerButton analyzerbutton;
    PowerButton globalBypassButton;
};

//==============================================================================

class SimpleMBCompAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor&);
    ~SimpleMBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
private:
    // Declare and initialize LookAndFeel under the editor class so it falls under this parent component
    LookAndFeel lnf;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleMBCompAudioProcessor& audioProcessor;
    
    ControlBar controlBar;
    GlobalControls globalControls { audioProcessor.apvts };
    CompressorBandControls bandControls { audioProcessor.apvts };
    SpectrumAnalyzer analyzer { audioProcessor };
    
    void toggleGlobalBypassState();
    std::array<juce::AudioParameterBool*, 3> getBypassParams();
    
    void updateGlobalBypassButton();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessorEditor)
};
