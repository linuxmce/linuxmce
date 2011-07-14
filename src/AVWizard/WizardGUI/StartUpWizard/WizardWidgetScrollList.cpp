/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "WizardWidgetScrollList.h"
//---------------------------------------------------------------------------
#include "SDL_image.h"
#include "GUIWizardUtils.h"
//---------------------------------------------------------------------------
#include <iostream>
//---------------------------------------------------------------------------

WizardWidgetScrollList::WizardWidgetScrollList(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name),
	  Focused(false),
	  ItemIndex(-1)
{
	this->BackColor = Utils::StringToColorDesc("70A070");
	this->BackFocusedColor = Utils::StringToColorDesc("A0A0A0");
	this->HighBackColor = Utils::StringToColorDesc("00AF00");
	this->HighBackFocusedColor = Utils::StringToColorDesc("FFFFFF");
}

WizardWidgetScrollList::~WizardWidgetScrollList()
{
	delete BackColor;
	BackColor = NULL;
	delete BackFocusedColor;
	BackFocusedColor = NULL;
	delete HighBackColor;
	HighBackColor = NULL;
	delete HighBackFocusedColor;
	HighBackFocusedColor = NULL;
}

/*virtual*/ void WizardWidgetScrollList::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "ScrollBackColor")
	{
		delete BackColor;
		BackColor = Utils::StringToColorDesc(DataValue);
	}
	else
	if(Attribute == "ScrollBackFocusedColor")
	{
		delete BackFocusedColor;
		BackFocusedColor = Utils::StringToColorDesc(DataValue);
	}
	else
	if(Attribute == "HighScrollBackColor")
	{
		delete HighBackColor;
		HighBackColor = Utils::StringToColorDesc(DataValue);
	}
	else
	if(Attribute == "HighScrollBackFocusedColor")
	{
		delete HighBackFocusedColor;
		HighBackFocusedColor = Utils::StringToColorDesc(DataValue);
	}
	else
	if(Attribute == "Focused")
		Focused = Utils::StringToInt32(DataValue) != 0;
	else
		WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}


WizardWidgetPage* WizardWidgetScrollList::GetContainerPage()
{
	WizardWidgetBase* Result = this;
	while(Result->Parent != NULL)
		Result = Result->Parent;

	WizardWidgetPage* Page = (WizardWidgetPage*) Result;
	return Page;
}

/*virtual*/ void WizardWidgetScrollList::AddItem(std::string ItemName, std::string ItemValue,
						 std::string ItemRatio)
{
	if(ItemNames.size() == 0)
		ItemIndex = 0;
	ItemNames.push_back(ItemName);
	ItemValues.push_back(ItemValue);
	ItemRatios.push_back(ItemRatio);
}

void WizardWidgetScrollList::RemoveItem(std::string ItemValue)
{
	// TODO
}

void WizardWidgetScrollList::Clear()
{
	ItemNames.clear();
	ItemValues.clear();
	ItemRatios.clear();
	ItemIndex = -1;
}

int WizardWidgetScrollList::GetItemIndex()
{
	return ItemIndex;
}

