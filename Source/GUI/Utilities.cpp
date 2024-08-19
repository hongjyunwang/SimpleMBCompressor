/*
  ==============================================================================

    Utilities.cpp
    Created: 15 Aug 2024 8:02:08am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#include "Utilities.h"


juce::String getValString(const juce::RangedAudioParameter& param,
                          bool getLow,
                          juce::String suffix)
{
    /*
     Get the value string used for the labels on the left and right corners of a rotary slider
     
     Inputs:
     - param: A reference to the parameter of the rotary slider
     - getLow: A boolean value that defines whether we are setting the low value or not
     - suffix: A juce::String that is the parameter's suffix
     Outputs:
     - str: The desired juce::String label
     */
    juce::String str;
    
    auto val = getLow ? param.getNormalisableRange().start :
                        param.getNormalisableRange().end;
    
    // If val exceeds 1000, add a k before the suffix on the output str
    bool useK = truncateKiloValue(val);
    str << val;
    if(useK)
        str << "k";
    str << suffix;
    
    return str;
}

juce::Rectangle<int> drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    g.setColour(Colours::blueviolet);
    g.fillAll();
    
    auto localBounds = bounds;
    
    bounds.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);
    
    g.drawRect(localBounds);
    
    return bounds;
}
