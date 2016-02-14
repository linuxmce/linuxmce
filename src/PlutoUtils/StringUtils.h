/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file StringUtils.h
 Header file for the StringUtils namespace.
 */
#ifndef STRINGUTILS
#define STRINGUTILS

#ifndef SYMBIAN
	#include <string>
	#include <vector>
	#include <deque>
	#include "CommonIncludes.h"
	using namespace ::std;
#endif //ifndef SYMBIAN

#ifdef ARMV4I
#include "WinCE.h"
#endif

#ifdef SYMBIAN
struct StringUtils
#else

/**
@namespace StringUtils
For String Utilities.
*/
namespace StringUtils
#endif //SYMBIAN
{
    /**
     * @brief breakes the input string into pieces sparated by sTokens; call it repeatedly to get all the pieces
	 * Normally if sTokens is more than 1 character, any character in Token will serve as a delimiter.  If bMultiCharacterTokens=true
	 * then only the full string of tokens will count
     */
#ifdef SYMBIAN
	static
#endif
    string Tokenize( string &sInput, string sTokens, string::size_type &CurPos, bool bMultiCharacterTokens=false);

    /**
     * @brief converts input string to lowercase
     */
#ifdef SYMBIAN
	static
#endif
    string ToLower( const string &sInput );

    /**
     * @brief converts input string to uppercase
     */
#ifdef SYMBIAN
	static
#endif
    string ToUpper( const string &sInput );

    /**
     * @brief returns the string representing the integer
     */
#ifndef SYMBIAN
	string i64tos( u_int64_t iNum );
#endif

#ifdef SYMBIAN
	static
#endif
    string itos( int iNum );
    string ftos( double dNum );

	/**
     * @brief replaces the sSearch string with the sReplace string in the sInput string
     */
#ifdef SYMBIAN
	static
#endif
	// Replaces sSearch with sReplace.  The first one replaces the input string passed in, the 2nd does not
    string Replace( string sInput, const string &sSearch, const string &sReplace );
	string Replace( string *sInput, const string &sSearch, const string &sReplace );

#ifndef SYMBIAN

    /**
     * @brief replaces the sSearch string with the sReplace string in the specified file and returns false if errors
     */
    bool Replace( string sInputFile, string sOutputFile, string sSearch, string sReplace);

    /**
     * @brief breaks the Input string in tokens and putes them in the deque_strings parameter, or a vector
     */
    void Tokenize(string &Input, string Tokens, deque<string> &deque_strings, bool bPushToFront);
	void Tokenize(string &Input, string Tokens, vector<string> &vect_strings);

    /**
     * @brief retrns a sring containing the specified char repeated count times
     */
    string RepeatChar(char c,int count);

    /**
     * @brief returns the string representing the long
     */
    string ltos( long lNum );

    /**
     * @brief eliminates spaces from the begining and the ending of the input string
     */
    string TrimSpaces( string &sInput );

    /**
     * @brief compares strings ignoring the case
     */
    int CompareNoCase( string sFirst, string sSecond );

	/**
     * @brief returns true if sFirst starts with sSecond
     */
    bool StartsWith( const string &sSearchIn, const string &sSearchFor, bool bIgnoreCase=false );

	/**
     * @brief returns true if sFirst ends with sSecond
     */
    bool EndsWith( const string &sSearchIn, const string &sSearchFor, bool bIgnoreCase=false );

	/**
     * @brief returns string with characters removed at the end
     */
    string RemoveStringFromEnd( const string &sBig, int iCutChars);

	/**
     * @brief returns true if sInput contains only white space
     */
    bool OnlyWhiteSpace( const char *pInput );
	bool OnlyWhiteSpace( string &sInput );

	/**
     * @brief returns the value folowing the parameter name in the parameter list tokenized using the delimiters
     */
    string ParseValueFromString( string sParameterList, string sParameterName, string sDefaultValue, const char *pcDelimiters = NULL );

    /**
     * @brief same, but it returns an integer (the parameter is assumed to be a number)
     */
    int ParseIntFromString( string sParameterList, string sParameterName, int iDefaultValue );


    /**
     * @brief converts a printf format string with the parameter list into a string, like this: "%d bla", 12 becomes "12 bla"
     */
    string Format( const char *pcFormat, ... );


	/** Escape control characters.
    Escape tabs, cr/lf, and non-asci characters as '\\t', '\\r', '\\n' and '\\xx' where xx is a 2-digit hex.

	 UnEscape restores the string afterward.
     */
	string Escape(string Input);
	string UnEscape(string Input);

	/**
     * @brief @todo ask
     */
    string EscapeChars( string sInput );


    /**
     * @brief Do a word wrap  on sInput, where the max line length is iNumChars, and the result is vectStrings lines.
     */
    void BreakIntoLines( string sInput, vector<string> *vectStrings, int iNumChars );

    /**
     * @brief converts hex number stored in the first two chars of the parameter to a long and returns it as a char
     */
    char HexByte2Num( char* pcPtr );


    /**
     * @brief assumes the input to have the form: ipaddress:port and does the parsing
     */
    void AddressAndPortFromString( string sInput, int iDefaultPort, string &sAddress, int &iPort );


    /**
     * @brief adds some URL compliancy
     * @todo extend
     */
    string URLEncode( string in );

    /**
     * @brief removes some of the URL special (like %20)
     * @todo extend
     */
    string URLDecode(string in);


    /**
     * @brief the checksum is calulated by adding the values in the char representation of the data
     */
    unsigned int CalcChecksum( unsigned int iSize, char *pData );

	/**
	 * @brief make a mac address from an integer. Usefull to simulate mac addresses from devices
	 */
	string makeUpPlayerAddressFromPlayerId(unsigned int playerId);

    time_t SQLDateTime( string sSQLDate ); /** < converts the sql formatted date time into a time_t structure */
    string SQLDateTime( time_t t=0 ); /** < converts a time_t structure into a sql formatted date time */
    string SQLEscape( string sInput, bool bForLikeStatement = false); /** < convert a escape sequence so that it can be understood by sql */
	string HourMinute(time_t t=0,bool b24Hour=true); /** < converts a time_t structure into a HH:MM formatted date time */

	/** Time Conversion functions for clock display */
	string Hour(time_t t);
	string Minute(time_t t);
	string DayOfWeek(time_t t);
	string DayOfMonth(time_t t);
	string Month(time_t t);

	string GetStringFromConsole(); /** < Let's the user enter a string terminated by a carriage return */

	/** < Convert space delimited string to arguments, like on a command line.  Use " to include spaces
		  There's a hardcoded limit of 500 arguments */
	char **ConvertStringToArgs(string sInput,int &iNumArgs,int *p_iPosNext=NULL);
	void FreeArgs(char **pArgs,int iNumArgs);

	time_t StringToDate(string Value);  /** < Converts a date/time as a string to a time_t */

	string GetDow( int iDow, bool bFull=false );  /** < Returns the name of the day of the week based on the integer.  If bFull, it is not abbreviated */

	// Convert Input to Upper and strip anything but A-Z and 0-9
	string UpperAZ09Only(string sInput);

	string SecondsAsTime(int iSeconds);
	int TimeAsSeconds(string Time);

	string PrecisionTime();
	time_t MakeTime( int Year, int Month, int Day, int Hour=0, int Minute=0, int Second=0 );

	bool ContainsNonAscii(string &sInput);

	/**
	 * @brief escape bash pattern special characters in strings
	 */
	string BashPatternEscape(string sInput, string sCharsToNotEscape = "");

	/**
	 * @brief splits string into elements separated by delimiters
	 */

	vector<string> Split(string sInput, string sDelimiters, bool bRemoveEmptyElements=false);

	string TitleHash(string sTitle); /** Calculate a title hash */

	string StringSHA1(string sData); /** Calculate a SHA1 from a string. */

#endif //#ifndef SYMBIAN
};

#endif //#ifndef _STRINGUTILS
