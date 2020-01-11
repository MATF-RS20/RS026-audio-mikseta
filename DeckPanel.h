/*
  ==============================================================================

    DeckPanel.h
    Created: 12 Nov 2019 8:45:00pm
    Author:  eaelste

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

enum PlayingState
{
	Playing,
	Paused,
	Looping,
	Stopped
};

class ReferenceCountedBuffer : public ReferenceCountedObject
{
public:
	typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

	ReferenceCountedBuffer(const String& nameToUse,
		int numChannels,
		int numSamples)
		: name(nameToUse),
		buffer(numChannels, numSamples)
	{
		
	}

	~ReferenceCountedBuffer()
	{
		
	}

	AudioSampleBuffer* getAudioSampleBuffer()
	{
		return &buffer;
	}

	int position = 0;

private:
	String name;
	AudioSampleBuffer buffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReferenceCountedBuffer)
};

//==============================================================================
/*
*/
class DeckPanel : public Component,
	private Thread,
	public DragAndDropTarget,
	public FileDragAndDropTarget,
	public ChangeListener,
	public Button::Listener
{
public:
    DeckPanel();
    ~DeckPanel();

    void paint (Graphics&) override;
    void resized() override;

	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;

	void run() override;
	void checkForBuffersToFree();

	void openFile(File f);

	void clearBuffer();

	void processNextBlock(const AudioSourceChannelInfo& bufferToFill);

	void checkForPathToOpen();

	bool isInterestedInFileDrag(const StringArray& files) override;
	void filesDropped(const StringArray &files, int x, int y) override;
	void fileDragEnter(const StringArray& files, int x, int y) override;
	void fileDragExit(const StringArray& files) override;

	void setGain(float newGain);

	void changeListenerCallback(ChangeBroadcaster* source) override;

	void buttonClicked(Button* buttonThatWasClicked) override;
private:
	String filePath;
	PlayingState state;

	TextButton play, pause, stop, loop;

	AudioFormatManager formatManager;

	ReferenceCountedArray<ReferenceCountedBuffer> buffers;
	ReferenceCountedBuffer::Ptr currentBuffer;

	
	WaveformDisplay display;

	float gain;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckPanel)
};
