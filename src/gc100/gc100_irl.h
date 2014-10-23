#ifndef GC100_IRL_H
#define GC100_IRL_H

#include <string>
using namespace std;

namespace gc100_irl
{
	const string LEARN_PREFIX_IRL = "GC-IRL";
	const string LEARN_PREFIX_IRE = "GC-IRE";

	string IRL_uncompress(const string &IRL_string);
	string IRL_to_pronto(const string &IRL_string);
}

#endif /* GC100_IRL_H */
