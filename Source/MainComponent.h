/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/


class PlaylistTableComponent :	public Component,
								public TableListBoxModel
{
public :
	PlaylistTableComponent()
	{
		loadData();
		addAndMakeVisible(table);
		table.setColour(ListBox::outlineColourId, Colours::red);
		table.setOutlineThickness(1);
	
		if (columnList != nullptr)
		{
			forEachXmlChildElement(*columnList, columnXml)
			{
				table.getHeader().addColumn(columnXml->getStringAttribute("name"),
					columnXml->getIntAttribute("columnId"),
					columnXml->getIntAttribute("width"), 50, 400, TableHeaderComponent::defaultFlags);
			}
		}

	
		addAndMakeVisible(debugLabel);
		debugLabel.setText("default debug message",dontSendNotification);
		table.setMultipleSelectionEnabled(true);
	}

	int getNumRows() override
	{
		return numRows;
	}

	void paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
	{
		auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
			.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
		if (rowIsSelected)
			g.fillAll(Colours::lightblue);
		else if (rowNumber % 2)
			g.fillAll(alternateColour);
	}

	void paintCell(Graphics& g, int rowNumber, int columnId,
		int width, int height, bool rowIsSelected) override
	{
		g.setColour(rowIsSelected ? Colours::darkblue : getLookAndFeel().findColour(ListBox::textColourId)); // [5]
		g.setFont(font);
		if (auto* rowElement = dataList->getChildElement(rowNumber))
		{
			auto text = rowElement->getStringAttribute(getAttributeNameForColumnId(columnId));
			g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true); // [6]
		}
		g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
		g.fillRect(width - 1, 0, 1, height);                                             // [7]
	}


	Component* refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/,
		Component* existingComponentToUpdate) override
	{
		if (columnId == 4)
		{
			auto* mySelectionBox = dynamic_cast<SelectionColumnCustomComponent*>(existingComponentToUpdate);

			if (mySelectionBox == nullptr)
			{
				mySelectionBox = new SelectionColumnCustomComponent(*this);
			}

			mySelectionBox->setRowAndColumn(rowNumber, columnId);
			return mySelectionBox;
		}

		if (columnId == 3)
		{
			auto* textLabel = dynamic_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

			if (textLabel == nullptr)
			{
				textLabel = new EditableTextCustomComponent(*this);
			}

			textLabel->setRowAndColumn(rowNumber, columnId);
			return textLabel;
		}

		jassert(existingComponentToUpdate == nullptr);
		return nullptr;
	}

	int getColumnAutoSizeWidth(int columnId) override
	{
		if (columnId == 9)
			return 50;

		int widest = 32;

		for (auto i = getNumRows(); --i >= 0;)
		{
			if (auto* rowElement = dataList->getChildElement(i))
			{
				auto text = rowElement->getStringAttribute(getAttributeNameForColumnId(columnId));

				widest = jmax(widest, font.getStringWidth(text));
			}
		}

		return widest + 8;
	}


	int getSelection(const int rowNumber) const
	{
		return dataList->getChildElement(rowNumber)->getIntAttribute("Select");
	}

	void setSelection(const int rowNumber, const int newSelection)
	{
		dataList->getChildElement(rowNumber)->setAttribute("Select", newSelection);
	}

	String getText(const int columnNumber, const int rowNumber) const
	{
		return dataList->getChildElement(rowNumber)->getStringAttribute(getAttributeNameForColumnId(columnNumber));
	}

	void setText(const int columnNumber, const int rowNumber, const String& newText) const
	{
		const auto& columnName = table.getHeader().getColumnName(columnNumber);
		dataList->getChildElement(rowNumber)->setAttribute(columnName, newText);
	}

	void resized() override
	{
		debugLabel.setBounds(100, 500, 500, 30);
		table.setBoundsInset(BorderSize<int>(8));
	}

	void debugLabelMsg(String message)
	{
		debugLabel.setText(message, dontSendNotification);
	}


private:

	TableListBox table{ {}, this };
	Font font{ 14.0 };
	std::unique_ptr<XmlElement> playlistData;
	XmlElement* columnList = nullptr;
	XmlElement* dataList = nullptr;
	int numRows = 0;
	Label debugLabel;
	//==========================================================================
	class EditableTextCustomComponent : public Label
	{
	public:
		EditableTextCustomComponent(PlaylistTableComponent& td)
			:owner(td)
		{
			setEditable(false, true, false);
		}

		void mouseDown(const MouseEvent& event) override
		{
			owner.table.selectRowsBasedOnModifierKeys(row, event.mods, false);
			Label::mouseDown(event);
		}

		void textWasEdited()override
		{
			owner.setText(columnId, row, getText());
		}

		void setRowAndColumn(const int newRow, const int newColumn)
		{
			row = newRow;
			columnId = newColumn;
			setText(owner.getText(columnId, row), dontSendNotification);
		}

	private:
		PlaylistTableComponent& owner;
		int row, columnId;
		Colour textColour;
	};

	class SelectionColumnCustomComponent : public Component
	{
	public:
		SelectionColumnCustomComponent(PlaylistTableComponent& td)
			: owner(td)
		{
			addAndMakeVisible(toggleButton);

			toggleButton.onClick = [this] { owner.setSelection(row, (int)toggleButton.getToggleState()); };
		}

		void resized() override
		{
			toggleButton.setBoundsInset(BorderSize<int>(2));
		}

		void setRowAndColumn(int newRow, int newColumn)
		{
			row = newRow;
			columnId = newColumn;
			toggleButton.setToggleState((bool)owner.getSelection(row), dontSendNotification);
		}

	private:
		PlaylistTableComponent& owner;
		ToggleButton toggleButton;
		int row, columnId;
	};



	void loadData()
	{
		auto dir = File::getCurrentWorkingDirectory();

		int numTries = 0;

		while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
			dir = dir.getParentDirectory();

		auto tableFile = dir.getChildFile("Resources").getChildFile("Table_Data.xml");

		if (tableFile.exists())
		{
			playlistData= XmlDocument::parse(tableFile);
			dataList = playlistData->getChildByName("DATA");
			columnList = playlistData->getChildByName("HEADERS");

			numRows = dataList->getNumChildElements();
		}
		else
		{
			debugLabelMsg("no such file you nob");
		}
	}

	String getAttributeNameForColumnId(const int columnId) const
	{
		forEachXmlChildElement(*columnList, columnXml)
		{
			if (columnXml->getIntAttribute("columnId") == columnId)
				return columnXml->getStringAttribute("name");
		}
		return{};
	}


};


//===================================================================================
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
	PlaylistTableComponent table;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};


