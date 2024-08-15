/*
  ==============================================================================

    RotarySliderWithLabels.h
    Created: 15 Aug 2024 7:49:01am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct RotarySliderWithLabels : juce::Slider
{
    // Constructor with initializer list
    // "juce::Slider(...)" Initializes the base class juce::Slider with specific parameters
    // setLookAndFeel is a member function of the Slider class
    RotarySliderWithLabels(juce::RangedAudioParameter* rap,
                           const juce::String& unitSuffix,
                           const juce::String& title /*= "NO TITLE"*/) :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                 juce::Slider::TextEntryBoxPosition::NoTextBox), param(rap), suffix(unitSuffix)
    {
        setName(title);
    }
    
    struct LabelPos
    {
        float pos;
        juce::String label;
    };
    
    juce::Array<LabelPos> labels;
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    // Made a virtual function so RatioSlider can override it
    virtual juce::String getDisplayString() const;
    
    void changeParam(juce::RangedAudioParameter* p);
    
protected:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct RatioSlider : RotarySliderWithLabels
{
    // This struct is made specifically for the ratio slider. It inherits from RotarySliderWithLabels
    // The RotarySliderWithLabels is initialized to the ratio parameter
    
    RatioSlider(juce::RangedAudioParameter* rap, const juce::String& unitSuffix) : RotarySliderWithLabels(rap, unitSuffix, "RATIO") {}
    
    // RatioSlider gets its own getDisplayString function because its displaystring needs to be the ratio and not a float value
    juce::String getDisplayString() const override;
};
