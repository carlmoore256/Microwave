#include "PluginProcessor.h"
#include "PluginEditor.h"


//================================================================
MicrowaveAudioProcessor::MicrowaveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MicrowaveAudioProcessor::~MicrowaveAudioProcessor()
{
	for(InterfaceSound* i_s : interfaceSounds)
		delete i_s;
	
	delete microwaveSound;
}

//===============================================================
const juce::String MicrowaveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MicrowaveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MicrowaveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MicrowaveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MicrowaveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MicrowaveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MicrowaveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MicrowaveAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MicrowaveAudioProcessor::getProgramName (int index)
{
    return {};
}

void MicrowaveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//===============================================================
void MicrowaveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	int blockSize = AudioProcessor::getBlockSize();
	beepShort = new InterfaceSound("beep_clip_wav", blockSize);
	beepError = new InterfaceSound("error_beep_wav", blockSize);
	beepLong = new InterfaceSound("beep_long_wav", blockSize);
	beepFinished = new InterfaceSound("beep_finished_wav", blockSize);
	openDoor = new InterfaceSound("door_open_wav", blockSize);
	closeDoor = new InterfaceSound("door_close_wav", blockSize);
	
	interfaceSounds.add(beepShort);
	interfaceSounds.add(beepError);
	interfaceSounds.add(beepLong);
	interfaceSounds.add(beepFinished);
	interfaceSounds.add(openDoor);
	interfaceSounds.add(closeDoor);
	
	microwaveSound = new OperationSound(m_granularWindowSize, m_granularHopSize);
	
//	popcornProcessor = new PopcornProcessor(blockSize, AudioProcessor::getTotalNumOutputChannels());
}

void MicrowaveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MicrowaveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MicrowaveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	processInterfaceSounds(buffer);
	
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

//		buffer.addFrom
//		for(int i = 0; i < buffer.getNumSamples(); i++)
//		{
//			int chantoread = channel % beepBuffer.getNumChannels();
//
//			buffer.setSample(channel, i, 0.3 * beepBuffer.getSample(chantoread, m_filePos));
//		}
		
//		processInterfaceSounds(channelData);
		
//		buffer.addFrom(channel, 0, beepBuffer, channel % beepBuffer.getNumChannels(), m_filePos, buffer.getNumSamples());
		
		if (m_filePos++ >= beepBuffer.getNumSamples())
			m_filePos = 0;
        // ..do something to the data...
    }
}

void MicrowaveAudioProcessor::processInterfaceSounds(AudioBuffer<float>& buffer)
{
	for(InterfaceSound* interfaceSound : interfaceSounds)
		interfaceSound -> writeNextBuffer(buffer);
	
	microwaveSound -> writeNextBuffer(buffer);
	
//	popcornProcessor -> processBlock(buffer);
}

//void MicrowaveAudioProcessor::popcorn(AudioBuffer<float>& buffer)
//{
//
//}

//================================================================
bool MicrowaveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MicrowaveAudioProcessor::createEditor()
{
    return new MicrowaveAudioProcessorEditor (*this);
}

//===============================================================
void MicrowaveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MicrowaveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//===============================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MicrowaveAudioProcessor();
}
