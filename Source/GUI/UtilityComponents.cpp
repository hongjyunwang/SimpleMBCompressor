/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 15 Aug 2024 7:57:30am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#include "UtilityComponents.h"

Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

void Placeholder::paint(juce::Graphics& g)
{
    g.fillAll(customColor);
}

RotarySlider::RotarySlider() :
juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
{ }
