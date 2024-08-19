/*
  ==============================================================================

    Utilities.h
    Created: 15 Aug 2024 8:02:08am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum FFTOrder
{
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};

template<typename Attachment,
         typename APVTS,
         typename Params,
         typename ParamName,
         typename SliderType>
void makeAttachment(std::unique_ptr<Attachment>& attachment,
                    APVTS& apvts,
                    const Params& params,
                    const ParamName& name,
                    SliderType& slider)
{
    /*
     Set the SliderAttachment between the input parameter and input slider
     
     Inputs:
     - attachment: reference to pointer unique_ptr<Attachment>
     - apvts: reference to the apvts
     - params: reference to the params map declared in PluginProcessor.h
     - name: reference to the name of the parameter
     - slider: reference to a Slider object
     Outputs:
     - No return
     - Creates an attachment between the slider and the parameter
     */
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}

template<typename APVTS,
        typename Params,
        typename Name>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    /*
     Returns a reference to the correct parameter based on the input name
     
     Inputs:
     - apvts: reference to the apvts
     - params: reference to the params map declared in PluginProcessor.h
     - name: reference to the name of the parameter
     Outputs:
     - *param: a reference to the correct parameter
     */
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);
    
    return *param;
}

template<typename T>
bool truncateKiloValue(T& value)
{
    if(value > static_cast<T>(999))
    {
        value /= static_cast<T>(1000);
        return true;
    }
    
    return false;
}

juce::String getValString(const juce::RangedAudioParameter& param,
                          bool getLow,
                          juce::String suffix);

template<typename Labels,
         typename ParamType,
         typename SuffixType>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    /*
     Add the pair of labels that is at the left and right corners of a rotary slider
     
     Inputs:
     - labels: a reference to juce::Array<LabelPos> that is the specified label object
     - param: reference to the specified parameter
     - suffix: a reference to the specified parameter's unit suffix
     Outputs:
     - None
     - Edits the juce::Array under the passed in labels
     */
    // The "labels" are "juce::Array<LabelPos>" objects declared in the RotarySliderWithLabel struct for each slider
    // For "labels.add({x, y});" {x, y} make up a LabelPos object. x is the pos, y is the actual label
    // The 0.f and 1.f pos variales are used to set the labels to the left and right corners. More details in RotarySliderWithLabels::paint function
    labels.clear();
    labels.add({0.f, getValString(param, true, suffix)});
    labels.add({1.f, getValString(param, false, suffix)});
}

juce::Rectangle<int> drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds);
