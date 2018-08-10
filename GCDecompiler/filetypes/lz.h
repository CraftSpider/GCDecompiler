
#include <string>
#include "types.h"

namespace types {

class LZ {

	std::string filename;
	int compressed_size, decompressed_size;
	uchar *compressed;
	uchar *decompressed;

	void decompress();
	void compress();

public:

	LZ(const std::string& filename);

	static void decompress(const std::string& file_in, const std::string& file_out = "");
	static void compress(const std::string& file_in, const std::string& file_out = "");

	void write_compressed(const std::string& file_out);
	void write_decompressed(const std::string& file_out);

};

}
