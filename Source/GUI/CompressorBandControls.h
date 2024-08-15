/*
  ==============================================================================

    CompressorBandControls.h
    Created: 15 Aug 2024 8:08:02am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~ CompressorBandControls() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
private:
    juce::AudioProcessorValueTreeState& apvts;
    
    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
    RatioSlider ratioSlider; 
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
                                releaseSliderAttachment,
                                thresholdSliderAttachment,
                                ratioSliderAttachment;
    
    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;
    
    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment,
                                soloButtonAttachment,
                                muteButtonAttachment;
    
    juce::Component::SafePointer<CompressorBandControls> safePtr { this };
    
    void updateAttachments();
    void updateSliderEnablements();
    void updateSoloMuteBypassToggleStates(juce::Button& clickedButton);

};
