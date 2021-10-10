//
//  OlaBuffer.cpp
//  Microwave
//
//  Created by Carl Moore on 8/19/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#include "OlaBuffer.hpp"

OlaBuffer::OlaBuffer(int ringBuffSize, int overlap, int channels) : m_ringBuffSize(ringBuffSize), m_overlap(overlap), m_channels(channels), m_window(ringBuffSize, dsp::WindowingFunction<float>::hann)
{
	m_hop = m_ringBuffSize / float(m_overlap);
	
	for(int channel = 0; channel < channels; channel++)
		overlapBuffers.add(new AudioBuffer<float>(overlap, ringBuffSize));
	

	processFlags.resize(overlap);
	processFlags.fill(false);
	
	m_ringBuffWriteIdx = 0;
	m_overlapBuffTargetChannel = 0;
}

OlaBuffer::~OlaBuffer()
{
	for(AudioBuffer<float>* buffer : overlapBuffers)
		delete buffer;
}

void OlaBuffer::reset()
{
	for(AudioBuffer<float>* buffer : overlapBuffers)
		buffer -> clear();
	
	ringBuffer.clear();
	
	m_ringBuffWriteIdx = 0;
	m_overlapBuffTargetChannel = 0;
}

void OlaBuffer::fillRingBuf(AudioBuffer<float>& inBuffer)
{
	int n_samples = inBuffer.getNumSamples();
	int n_chan = inBuffer.getNumChannels();
	
	for(int channel = 0; channel < n_chan; channel++)
	{
		ringBuffer.copyFrom(channel, m_ringBuffWriteIdx, inBuffer, channel, 0, n_samples);
	}
}

void OlaBuffer::fillOverlapBuffer()
{
	if(m_ringBuffWriteIdx % m_hop == 0)
	{
		for(int channel = 0; channel < m_channels; channel++)
		{
			overlapBuffers[channel] -> copyFrom(m_overlapBuffTargetChannel, 0, ringBuffer, channel, m_ringBuffWriteIdx, m_ringBuffSize-m_ringBuffWriteIdx);
			overlapBuffers[channel] -> copyFrom(m_overlapBuffTargetChannel, m_ringBuffSize-m_ringBuffWriteIdx, ringBuffer, channel, 0, m_ringBuffWriteIdx);
		}
	}
	
	// change flag to indicate overlap channel is ready for processing
	processFlags.set(m_overlapBuffTargetChannel, true);
	
	// advance the target channel for next time
	m_overlapBuffTargetChannel++;
	m_overlapBuffTargetChannel %= m_overlap;
}

void OlaBuffer::windowBuffers(int olapChannel)
{
	for (int channel = 0; channel < m_channels; channel++)
	{
		auto* overlapBuffData = overlapBuffers[channel]->getWritePointer(olapChannel);
		m_window.multiplyWithWindowingTable(overlapBuffData, m_ringBuffSize);
	}
}

void OlaBuffer::outputOlaBlock(AudioBuffer<float>& outBuffer)
{
	int blockSize = outBuffer.getNumSamples();
	
	for(int channel = 0; channel < m_channels; channel++)
	{
		outBuffer.clear(channel, 0, blockSize);
		
		for(int overlapChannel = 0; overlapChannel < m_overlap; overlapChannel++)
		{
			int thisOlapChannel, startIdx;
			
			thisOlapChannel = m_overlapBuffTargetChannel + overlapChannel;
			thisOlapChannel %= m_overlap;
			
			startIdx = m_ringBuffWriteIdx - (m_hop * thisOlapChannel);
			if(startIdx < 0)
				startIdx += m_ringBuffSize;
			
			if(startIdx + blockSize < m_ringBuffSize)
				outBuffer.addFrom(channel, 0, *overlapBuffers[channel], thisOlapChannel, startIdx, blockSize);
			else {
				int numSamps = m_ringBuffSize-startIdx;
				outBuffer.addFrom(channel, 0, *overlapBuffers[channel], thisOlapChannel, startIdx, numSamps);
				outBuffer.addFrom(channel, numSamps, *overlapBuffers[channel], thisOlapChannel, startIdx, numSamps);
			}
		}
		
		outBuffer.applyGain(channel, 0, blockSize, 1.0f / (float)m_overlap);
	}
}

void OlaBuffer::processOlaBuffer(AudioBuffer<float>& buffer, void* processorFunc)
{
	// add block into ring buffer
	fillRingBuf(buffer);
	// fill an overlap buffer if there are enough samps
	fillOverlapBuffer();
	
	for(int olapChannel = 0; olapChannel < m_overlap; olapChannel++)
	{
		if(processFlags[olapChannel])
		{
			windowBuffers(olapChannel);
			
			// do processing here
			for(int channel = 0; channel < m_channels; channel++)
			{
//				processorFunc(overlapBuffers[channel] -> getWritePointer(olapChannel));
			}
			
			windowBuffers(olapChannel);
			
			processFlags.set(olapChannel, false);
		}
	}
	
	outputOlaBlock(buffer);
	
	m_ringBuffWriteIdx += buffer.getNumSamples();
	m_ringBuffWriteIdx %= m_ringBuffSize;
}
