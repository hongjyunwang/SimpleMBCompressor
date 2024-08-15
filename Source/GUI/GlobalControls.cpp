/*
  ==============================================================================

    GlobalControls.cpp
    Created: 15 Aug 2024 8:12:18am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#include "GlobalControls.h"
#include "../DSP/Params.h"
#include "Utilities.h"

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    // Retrieve the parameter map we declared in PluginProcessor.h
    using namespace Params;
    const auto& params = GetParams();
    
    // Initialize pointers to the parameters
    auto getParamHelper = [&params, &apvts](const auto& name) -> auto&
    {
        /*
         This is essentially a wrapper around the function "getParam"
         
         Inputs:
         - name: reference to the name of the parameter
         Outputs:
         - Returns a reference to the parameter itself
         */
        
        return getParam(apvts, params, name);
    };
    
    auto& gainInParam = getParamHelper(Names::Gain_In);
    auto& lowMidParam = getParamHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& gainOutParam = getParamHelper(Names::Gain_Out);
    
    inGainSlider = std::make_unique<RSWL>(&gainInParam, "dB", "INPUT GAIN");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidParam, "Hz", "LOW-MID X-OVER");
    midHighXoverSlider = std::make_unique<RSWL>(&midHighParam, "Hz", "MID-HI X-OVER");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam, "dB", "OUTPUT GAIN");
    
    // Make slider attachments to the apvts
    auto makeAttachmentHelper = [&params, &apvts](auto& attachment, const auto& name, auto& slider)
    {
        /*
         This is essentially a wrapper around the function "makeAttachment"
         
         Inputs:
         - attachment: reference to pointer unique_ptr<Attachment>
         - name: reference to the name of the parameter
         - slider: reference to a Slider object
         Outputs:
         - No return
         - Creates an attachment between the slider and the parameter
         */
        makeAttachment(attachment, apvts, params, name, slider);
    };
    makeAttachmentHelper(inGainSliderAttachment,
                         Names::Gain_In,
                         *inGainSlider);
    makeAttachmentHelper(lowMidXoverSliderAttachment,
                         Names::Low_Mid_Crossover_Freq,
                         *lowMidXoverSlider);
    makeAttachmentHelper(midHighXoverSliderAttachment,
                         Names::Mid_High_Crossover_Freq,
                         *midHighXoverSlider);
    makeAttachmentHelper(outGainSliderAttachment,
                         Names::Gain_Out,
                         *outGainSlider);
    
    // Add the labels on the left and right corners of the slider
    addLabelPairs(inGainSlider -> labels, gainInParam, "dB");
    addLabelPairs(lowMidXoverSlider -> labels, lowMidParam, "Hz");
    addLabelPairs(midHighXoverSlider -> labels, midHighParam, "Hz");
    addLabelPairs(outGainSlider -> labels, gainOutParam, "dB");
    
    // Add and make visible sliders (note that they need to be dereferenced)
    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);
}

void GlobalControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;
    
    // A flexbox is a GUI object that is essentially a big box containing smaller boxes
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);
    
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);
    
    flexBox.performLayout(bounds);
}
