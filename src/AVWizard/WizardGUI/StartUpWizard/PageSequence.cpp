#include "PageSequence.h"

PageSequence::PageSequence(const std::string & StartPage)
{
	const char * PageOrder[] =
	{
		"Welcome",
		"VideoResolution",
		"VideoResolutionConfirm",
		"UISwitcher",
		"UISwitcherTest",
		"VideoAdjustSize",
		"AudioSoundCard",
		"AudioConnector",
		"AudioVolume",
		"DolbyTest",
		"DTSTest",
		"FinalSelections",
		NULL
	};

	CurrentPageIdx = -1;
	for (size_t i = 0; PageOrder[i] != NULL; i++)
	{
		Sequence.push_back(PageOrder[i]);
		if (StartPage == PageOrder[i])
			CurrentPageIdx = i;
	}

	LastPageOperation = &PageSequence::NextPage;
}

std::string PageSequence::NextPage()
{
	LastPageOperation = &PageSequence::NextPage;

	if (CurrentPageIdx < Sequence.size())
		++CurrentPageIdx;
	if (CurrentPageIdx < Sequence.size())
		return Sequence[CurrentPageIdx];
	else
		return "";
}

std::string PageSequence::PrevPage()
{
	LastPageOperation = &PageSequence::PrevPage;

	if (CurrentPageIdx >= 0)
		--CurrentPageIdx;
	if (CurrentPageIdx >= 0)
		return Sequence[CurrentPageIdx];
	else
		return "";
}

std::string PageSequence::CurrentPage()
{
	if (CurrentPageIdx >= 0 && CurrentPageIdx < Sequence.size())
		return Sequence[CurrentPageIdx];
	else
		return "";
}

std::string PageSequence::RepeatLastOperation()
{
	return (this->*LastPageOperation)();
}

bool PageSequence::GoToPage(const std::string & Page)
{
	for (size_t i = 0; i < Sequence.size(); i++)
	{
		if (Page == Sequence[i])
		{
			CurrentPageIdx = i;
			return true;
		}
	}
	return false;
}

size_t PageSequence::Size()
{
	return Sequence.size();
}
