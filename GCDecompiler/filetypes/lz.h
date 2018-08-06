
#include <string>
#include "types.h"

namespace types {

using std::string;

class LZ {

	string filename;
	int compressed_size, decompressed_size;
	char *compressed;
	char *decompressed;

	void decompress();
	void compress();

public:

	LZ(string filename);

	static void decompress(string file_in, string file_out = "");
	static void compress(string file_in, string file_out = "");

	void write_compressed(string file_out);
	void write_decompressed(string file_out);

};

}
