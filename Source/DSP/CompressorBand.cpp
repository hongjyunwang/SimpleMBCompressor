/*
  ==============================================================================

    CompressorBand.cpp
    Created: 15 Aug 2024 8:23:37am
    Author:  Hong Jyun Wang

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(attack -> get());
    compressor.setRelease(release -> get());
    compressor.setThreshold(threshold -> get());
    // Use a helper function to get the ratio value from the choicebox string array
    compressor.setRatio(ratio -> getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    auto preRMS = computeRMSLevel(buffer);
    
    // The compressor needs a context to process audio and the context needs an audio block to be constructed
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    context.isBypassed = bypassed -> get();
    compressor.process(context);
    
    auto postRMS = computeRMSLevel(buffer);
    
    auto convertToDb = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };
    
    rmsInputLevelDb.store(convertToDb(preRMS));
    rmsOutputLevelDb.store(convertToDb(postRMS));
}
