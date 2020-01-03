/*
  ==============================================================================

    FileExplorer.cpp
    Created: 13 Nov 2019 11:01:54am
    Author:  eaelste

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FileExplorer.h"

ExplorerFilter::ExplorerFilter(String s):FileFilter(s)
{

}

bool ExplorerFilter::isFileSuitable(const File& file) const
{
	//return true;
	//Prikazuje samo fajlove koji su .wav
	if (file.getFileExtension() == ".wav")
		return true;
	else
		return false;
}

bool ExplorerFilter::isDirectorySuitable(const File& file) const
{
	//return true;
	//prikazuje samo foldere koji negde u sebi imaju neki audio fajl .wav
	int foundAudioFiles = 0;
	Array<File> childFiles;
	int numOfSubFiles = file.findChildFiles(childFiles, File::TypesOfFileToFind::findFiles, true);
	for (int i = 0; i < numOfSubFiles; i++)
	{
		if (childFiles[i].getFileExtension() == ".wav")
			foundAudioFiles++;
	}
	if (foundAudioFiles > 0)
		return true;
	else
		return false;
}
//==============================================================================

DragFileTreeComponent::DragFileTreeComponent(DirectoryContentsList& list):FileTreeComponent(list)
{

}

bool DragFileTreeComponent::isInterestedInFileDrag(const StringArray& files)
{
	//if (files.size() == 1)
	{
		return true;
	}
}

void DragFileTreeComponent::filesDropped(const StringArray &files, int x, int y)
{
	directoryContentsList.setDirectory(File(files[0]), true, true);
}

void DragFileTreeComponent::fileDragEnter(const StringArray& files, int x, int y)
{

}

//==============================================================================
FileExplorer::FileExplorer() :tst("Explorer Thread"),explorerList(new ExplorerFilter("Only .wav files"),tst),explorer(explorerList)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	addAndMakeVisible(&explorer);
	
	tst.startThread();
	explorerList.setDirectory(File::getCurrentWorkingDirectory().getParentDirectory(), true, true);
	explorer.setDragAndDropDescription("File");
	//explorer.setRootItemVisible(true);
	
}

FileExplorer::~FileExplorer()
{
}

void FileExplorer::paint (Graphics& g)
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

void FileExplorer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	explorer.setBounds(getLocalBounds());
}

