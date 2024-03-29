/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "SelectionColumnCustomComponent.h"
#include "EditableTextCustomComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1200, 600);
	addAndMakeVisible(table);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
	table.setBounds(5, 5, 1200, 600);
}


