/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

/*
GUI Roadmap:
 1) Global Controls (x-over sliders, gain sliders)
 2) Main Band Controls (attack, release, threshold, ratio)
 3) Add solo/mute/bypass buttons
 4) Band Select Functionality
 5) Band Select Buttons reflect the solo/mute/bypass state
 6) Custom Look and Feel for Sliders and Toggle Buttons (adapt from Simple EQ)
 7) Spectrum Analyzer Overview
 8) Data structures for spectrum analyzer (adapt from Simple EQ)
 9) Fifo usage in pluginProcessor::procesBlock
 10) implementation of the analyzer rendering pre-computed paths (adapt from Simple EQ)
 11) drawing crossover on top of the analyzer plot
 12) Drawing gain reduction on top of the analyzer
 13) Analyzer Bypass (adapt from Simple EQ)
 14) Global Bypass button
 */

#include <JuceHeader.h>
#include "PluginProcessor.h"


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton & toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};

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

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto insetRect = bounds.reduced(4);
        
        randomPath.clear();
        
        juce::Random r;
        
        randomPath.startNewSubPath(insetRect.getX(),
                                   insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        
        for( auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2 )
        {
            randomPath.lineTo(x,
                              insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        }
    }
    
    juce::Path randomPath;
};

//==============================================================================
/**
*/
struct Placeholder : juce::Component
{
    //Constructor
    Placeholder();
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }
    
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider() :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    { }
};

struct CompressorBandControls : juce::Component
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;
    
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

class SimpleMBCompAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor&);
    ~SimpleMBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Declare and initialize LookAndFeel under the editor class so it falls under this parent component
    LookAndFeel lnf;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleMBCompAudioProcessor& audioProcessor;
    
    Placeholder controlBar, analyzer /*globalControls,*/ /*bandControls*/;
    GlobalControls globalControls { audioProcessor.apvts };
    CompressorBandControls bandControls { audioProcessor.apvts };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessorEditor)
};
