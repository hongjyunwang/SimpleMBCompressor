/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 15 Aug 2024 8:08:02am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#include "CompressorBandControls.h"
#include "../DSP/Params.h"
#include "Utilities.h"

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
apvts(apv),
attackSlider(nullptr, "ms", "ATTACK"),
releaseSlider(nullptr, "ms", "RELEASE"),
thresholdSlider(nullptr, "dB", "THRESHOLD"),
ratioSlider(nullptr, "")
{
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    
    bypassButton.addListener(this);
    soloButton.addListener(this);
    muteButton.addListener(this);
    
    bypassButton.setName("X");
    soloButton.setName("S");
    muteButton.setName("M");
    
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);
    
    lowBand.setName("Low");
    midBand.setName("Mid");
    highBand.setName("High");
    
    // The RadioGroupId allows for the functionaltiy that only one band will be selected at once
    lowBand.setRadioGroupId(1);
    midBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);
    
    // Create a lambda function for switching between bands
    // buttonSwitcher is a callable object (specifically, a lambda function) that doesn't return a value. When assigned to onClick, it means that clicking the button will execute the lambda function.
    // Hooking up the parameters to attachments is done in the updateAttachments function
    auto buttonSwitcher = [safePtr = this -> safePtr]()
    {
        // Checks if safePtr is valid
        if(auto* c = safePtr.getComponent())
        {
            c -> updateAttachments();
        }
    };
    
    // Assign the lambda function to the button click event handlers
    // More simply put, we define what the button does when clicked through the lambda function
    lowBand.onClick = buttonSwitcher;
    midBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;
    
    // We don't send a notification because that would trigger the lambda function
    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);
    
    updateAttachments();
    
    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);
}

CompressorBandControls::~CompressorBandControls()
{
    bypassButton.removeListener(this);
    soloButton.removeListener(this);
    muteButton.removeListener(this);
}

void CompressorBandControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;
    
    // Create a flexbox to wrap the vertical arrangement of buttons
    auto createBandButtonControlBox = [](std::vector<Component*> comps)
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;
        
        auto spacer = FlexItem().withHeight(2);
        
        for(auto* comp : comps)
        {
            flexBox.items.add(spacer);
            flexBox.items.add(FlexItem(*comp).withFlex(1.f));
        }
        flexBox.items.add(spacer);
        
        return flexBox;
    };
    
    auto bandButtonControlBox = createBandButtonControlBox({&bypassButton, &soloButton, &muteButton});
    auto bandSelectControlBox = createBandButtonControlBox({&lowBand, &midBand, &highBand});

    
    // A flexbox is a GUI object that is essentially a big box containing smaller boxes
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4);
    
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(50));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30));
    
    flexBox.performLayout(bounds);
}

void CompressorBandControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void CompressorBandControls::buttonClicked(juce::Button *button)
{
    // Update Slider Enablement
    // If the band is muted or bypassed, the sliders should be disabled
    updateSliderEnablements();
    // In this function we control the enablement of the bypass/mute/solo buttons based on which on is clicked
    updateSoloMuteBypassToggleStates(*button);
}

void CompressorBandControls::updateSliderEnablements()
{
    auto disabeled = muteButton.getToggleState() || bypassButton.getToggleState();
    attackSlider.setEnabled(!disabeled);
    releaseSlider.setEnabled(!disabeled);
    thresholdSlider.setEnabled(!disabeled);
    ratioSlider.setEnabled(!disabeled);
}

