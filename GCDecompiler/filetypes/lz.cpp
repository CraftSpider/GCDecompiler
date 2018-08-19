
#include <string>
#include <fstream>
#include "utils.h"
#include "lz.h"
#include "logging.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("lz");

LZ::LZ(const std::string& filename) {
	logger->debug("Parsing LZ");

	this->filename = filename;
	this->compressed = nullptr;
	this->decompressed = nullptr;
	this->compressed_size = 0;
	this->decompressed_size = 0;

	std::fstream input(filename, ios::in | ios::binary | ios::ate);
	uint size = (uint)input.tellg();
	input.seekg(0, ios::beg);
	uchar *data = new uchar[size];
	input.read((char*)data, size);
	if (ends_with(filename, ".lz")) {
		// Compressed file. Fill compressed.
		this->compressed_size = size;
		this->compressed = data;
	} else {
		// Uncompressed File
		this->decompressed_size = size;
		this->decompressed = data;
	}
	
	logger->debug("Finished parsing LZ");
}

void LZ::decompress() {
	logger->info("Decompressing LZ" + filename);

	uint datapos = 0;

//	uint file_size = btoi(compressed, datapos, datapos + 4, LITTLE);
	datapos += 4;
	uint data_size = btoi(compressed, datapos, datapos + 4, LITTLE);
	datapos += 4;

	uint mempos = 0;

	uchar *data = new uchar[data_size]();

	while (mempos < data_size) {

		char block = btoi(this->compressed, datapos, datapos + 1);
		datapos += 1;

		for (int i = 0; i < 8 && mempos < data_size; ++i) {
			if (block & 0x01) { // Copy data raw
				data[mempos++] = btoi(this->compressed, datapos, datapos + 1);
				datapos += 1;
			} else { // Unpack reference
				uint reference = btoi(this->compressed, datapos, datapos + 2, BIG);
				datapos += 2;

				int length = (reference & 0x000F) + 3;
				int offset = ((reference & 0xFF00) >> 8) | ((reference & 0x00F0) << 4);

				int back_set = (mempos - 18 - offset) & 0xFFF;
				int read_pos = mempos - back_set;

				if (read_pos < 0) {
					int amt = -read_pos;
					if (length <= amt) {
						amt = length;
					}

					length -= amt;
					read_pos += amt;
					mempos += amt;
				}

				while (length-- > 0) {
					data[mempos++] = data[read_pos++];
				}
			}
			block = block >> 1;
		}
	}

	this->decompressed_size = data_size;
	this->decompressed = data;
	
	logger->info("Finished decompressing LZ");
}

void LZ::compress() {
	// TODO: Compression algorithm
}

void LZ::decompress(const std::string& file_in, const std::string& file_out) {
	LZ lz = LZ(file_in);
	lz.write_decompressed(file_out);
}

void LZ::compress(const std::string& file_in, const std::string& file_out) {
	LZ lz = LZ(file_in);
	lz.write_compressed(file_out);
}

void LZ::write_decompressed(const std::string& filename) {
	if (this->decompressed == nullptr && this->decompressed_size == 0) {
		this->decompress();
	}

	std::fstream output(filename, ios::out | ios::binary);
	output.write((char*)this->decompressed, this->decompressed_size);
}

void LZ::write_compressed(const std::string& filename) {
	if (this->compressed == nullptr && this->compressed_size == 0) {
		this->compress();
	}

	std::fstream output(filename, ios::out | ios::binary);
	output.write((char*)this->compressed, this->compressed_size);
}

}