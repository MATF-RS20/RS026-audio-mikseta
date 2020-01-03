/*
  ==============================================================================

    FileExplorer.h
    Created: 13 Nov 2019 11:01:54am
    Author:  eaelste

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ExplorerFilter : public FileFilter
{
public:
	ExplorerFilter(String s);
	bool isFileSuitable(const File& file) const override;
	bool isDirectorySuitable(const File& file) const override;
};

class DragFileTreeComponent : public FileTreeComponent
{
public:
	DragFileTreeComponent(DirectoryContentsList& list);

	bool isInterestedInFileDrag(const StringArray& files) override;
	void filesDropped(const StringArray &files, int x, int y) override;
	void fileDragEnter(const StringArray& files, int x, int y) override;
};

class FileExplorer    : public Component,
	public DragAndDropContainer
{
public:
    FileExplorer();
    ~FileExplorer();

    void paint (Graphics&) override;
    void resized() override;

	
private:
	TimeSliceThread tst;
	DirectoryContentsList explorerList;
	DragFileTreeComponent explorer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileExplorer)
};