void CompressorBandControls::updateSoloMuteBypassToggleStates(juce::Button &clickedButton)
{
    // Only one button can be activated at a time
    // Note that we send notifications here to update the parameter
    if(&clickedButton == &soloButton && soloButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    if(&clickedButton == &muteButton && muteButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    if(&clickedButton == &bypassButton && bypassButton.getToggleState())
    {
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
}

void CompressorBandControls::updateAttachments()
{
    // This function (1) figures out which button was clicked (2) figures out which parameters go with which buttons (3) Create parameters like we did in the constructor
    enum BandType
    {
        Low,
        Mid,
        High
    };
    
    // This lambda function is immediately called, setting variable bandType to the correct index from BandType enum
    BandType bandType = [this]()
    {
        if(lowBand.getToggleState())
            return BandType::Low;
        if(midBand.getToggleState())
            return BandType::Mid;
        return BandType::High;
    }();
    
    using namespace Params;
    std::vector<Names> names;
    
    // Depending on variable bandType, the switch statement creates a vector "names" storing the index of the intended parameters from Names enumerator
    switch (bandType)
    {
        case Low:
        {
            names = std::vector<Names>{
                Names::Attack_Low_Band,
                Names::Release_Low_Band,
                Names::Threshold_Low_Band,
                Names::Ratio_Low_Band,
                Names::Mute_Low_Band,
                Names::Solo_Low_Band,
                Names::Bypassed_Low_Band
            };
            break;
        }
        case Mid:
        {
            names = std::vector<Names>{
                Names::Attack_Mid_Band,
                Names::Release_Mid_Band,
                Names::Threshold_Mid_Band,
                Names::Ratio_Mid_Band,
                Names::Mute_Mid_Band,
                Names::Solo_Mid_Band,
                Names::Bypassed_Mid_Band
            };
            break;
        }
        case High:
        {
            names = std::vector<Names>{
                Names::Attack_High_Band,
                Names::Release_High_Band,
                Names::Threshold_High_Band,
                Names::Ratio_High_Band,
                Names::Mute_High_Band,
                Names::Solo_High_Band,
                Names::Bypassed_High_Band
            };
            break;
        }
    }
    
    // Enumerate the positions of each control
    enum Position
    {
        Attack,
        Release,
        Threshold,
        Ratio,
        Mute,
        Solo,
        Bypass
    };
    
    std::cout << "Test Message";
            
    // Retrieve the map params from namespace Params
    const auto& params = GetParams();
    
    auto getParamHelper = [&params, &apvts = this -> apvts, &names](const auto& pos) -> auto&
    {
        /*
         This is essentially a wrapper around the function "getParam"
         
         Inputs:
         - pos: index in vector names corresponding to the Names enumeration (from namspace Params) of the intended parameter
         Outputs:
         - Returns a reference to the parameter itself
         */
        return getParam(apvts, params, names.at(pos));
    };
    
    // Reset the attachments before we begin the update
    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    bypassButtonAttachment.reset();
    soloButtonAttachment.reset();
    muteButtonAttachment.reset();
        
    // Begin the update
    // (1) Retrieve the correct parameter
    // (2) Add the labels on the left and right corners of the slider
    // (3) Update parameter
    auto& attackParam = getParamHelper(Position::Attack);
    addLabelPairs(attackSlider.labels, attackParam, "ms");
    attackSlider.changeParam(&attackParam);
    
    auto& releaseParam = getParamHelper(Position::Release);
    addLabelPairs(releaseSlider.labels, releaseParam, "ms");
    releaseSlider.changeParam(&releaseParam);
    
    auto& thresholdParam = getParamHelper(Position::Attack);
    addLabelPairs(thresholdSlider.labels, thresholdParam, "dB");
    thresholdSlider.changeParam(&thresholdParam);
    
    auto& ratioParamRap = getParamHelper(Position::Ratio);
    ratioSlider.labels.clear();
    ratioSlider.labels.add({0.f, "1:1"});
    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&ratioParamRap);
    ratioSlider.labels.add({1.f,
        juce::String(ratioParam -> choices.getReference(ratioParam -> choices.size() - 1).getIntValue()) + ":1"});
    ratioSlider.changeParam(ratioParam);

            
    // Make slider attachments to the apvts
    auto makeAttachmentHelper = [&params, &apvts = this -> apvts](auto& attachment, const auto& name, auto& slider)
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
    
    makeAttachmentHelper(attackSliderAttachment, names[Position::Attack], attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, names[Position::Release], releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, names[Position::Threshold], thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, names[Position::Ratio], ratioSlider);
    makeAttachmentHelper(bypassButtonAttachment, names[Position::Bypass], bypassButton);
    makeAttachmentHelper(muteButtonAttachment, names[Position::Mute], muteButton);
    makeAttachmentHelper(soloButtonAttachment, names[Position::Solo], soloButton);
}
