
#include <fstream>
#include <iostream>
#include <cmath>
#include "logging.h"
#include "tpl.h"
#include "utils.h"
#include "zlib.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("tpl");

Color parse_i4(char *block, char pixel) {
    uchar tone = (uchar)block[pixel / 2];
    bool which = !(pixel % 2);
    tone = (uchar)(((tone >> 4*which) & 0xF) * 0x11);
    return {tone, tone, tone, 0xFF};
}

Color parse_i8(char *block, int pixel) {
    uchar tone = (uchar)block[pixel];
    return {tone, tone, tone, 0xFF};
}

Color parse_cmpr(char *block, int pixel) {
    // TODO:
    // Determine block
    // Read block palette
    // Get palette for pixel in block
    return Color {};
}

void parse_image_data(std::fstream& input, uint height, uint width, int offset, uint format, Color **image_data) {
    logger->trace("Parsing " + format_names[format]);
    input.seekg(offset);
    uchar fheight = format_heights[format];
    uchar fwidth = format_widths[format];
    for (uint i = 0; i < height; i += fheight) {
        for (uint j = 0; j < width; j += fwidth) {
            uchar block_height = (uchar)(height - i);
            if (block_height > fheight)
                block_height = fheight;
            uchar block_width = (uchar)(width - j);
            if (block_width > fwidth)
                block_width = fwidth;
            
            uchar num_pixels = block_height * block_width;
            char block_size = (char)(num_pixels * bits_per_pixel[format] / 8);
            char *block = new char[block_size];
            input.read(block, block_size);
            
            for (char k = 0; k < num_pixels; ++k) {
                Color col {};
                switch (format) {
                    case 0:
                        col = parse_i4(block, k);
                        break;
                    case 1:
                        col = parse_i8(block, k);
                        break;
                    default:
                        logger->warn("No pixel parser available");
                        return;
                }
                image_data[i + (k / fwidth)][j + (k % fwidth)] = col;
            }
        }
    }
    logger->trace("Parsed " + format_names[format]);
}

TPL::TPL() {
	this->num_images = 0;
	this->images = std::vector<Image>();
}

void write_chunk(std::fstream *output, uint len, string type, char *data) {
	write_int(output, len);
	write_string(output, type);
	output->write(data, len);
	
	char *crc_data = new char[4 + len];
	for (int i = 0; i < 4; i++) {
		crc_data[i] = type[i];
	}
	for (uint i = 0; i < len; i++) {
		crc_data[i + 4] = data[i];
	}
	uint crc = crc32(crc_data, len + 4);

	write_int(output, crc);
}

