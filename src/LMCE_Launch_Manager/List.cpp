#include "List.h"
#include <vector>
#include <iostream>

List::List()
{

}
List::~List()
{

}
void List::append(string s) 
{
	m_vList.push_back(s);
}
void List::clear()
{
	m_vList.clear();
}
int List::find(string s)
{
	for(unsigned int i=0;i<m_vList.size();i++) {
		if(m_vList[i] == s)
			return i;
	}
	return int(m_vList.size()-1);
}
bool List::contains(string s)
{
	for(unsigned int i=0;i<m_vList.size();i++) {
		if(m_vList[i] == s)
			return true;
	}
	return false;
}
int List::end() 
{
	return m_vList.size()-1;
}
string List::join(string sSeparator) 
{
	string s;	
	for(unsigned int i=0;i<m_vList.size();i++) {
		s+=m_vList[i] + sSeparator;
	}
	return s;
}
unsigned int List::size()
{
	return m_vList.size();
}

string List::index(int i) {
	return m_vList[i];
}
