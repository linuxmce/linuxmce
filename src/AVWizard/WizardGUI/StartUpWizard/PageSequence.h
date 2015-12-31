#ifndef PageSequence_h
#define PageSequence_h

#include <string>
#include <vector>

typedef std::vector<std::string> StringVector;

class PageSequence
{
	public:
		PageSequence(const std::string & StartPage);
		std::string NextPage();
		std::string PrevPage();
		std::string CurrentPage();
		std::string RepeatLastOperation();
		bool GoToPage(const std::string & Page);
		size_t Size();

	private:
		StringVector Sequence;
		int CurrentPageIdx;

		std::string (PageSequence::*LastPageOperation)();
};

#endif /*PageSequence_h*/
