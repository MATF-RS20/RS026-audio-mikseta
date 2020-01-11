/*
  ==============================================================================

    WaveformDisplay.h
    Created: 13 Nov 2019 7:34:33pm
    Author:  eaelste

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define INDENT 5
//==============================================================================
/*
*/
class WaveformDisplay    : public Component,
	public ChangeBroadcaster,
	public ChangeListener
{
public:
    WaveformDisplay(AudioFormatManager& manager);
    ~WaveformDisplay();

    void paint (Graphics&) override;
    void resized() override;

	void changeListenerCallback(ChangeBroadcaster* source) override;

	void fileBeingDragged(bool isBeingDragged);

	void setSource(File f);

	void addThumbnailListener(ChangeListener* listener);

	void mouseEnter(const MouseEvent& e) override;
	void mouseExit(const MouseEvent& e) override;
	void mouseMove(const MouseEvent& e) override;
	void mouseDown(const MouseEvent& e) override;

	void setPlayingPosition(int newPos);

	int getPlayingPosition();
private:
	AudioThumbnailCache thumbnailCache;
	AudioThumbnail thumbnail;
	Rectangle<int> displayRectangle;

	bool isFileBeingDragged;

	bool mouseIn;
	int mousePos;

	int playingPosition;
	int numSamples;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
