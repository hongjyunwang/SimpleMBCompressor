/*
  ==============================================================================

    GlobalControls.h
    Created: 15 Aug 2024 8:12:18am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Declare RotarySliderWithLabels for each slider
    // Note we declare pointers here
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;
    
    // Declare SliderAttachments
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> inGainSliderAttachment,
                                lowMidXoverSliderAttachment,
                                midHighXoverSliderAttachment,
                                outGainSliderAttachment;
};
