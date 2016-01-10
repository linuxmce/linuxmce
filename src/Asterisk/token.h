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
//
// C++ Interface: socket
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTOKEN_H
#define ASTERISKTOKEN_H

#include <map>
#include <string>

namespace ASTERISK { 

/**
@author 
*/
class Token {
public:
    Token();
    virtual ~Token();

public:
    void setKey(std::string key, std::string value);
    std::string& getKey(std::string key);
    bool hasKey(std::string key);

    void removeAll();
   
    virtual std::string serialize() const;
    virtual int unserialize(std::string str);

private:
    std::map<std::string, std::string> tokmap;
};

};

#endif
