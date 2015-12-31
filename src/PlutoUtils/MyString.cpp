/* 
	MyString
		
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "MyString.h"

//-----------------------------------------------------------------------------------------
string::string(const char *str):tmp_buff(NULL) 
{
	TInt len = strlen(str);
	base_str = HBufC::NewL(len);
	TPtr pStr = base_str->Des();

	for (int i = 0; i < len; i++)
		pStr.Append(TChar(str[i]));
}
//-----------------------------------------------------------------------------------------
string::string(const string& str):tmp_buff(NULL) 
{
	TInt len = str.length();
	base_str = HBufC::NewL(len);
	TPtr pStr = base_str->Des();

	for (int i = 0; i < len; i++)
		pStr.Append(str[i]);


	/*
	base_str = HBufC::NewL(str.base_str->Length());
	TPtr local = base_str->Des();
	TPtr another = str.base_str->Des();
	local.Copy(another);
	*/
}
//-----------------------------------------------------------------------------------------
string::string(const TDesC& str):tmp_buff(NULL) {
	base_str = HBufC::NewL(str.Length());
	TPtr local = base_str->Des();
	local.Copy(str);
}
//-----------------------------------------------------------------------------------------
string::~string() 
{
	delete base_str;
	delete tmp_buff;
}
//-----------------------------------------------------------------------------------------
const char* string::c_str() {
	if (tmp_buff) 
		delete tmp_buff;

	tmp_buff = new char[base_str->Length() + 1];
	TPtr str = base_str->Des();

	for (int i = 0; i < str.Length(); i++)
		tmp_buff[i] = (char)str[i];
	tmp_buff[str.Length()] = 0;

	return tmp_buff;
}
//-----------------------------------------------------------------------------------------
string::size_type string::find(const string& str, string::size_type pos) {
	TPtr lStr = base_str->Des();
	TInt lPos =  lStr.Mid(pos).Find(str.Des());
	if (lPos != KErrNotFound)
		return lPos + pos;
	else
		return (unsigned int)npos;
}
//-----------------------------------------------------------------------------------------
string::size_type string::find(const char ch, string::size_type pos) {
	TBuf<1> tmp;
	tmp.Append(TChar(ch));
	return find(tmp, pos);
}
//-----------------------------------------------------------------------------------------
string& string::replace(string::size_type pos, string::size_type n, 
		const string& str) {
	base_str->Des().Replace(pos, n, str.Des());
	return *this;
}
//-----------------------------------------------------------------------------------------
string operator + (string a, string b) {
	HBufC *tmpStr = HBufC::NewL(a.length() + b.length());
	TPtr str = tmpStr->Des();
	
	str.Copy(a.Des());
	str.Append(b.Des());
	string ret(str);
	
	delete tmpStr;
	return ret;
}
//-----------------------------------------------------------------------------------------
TBool operator < (string a, string b) {
	return a.Des().Compare(b.Des()) == -1;
}
//-----------------------------------------------------------------------------------------
TBool operator > (string a, string b) {
	return a.Des().Compare(b.Des()) == 1;
}
//-----------------------------------------------------------------------------------------
TBool operator == (string a, string b) {
	return a.Des().Compare(b.Des()) == 0;
}
//-----------------------------------------------------------------------------------------
TBool operator != (string a, string b) {
	return a.Des().Compare(b.Des()) != 0;
}
//-----------------------------------------------------------------------------------------
string& string::operator = (const char* str) {
	delete base_str;

	TInt len = strlen(str);
	base_str = HBufC::NewL(len);
	TPtr pStr = base_str->Des();

	for (int i = 0; i < len; i++)
		pStr.Append(TChar(str[i]));

	return *this;
}
//-----------------------------------------------------------------------------------------
string& string::operator = (char ch) {
	delete base_str;

	base_str = HBufC::NewL(1);
	TPtr pStr = base_str->Des();

	pStr.Append(TChar(ch));

	return *this;
}
//-----------------------------------------------------------------------------------------
string& string::operator = (const string& str) {
	delete base_str;

	base_str = HBufC::NewL(str.length());
	TPtr local = base_str->Des();
	local.Copy(str.Des());

	return *this;
}
//-----------------------------------------------------------------------------------------
string& string::operator = (const TDesC& str) {
	delete base_str;
	base_str = HBufC::NewL(str.Length());
	TPtr local = base_str->Des();
	local.Copy(str);

	return *this;
}
//-----------------------------------------------------------------------------------------
string&  string::operator += (const char* str) {
	HBufC *tmp_base_str = base_str;

	TInt len = strlen(str) + tmp_base_str->Length();
	base_str = HBufC::NewL(len);
	TPtr pStr = base_str->Des();

	pStr.Copy(tmp_base_str->Des());
	for (int i = 0; i < len; i++)
		pStr.Append(TChar(str[i]));

	delete tmp_base_str;

	return *this;
}
//-----------------------------------------------------------------------------------------
string&  string::operator += (const string& str) {
	HBufC *tmp_base_str = base_str;

	base_str = HBufC::NewL(str.length() + tmp_base_str->Length());
	TPtr local = base_str->Des();
	local.Copy(tmp_base_str->Des());
	local.Append(str.Des());

	delete tmp_base_str;

	return *this;
}
//-----------------------------------------------------------------------------------------
string&  string::operator += (const TDesC& str) {
	HBufC *tmp_base_str = base_str;

	base_str = HBufC::NewL(str.Length() + tmp_base_str->Length());
	TPtr local = base_str->Des();
	local.Copy(tmp_base_str->Des());
	local.Append(str);

	delete tmp_base_str;

	return *this;
}
//-----------------------------------------------------------------------------------------
string&  string::operator += (const char ch) {
	HBufC *tmp_base_str = base_str;

	TInt len = 1 + tmp_base_str->Length();
	base_str = HBufC::NewL(len);
	TPtr pStr = base_str->Des();

	pStr.Copy(tmp_base_str->Des());
	pStr.Append(TChar(ch));

	delete tmp_base_str;

	return *this;
}
//-----------------------------------------------------------------------------------------
void string::SetLength(size_type Size)
{
	//TODO: To be implemented!

	Size = 0;
}
//-----------------------------------------------------------------------------------------
void string::Append(char Char)
{
	/*
	TPtr pStr = base_str->Des();
	TInt OldLength = pStr.Lenght();

	HBufC* new_base_str = HBufC::NewL(OldLength + 1);
	TPtr pNewStr = base_str->Des();

	for (int i = 0; i < OldLength; i++)
		pNewStr.Append(pStr[i]);

	pNewStr[OldLength] = Char;

	if(tmp_buff)
	{
		delete tmp_buff;
		tmp_buff = NULL;
	}

	delete new_base_str;
	new_base_str = NULL;

	base_str = new_base_str;
	*/

	(*this) += Char;
}
//-----------------------------------------------------------------------------------------
/*static*/ int string::ToInteger(string str, int DefaultValue)
{
	TLex8 lex(reinterpret_cast<const unsigned char*>(str.c_str()));
	TInt val = 0;
	
	if(KErrNone != lex.Val(val))
		return DefaultValue;

	return val;
}
//-----------------------------------------------------------------------------------------
/*static*/ string string::IntegerToString(int Value)
{
	TBuf16<100> buf;

	TUint number(Value);
	buf.Num(number, EDecimal); 

	return string(buf);
}
//-----------------------------------------------------------------------------------------
string::size_type string::strlen(const char *str)
{
	string::size_type size = 0;

	while(*(str + size) != '\0')
	{
		size++;
	}
	
	return size;
}
//-----------------------------------------------------------------------------------------

