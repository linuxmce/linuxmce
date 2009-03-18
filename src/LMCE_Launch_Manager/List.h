#ifndef List_h
#define List_h
//Simple list class. Provides methods to append an entry, clear the list, and see if the list contains a certain entry.
//This is needed to replace the QStringList in the old code.
//jondecker76
#include <vector>
#include "LMCE_Launch_Manager.h"
#include "Gen_Devices/LMCE_Launch_ManagerBase.h"

class List
{
private:
	vector<string> m_vList;
public:
	//constructor
	List();
	//destructor
	~List();
	//append to the end of the list
	void append(string s);
	void clear();
	bool contains(string s);
	int find(string s);
	int end();
	unsigned int size();
	string index(int i);
	string join(string sSeparator);
};
#endif
