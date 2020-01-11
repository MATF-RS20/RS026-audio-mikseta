/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 13 Nov 2019 7:34:33pm
    Author:  eaelste

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager& manager):thumbnailCache(5), thumbnail(512, manager, thumbnailCache)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	thumbnail.addChangeListener(this);

	isFileBeingDragged = false;
	displayRectangle = Rectangle<int>();

	mouseIn = false;
	mousePos = 0;

	playingPosition = 0;
	numSamples = 0;
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

	if (isFileBeingDragged)
	{
		g.setColour(Colours::purple);
		g.drawRoundedRectangle(displayRectangle.toFloat(), 5.0f, 2.0f);
	}
	else
	{
		g.setColour(Colours::violet);
		g.drawRoundedRectangle(displayRectangle.toFloat(), 5.0f, 1.0f);
	}
	

	thumbnail.drawChannel(g, displayRectangle.reduced(0, INDENT), 0.0, thumbnail.getTotalLength(), 0, 1.0f);
	
	g.setColour(Colours::palevioletred);
	g.drawVerticalLine(playingPosition, displayRectangle.getY() + 1.0f, displayRectangle.getHeight() - 1.0f);

	g.setColour(Colours::white);
	if (mouseIn)
		g.drawVerticalLine(mousePos, displayRectangle.getY()+1.0f, displayRectangle.getHeight()-1.0f);
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	displayRectangle = Rectangle<int>(0, 0, getWidth(), getHeight());
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &thumbnail)
	{
		numSamples = thumbnail.getNumSamplesFinished();
		repaint();
	}
}

void WaveformDisplay::fileBeingDragged(bool isBeingDragged)
{
	isFileBeingDragged = isBeingDragged;
	repaint();
}

void WaveformDisplay::setSource(File f)
{
	thumbnail.setSource(new FileInputSource(f));
	repaint();
}

void WaveformDisplay::addThumbnailListener(ChangeListener* listener)
{
	thumbnail.addChangeListener(listener);
}

void WaveformDisplay::mouseEnter(const MouseEvent& e)
{
	mouseIn = true;
	repaint();
}
void WaveformDisplay::mouseExit(const MouseEvent& e)
{
	mouseIn = false;
	repaint();
}
void WaveformDisplay::mouseMove(const MouseEvent& e) 
{
	mousePos = getMouseXYRelative().getX();
	repaint();
}
void WaveformDisplay::mouseDown(const MouseEvent& e) 
{
	playingPosition = getMouseXYRelative().getX();
	sendChangeMessage();
	repaint();
}

void WaveformDisplay::setPlayingPosition(int newPos)
{
	
	float positionInSample = static_cast<float>(newPos)/static_cast<float>(numSamples);
	playingPosition = positionInSample * getWidth();
	repaint();
}

int WaveformDisplay::getPlayingPosition()
{
	float positionInSample = static_cast<float>(playingPosition) / static_cast<float>(getWidth());
	return positionInSample * numSamples;
}