void TPL::to_png(const int& index, const string& filename) {
    logger->info("Writing PNG to " + filename);
    
	Image image = this->images[index];

	std::fstream output_r(filename, ios::out | ios::binary);
	std::fstream *output = &output_r;

	const char magic[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	output->write(magic, 8);

	const char *height = itob(image.height);
	const char *width = itob(image.width);
	char depth = '\x08';
	char type = '\x06';
	char compression = '\x00';
	char filter = '\x00';
	char interlace = '\x00';

	char ihdr[13];
	for (int i = 0; i < 4; i++) {
		ihdr[i] = height[i];
	}
	delete[] height;
	for (int i = 0; i < 4; i++) {
		ihdr[i + 4] = width[i];
	}
	delete[] width;
	ihdr[8] = depth;
	ihdr[9] = type;
	ihdr[10] = compression;
	ihdr[11] = filter;
	ihdr[12] = interlace;

	write_chunk(output, 13, "IHDR", ihdr);
 
	uint line_width = image.width * 4 + 1;
	uint in_size = image.height * line_width;
	uchar *data = new uchar[in_size];
	
	for (uint i = 0; i < image.height; ++i) {
	    data[i * line_width] = 0;
	    for (uint j = 0; j < image.width; ++j) {
	        Color col = image.image_data[i][j];
	        int pos = (j * 4 + i * line_width) + 1;
	        data[pos] = col.R;
	        data[pos + 1] = col.G;
	        data[pos + 2] = col.B;
	        data[pos + 3] = col.A;
	    }
	}
	
	z_stream strm = {};
	strm.next_in = data;
	strm.avail_in = in_size;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	ulong out_size = deflateBound(&strm, strm.avail_in);
	uchar *idat = new uchar[out_size];
	strm.next_out = idat;
	strm.avail_out = (uint)out_size;
	deflate(&strm, Z_FINISH);
	uint len = (uint)strm.total_out;
	write_chunk(output, len, "IDAT", (char*)idat);

	write_chunk(output, 0, "IEND", nullptr);
	
	output->close();
    
    logger->info("PNG " + filename + " written successfully");
}

uint TPL::get_num_images() const {
    return num_images;
}

WiiTPL::WiiTPL(string filename) {

	std::fstream input = std::fstream(filename, ios::in | ios::binary);
	if (input.fail()) {
	    logger->error("Failed to open TPL file");
	}

	identifier = next_int(input);
	num_images = next_int(input);
	table_offset = next_int(input);

	input.seekg(this->table_offset);

	// Build image table
	this->image_table = std::vector<WiiImageTableEntry>();
	for (uint i = 0; i < this->num_images; ++i) {
		uint image_header = next_int(input);
		uint palette_header = next_int(input);
		WiiImageTableEntry entry = {image_header, palette_header};
		this->image_table.push_back(entry);
	}

	// Build images from image table data
	for (auto entry : this->image_table) {
		
		// Build Palette Header
		input.seekg(entry.palette_header);
		uint entry_count = next_short(input);
		char unpacked = next_char(input);
		input.seekg(1, ios::cur); // Skip padding
		uint format = next_int(input);
		uint offset = next_int(input);

		WiiPaletteHeader palette = {unpacked, entry_count, format, offset};

		// Build Image Header
		input.seekg(entry.image_header);
		uint height = next_short(input);
		uint width = next_short(input);
		format = next_int(input);
		offset = next_int(input);
		uint wrap_s = next_int(input);
		uint wrap_t = next_int(input);
		uint min_filter = next_int(input);
		uint max_filter = next_int(input);
		float lod_bias = next_float(input);
		char edge_lod = next_char(input);
		char min_lod = next_char(input);
		char max_lod = next_char(input);
		unpacked = next_char(input);

		WiiImageHeader image_head {};
		image_head.height = height;
		image_head.width = width;
		image_head.format = format;
		image_head.offset = offset;
		image_head.wrap_s = wrap_s;
		image_head.wrap_t = wrap_t;
		image_head.min_filter = min_filter;
		image_head.max_filter = max_filter;
		image_head.lod_bias = lod_bias;
		image_head.edge_lod_enable = edge_lod;
		image_head.min_lod = min_lod;
		image_head.max_lod = max_lod;
		image_head.unpacked = unpacked;

		// Now, based on image type, pass the file, header, and palette into the right parser
		Color **image_data = new Color* [height];
		for (uint i = 0; i < width; i++)
			image_data[i] = new Color[width];
		parse_image_data(input, image_head.height, image_head.width, image_head.offset, image_head.format, image_data);

		// Now create an image from that data
		WiiImage image(palette, image_head, image_data);
		this->images.push_back(image);
	}
}

GCTPL::GCTPL(const string& filename) {
    logger->trace("Reading TPL");
    if (!ends_with(filename, ".tpls")) {
        logger->warn("File " + filename + " is not a TPL, reading will likely fail.");
    }

	std::fstream input = std::fstream(filename, ios::in | ios::binary);
	
	if (input.fail()) {
	    logger->error("Failed to open TPL file");
	}

	this->num_images = next_int(input);
    
    logger->trace("Building image table");
	this->image_table = std::vector<GCImageTableEntry>();
	for (uint i = 0; i < this->num_images; ++i) {
		uint format = next_int(input);
		uint offset = next_int(input);
		uint width = next_short(input);
		uint height = next_short(input);
		uint level_count = next_short(input);
		uint check = next_short(input);
		if (check != 0x1234) { // TODO: Fix later for other games than SMB2
			throw std::runtime_error("Invalid TPL Header");
		}
		GCImageTableEntry entry = {format, offset, width, height, level_count};
		this->image_table.push_back(entry);
	}
    
    logger->trace("Reading images");
	for (auto entry : this->image_table) {

		Color **image_data = new Color*[entry.height];
		for (uint i = 0; i < entry.height; i++)
			image_data[i] = new Color[entry.width];
		parse_image_data(input, entry.height, entry.width, entry.offset, entry.format, image_data);

		GCImage image(entry, image_data);
		this->images.push_back(image);
	}
    
    logger->trace("Finished reading TPL");
}

Image WiiTPL::get_image(const uint& index) const {
	return this->images.at(index);
}

Image GCTPL::get_image(const uint& index) const {
	return this->images.at(index);
}

}