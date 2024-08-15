/*
  ==============================================================================

    CustomButtons.h
    Created: 15 Aug 2024 7:53:38am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;
    
    juce::Path randomPath;
};

