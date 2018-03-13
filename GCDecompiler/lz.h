
#include <string>
#include "types.h"

using std::string;

class LZ {

public:

	LZ(string filename);

	string dump_all();
	void dump_all(string filename);

};
