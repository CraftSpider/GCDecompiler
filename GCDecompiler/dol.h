
#include <vector>
#include <string>
#include "types.h"
#include "section.h"

using std::string;

class DOL {

public:
	uint entry_offset, bss_address, bss_size;

	std::vector<Section> sections;

	DOL(string filename);

	string dump_all();
	void dump_all(string filename);

};