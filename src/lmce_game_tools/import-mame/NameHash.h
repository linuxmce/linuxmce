/**
 * NameHash - Generate a namehash given input
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef NAMEHASH_H
#define NAMEHASH_H

#include <string>

using namespace std;

namespace NameHash
{
  string NormalizeString(string sString);
  string FromString(string sString);
}

#endif /* NAMEHASH_H */
