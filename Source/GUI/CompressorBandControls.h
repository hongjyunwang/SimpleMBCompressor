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
    
    void toggleAllBands(bool shouldBeBypassed);
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
    
    // ActiveBand defaults to the lowBand
    // We need to update which band is the activeBand whenever we updateAttachments
    juce::ToggleButton* activeBand = &lowBand;
    
    // Update attachments for construction
    void updateAttachments();
    
    // Disable the attack, release, threshold, and ratio sliders for the currently attached band if the mute or bypass buttons are on
    void updateSliderEnablements();
    
    // Make sure only one of the solo/mute/bypass buttons are on
    void updateSoloMuteBypassToggleStates(juce::Button& clickedButton);
    
    // These three functions update the fill color of the active band based on whether which of the solo/mute/bypass buttons is on
    void updateActiveBandFillColor(juce::Button& clickedButton);
    void resetActiveBandColors();
    void refreshBandButtonColors(juce::Button &band, juce::Button &colorSource);
    
    // This function is only called during construction for the band select buttons to show the correct colors of the band buttons when GUI is initially loaded
    void updateBandSelectButtonStates();
};
