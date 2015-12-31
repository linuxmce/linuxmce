/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file MyString.h
 Header file for ??? .
 */
#ifndef __MyString_h
#define __MyString_h

#include <e32std.h>
#include <e32base.h>
//-----------------------------------------------------------------------------------------

/** @class string
 Class for strings.
 */
class string:public CBase
{
	public:

		typedef TUint size_type;
		enum { npos = -1 };
		//static const size_type npos = ~(size_type)0;

		// Default empty constuctor
		string():tmp_buff(NULL) {
			base_str = HBufC::NewL(0);
		}

		string(const char *str);

		string(const string& str);

		string(const TDesC& str);

		// Destructor
		virtual ~string();

		// Returns pointer to the zero-terminating string
		const char* c_str();

		TPtr Des() const {
			return base_str->Des();
		}

		// Returns length of a string
		size_type length() const {
			return base_str->Length();
		}

		/*
		string  substr(TInt first, TInt last) {
			return string(base_str->Des().Mid(first,
						last - first));
		}
		*/

		string  substr(TInt first, TInt length) {
			return string(base_str->Des().Mid(first,
						length));
		}


		string substr(TInt first) {
			return substr(first, length());
		}

		void toupper() {
			base_str->Des().UpperCase();
		}

		void tolower() {
			base_str->Des().LowerCase();
		}

		size_type find(const string& str, size_type pos = 0);
		size_type find(const char ch, size_type pos = 0);

		string& replace(size_type pos, size_type n, const string& str);

		string& operator = (const char* str);
		string& operator = (char ch);
		string& operator = (const string& str);
		string& operator = (const TDesC& str);

		string& operator += (const char* str);
		string& operator += (const char ch);
		string& operator += (const string& str);
		string& operator += (const TDesC& str);

		char operator [] (size_type n) const {
			return (char)(base_str->Des()[n]);
		}

		void SetAt(char Char, size_type pos)
		{
			base_str->Des()[pos] = Char;
		}

		void SetLength(size_type Size);
		void Append(char Char);

		static int ToInteger(string str, int DefaultValue);
		static string IntegerToString(int Value);

	private:
		HBufC* base_str;
		char*  tmp_buff;

		size_type strlen(const char *str);
};
//-----------------------------------------------------------------------------------------
string operator + (string a, string b);
TBool operator < (string a, string b);
TBool operator > (string a, string b);
TBool operator == (string a, string b);
TBool operator != (string a, string b);
//-----------------------------------------------------------------------------------------
inline int SymbianAtoi(const char *Str, int DefaultValue = 0)
{
	return string::ToInteger(string(Str), DefaultValue);
}
//-----------------------------------------------------------------------------------------
/*
inline string SymbianItoa(int Value)
{
	return string::IntegerToString(Value);
}
//-----------------------------------------------------------------------------------------
*/
#endif
