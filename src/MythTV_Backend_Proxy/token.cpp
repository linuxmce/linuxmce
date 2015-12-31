/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "token.h"

#include <iostream>

#define TOKEN_DELIMITER		"[]:[]"

using namespace std;

namespace MYTHTV {

	Token::Token()
	{
	}

	Token::Token(const char* data)
	{
		fillFromData(data);
	}

	Token::~Token()
	{
	}


	int Token::fillFromData(const char* data)
	{
		std::string tokdata(data);

		values_.clear();

		int index = -1, oldindex = 0;
		while((index = tokdata.find(TOKEN_DELIMITER, oldindex)) >= 0)
		{
			values_.push_back(tokdata.substr(oldindex, index - oldindex));
			oldindex = index + sizeof(TOKEN_DELIMITER) / sizeof(char) - 1;
		}

		values_.push_back(tokdata.substr(oldindex, tokdata.length() - oldindex));
		return values_.size();
	}

	int Token::addValue(const char* data)
	{
		values_.push_back(data);
		return values_.size() - 1;
	}

	std::string Token::getValue(int index)
	{
		return values_[index];
	}

	int Token::setValue(int index, const char* data)
	{
		values_[index] = data;
		return index;
	}

	int Token::removeValue(int index)
	{
		return 0;
	}

	int Token::findValue(const char* data)
	{
		for(unsigned int i = 0; i < values_.size(); i++)
			if(values_[i] == data)
				return i;

		return -1;
	}

	int Token::getValuesNum()
	{
		return values_.size();
	}

	std::string Token::Serialize()
	{
		string ret;
		for(unsigned int i = 0; i < values_.size(); i++)
		{
			ret += values_[i];
			if(i < values_.size() - 1)
				ret += TOKEN_DELIMITER;

		}
		return ret;
	}
};
