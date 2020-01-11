/*
  ==============================================================================

    DeckPanel.cpp
    Created: 12 Nov 2019 8:45:00pm
    Author:  eaelste

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckPanel.h"

//==============================================================================
DeckPanel::DeckPanel():Thread("AudioFileLoadingThread"),display(formatManager)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	filePath = String();
	startThread();

	state = PlayingState::Stopped;

	formatManager.registerBasicFormats();

	gain = 0.5f;

	display.addChangeListener(this);
	addAndMakeVisible(&display);

	addAndMakeVisible(&play);
	play.addListener(this);
	play.setButtonText("Play");
	
	addAndMakeVisible(&pause);
	pause.addListener(this);
	pause.setButtonText("Pause");

	addAndMakeVisible(&loop);
	loop.addListener(this);
	loop.setButtonText("Loop");

	addAndMakeVisible(&stop);
	stop.addListener(this);
	stop.setButtonText("Stop");

}

DeckPanel::~DeckPanel()
{
	stopThread(2000);
}

void DeckPanel::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

	
	g.setColour(Colours::black);
	g.drawRect(getLocalBounds());

}

void DeckPanel::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	
	display.setBounds(Rectangle<int>(0, 0, getWidth(), getHeight() / 2).reduced(INDENT));

	int buttonWidth = getWidth() / 4;
	play.setBounds(0, getHeight() / 2, buttonWidth, buttonWidth / 2);
	pause.setBounds(buttonWidth, getHeight() / 2, buttonWidth, buttonWidth / 2);
	loop.setBounds(2*buttonWidth, getHeight() / 2, buttonWidth, buttonWidth / 2);
	stop.setBounds(3*buttonWidth, getHeight() / 2, buttonWidth, buttonWidth / 2);
}

bool DeckPanel::isInterestedInFileDrag(const StringArray& files)
{
	if (files.size() == 1)
	{
		if (File(files[0]).getFileExtension() == ".wav")
			return true;
		else
			return false;
	}
	else
		return false;
}

void DeckPanel::filesDropped(const StringArray &files, int x, int y)
{
	filePath=File(files[0]).getFullPathName();
	display.fileBeingDragged(false);
	
	notify();
}

void DeckPanel::fileDragEnter(const StringArray& files, int x, int y)
{
	display.fileBeingDragged(isInterestedInFileDrag(files));
}

void DeckPanel::fileDragExit(const StringArray& files)
{
	if (isInterestedInFileDrag(files))
		display.fileBeingDragged(false);
}

void DeckPanel::run()
{
	while (!threadShouldExit())
	{
		checkForBuffersToFree();
		checkForPathToOpen();
		wait(500);
	}
}

void DeckPanel::checkForBuffersToFree()
{
	for (auto i = buffers.size(); --i >= 0;)                           // [1]
	{
		ReferenceCountedBuffer::Ptr buffer(buffers.getUnchecked(i)); // [2]

		if (buffer->getReferenceCount() == 2)                          // [3]
			buffers.remove(i);
	}
}

void DeckPanel::checkForPathToOpen()
{
	String pathToOpen;
	pathToOpen.swapWith(filePath);

	if (pathToOpen.isNotEmpty())
	{
		File file(pathToOpen);
		std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));

		if (reader.get() != nullptr)
		{
			auto duration = reader->lengthInSamples / reader->sampleRate;

			if (duration < 600)
			{
				ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),
					reader->numChannels,
					(int)reader->lengthInSamples);
				reader->read(newBuffer->getAudioSampleBuffer(), 0, (int)reader->lengthInSamples, 0, true, true);
				currentBuffer = newBuffer;
				buffers.add(newBuffer);

				display.setSource(file);
			}
			else
			{
				// handle the error that the file is 600 seconds or longer..
			}
		}
	}
}

void DeckPanel::openFile(File f)
{
	
}

void DeckPanel::clearBuffer()
{
	currentBuffer = nullptr;
}

bool DeckPanel::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
	if (dragSourceDetails.description.toString() == "File")
	{
		return true;
	}
	else
		return false;
}

void DeckPanel::itemDragEnter(const SourceDetails& dragSourceDetails)
{
	display.fileBeingDragged(isInterestedInDragSource(dragSourceDetails));
}

void DeckPanel::itemDropped(const SourceDetails& dragSourceDetails)
{
	if (isInterestedInDragSource(dragSourceDetails))
	{
		FileTreeComponent* dragComponent=dynamic_cast<FileTreeComponent*>(dragSourceDetails.sourceComponent.get());
		if (dragComponent != nullptr)
		{
			File f=dragComponent->getSelectedFile();
			filePath = f.getFullPathName();

			display.fileBeingDragged(false);
			notify();
		}
	}
}


void DeckPanel::itemDragExit(const SourceDetails& dragSourceDetails)
{
	if (isInterestedInDragSource(dragSourceDetails))
		display.fileBeingDragged(false);
}

void DeckPanel::processNextBlock(const AudioSourceChannelInfo& bufferToFill)
{
	ReferenceCountedBuffer::Ptr retainedCurrentBuffer(currentBuffer);
	if (retainedCurrentBuffer == nullptr) // [5]
	{
		//bufferToFill.clearActiveBufferRegion();
		return;
	}
	if (state != Stopped)
	{
		// [4]
		if (state != Paused)
		{

			

			auto* currentAudioSampleBuffer = retainedCurrentBuffer->getAudioSampleBuffer();               // [6]
			auto position = retainedCurrentBuffer->position;                                              // [7]

			auto numInputChannels = currentAudioSampleBuffer->getNumChannels();
			auto numOutputChannels = bufferToFill.buffer->getNumChannels();

			auto outputSamplesRemaining = bufferToFill.numSamples;
			auto outputSamplesOffset = 0;

			display.setPlayingPosition(position);
			while (outputSamplesRemaining > 0)
			{
				auto bufferSamplesRemaining = currentAudioSampleBuffer->getNumSamples() - position;
				auto samplesThisTime = jmin(outputSamplesRemaining, bufferSamplesRemaining);

				for (auto channel = 0; channel < numOutputChannels; ++channel)
				{
					bufferToFill.buffer->addFrom(channel,
						bufferToFill.startSample + outputSamplesOffset,
						*currentAudioSampleBuffer,
						channel % numInputChannels,
						position,
						samplesThisTime, gain);

				}
				
				outputSamplesRemaining -= samplesThisTime;
				outputSamplesOffset += samplesThisTime;
				position += samplesThisTime;

				if (position == currentAudioSampleBuffer->getNumSamples())
				{
					//ovo loop-uje audio
					position = 0;
					if (state != Looping)
					{
						state = PlayingState::Stopped;
					}
				}
			}

			retainedCurrentBuffer->position = position; // [8]
		}
	}
	else
	{
		retainedCurrentBuffer->position = 0;
		display.setPlayingPosition(retainedCurrentBuffer->position);
		 
	}
}

void DeckPanel::setGain(float newGain)
{
	gain = newGain;
}

void DeckPanel::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &display)
	{
		ReferenceCountedBuffer::Ptr retainedCurrentBuffer(currentBuffer); // [4]

		if (retainedCurrentBuffer == nullptr) // [5]
		{
			//bufferToFill.clearActiveBufferRegion();
			return;
		}
              // [6]
		retainedCurrentBuffer->position=display.getPlayingPosition();                                              // [7]

	}
}

void DeckPanel::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == &play)
	{
		state = PlayingState::Playing;
	}
	else if (buttonThatWasClicked == &pause)
	{
		state = PlayingState::Paused;
	}
	else if (buttonThatWasClicked == &loop)
	{
		state = PlayingState::Looping;
	}
	else if (buttonThatWasClicked == &stop)
	{
		state = PlayingState::Stopped;
	}
}