/*
  ==============================================================================

    UtilityComponents.h
    Created: 15 Aug 2024 7:57:30am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Placeholder : juce::Component
{
    //Constructor
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider();
};