/*virtual*/ void WizardWidgetScrollList::Paint()
{
	if(BackColor == NULL)
	{
		std::cout<<"Warning: Backcolor is NULL and shouldn't, setup the default one!";
		BackColor = Utils::StringToColorDesc("000000");
	}
	TColorDesc* Black = Utils::StringToColorDesc("000000");
	FrontEnd->HLine(Left+1, Width-2, Top, *Black);
	FrontEnd->HLine(Left+1, Width-2, Top+Height-1, *Black);
	FrontEnd->VLine(Left, Top+1, Height-2, *Black);
	FrontEnd->VLine(Left+Width-1, Top+1, Height-2, *Black);

	if(Focused)
	{
		FrontEnd->Rectangle(Left-4, Top-4, Width + 8, Height + 8, *Black);
		FrontEnd->FillRectangle(Left+1, Top+1, Width-2, Height-2, *HighBackColor);
	}
	else
		FrontEnd->FillRectangle(Left+1, Top+1, Width-2, Height-2, *BackColor);


	delete Black;
	Black = NULL;

	int NoItems = ItemNames.size();
	int NoOfDrawableItems = Height / 20;

	for (int i = 0; i<NoItems; i++)
	{
		int DeltaPos = i-ItemIndex;
		if( (DeltaPos<=(NoOfDrawableItems/2)) &&
			(DeltaPos>=(-NoOfDrawableItems/2)) )
		{
			std::string Item = ItemNames[i];
			WriteItem(Item, ItemIndex == i,
				i-ItemIndex+(NoOfDrawableItems/2), 0, 5, 20);
		}
		//FrontEnd->TextOutput((char*)Item.c_str(), Left+4, Top+4+i*18, *Black, 0);
	}
	
	
	
}

void WizardWidgetScrollList::WriteItem(std::string Name, bool Selected,
		int Index, int TopItemIndex, int BottomItemIndex, 
		int ItemHeight)
{
	TColorDesc* Black = Utils::StringToColorDesc("000000");
	if(Selected)
	{

		FrontEnd->HLine(Left+1, Width-1, Top+1+(Index - TopItemIndex)*ItemHeight, *Black);
		FrontEnd->HLine(Left+1, Width-2, Top+(Index - TopItemIndex+1)*ItemHeight, *Black);
		FrontEnd->VLine(Left+1, Top+2+(Index - TopItemIndex)*ItemHeight, ItemHeight-2, *Black);
		FrontEnd->VLine(Left+Width-2, Top+2+(Index - TopItemIndex)*ItemHeight, ItemHeight-2, *Black);

		if(Focused)
		{
		FrontEnd->FillRectangle(Left+2, Top+2+(Index - TopItemIndex)*ItemHeight,
			Width - 4, ItemHeight - 2,
			*HighBackFocusedColor);
		}
		else
		FrontEnd->FillRectangle(Left+2, Top+2+(Index - TopItemIndex)*ItemHeight,
			Width - 4, ItemHeight - 2,
			*BackFocusedColor);
	
	}
	FrontEnd->TextOutput((char*)Name.c_str(), Left+4,
		Top+2+(Index - TopItemIndex)*ItemHeight, *Black, 0);
	
	delete Black;
	Black = NULL;
}

	
void WizardWidgetScrollList::SetFocus(bool IsFocused)
{
	this->Focused = IsFocused;
}

bool WizardWidgetScrollList::GetFocus()
{
	return Focused;
}

std::string WizardWidgetScrollList::GetSelectedValue()
{
	std::string Result;
	int Index = GetItemIndex();
	if (-1 == Index)
		return Result;
	Result = ItemValues[Index];
	return Result;
}

std::string WizardWidgetScrollList::GetSelectedName()
{
	std::string Result;
	int Index = GetItemIndex();
	if (-1 == Index)
		return Result;
	Result = ItemNames[Index];
	return Result;
}

std::string WizardWidgetScrollList::GetSelectedRatio()
{
  std::string Result;
  int Index = GetItemIndex();
  if (-1 == Index)
    return Result;
  Result = ItemRatios[Index];
  return Result;
}

void WizardWidgetScrollList::SetItemIndex(int ItemIndex)
{
	this->ItemIndex = ItemIndex;
	if (this->ItemIndex < 0)
		this->ItemIndex = 0;
	if ((size_t)this->ItemIndex >= ItemNames.size())
		this->ItemIndex = ItemNames.size()-1;
}

void WizardWidgetScrollList::SetItemIndex(std::string TextToSelect)
{
	ItemIndex = -1;
	for(size_t i = 0; i < ItemValues.size(); i++)
		if(TextToSelect == ItemValues[i])
		{
			ItemIndex = i;
			return;
		}
}
