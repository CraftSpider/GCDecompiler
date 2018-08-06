#pragma once

#include <string>
#include <vector>
#include <map>
#include "types.h"

namespace types {

using std::string;

static std::map<int, string> format_names = {
        {0, "I4"}, {1, "I8"}, {2, "IA4"}, {3, "IA8"}, {4, "RGB565"}, {5, "RGB5A3"}, {6, "RGBA32"}, {8, "C4"}, {9, "C8"},
        {10, "C14x2"}, {14, "CMPR"}
};

static std::map<int, uchar> bits_per_pixel = {
        {0, 4}, {1, 8}, {2, 8}, {3, 16}, {4, 16}, {5, 16}, {6, 32}, {8, 4}, {9, 8}, {10, 16}, {14, 4}
};

static std::map<int, uchar> format_heights = {
        {0, 8}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {8, 8}, {9, 4}, {10, 4}, {14, 8}
};

static std::map<int, uchar> format_widths = {
        {0, 8}, {1, 8}, {2, 8}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {8, 8}, {9, 8}, {10, 4}, {14, 8}
};

//  Mario Kart/Wii games, possibly useful later
struct WiiImageTableEntry {
	uint image_header, palette_header;
};

struct WiiPaletteHeader {
	char unpacked;
	uint entry_count, format, offset;
};

struct WiiImageHeader {
	char edge_lod_enable, min_lod, max_lod, unpacked;
	uint height, width, format, offset, wrap_s, wrap_t, min_filter, max_filter;
	float lod_bias;
};

// GC TPL stuff. SMB2 etc
struct GCImageTableEntry {
	uint format, offset, width, height, level_count;
};

struct Color {
	uchar R, G, B, A;
};

class Image {

public:

	uint height, width;
	Color **image_data;

	Image() {
		this->image_data = nullptr;
	}

	Image(uint height, uint width, Color **image_data) {
		this->height = height;
		this->width = width;
		this->image_data = image_data;
	}

	Image(const Image &image) {
		this->height = image.height;
		this->width = image.width;
		this->image_data = new Color* [this->height];
		for (uint i = 0; i < this->height; i++) {
			this->image_data[i] = new Color[this->width];
		}
		for (uint i = 0; i < this->height; i++) {
			for (uint j = 0; j < this->width; j++) {
				this->image_data[i][j] = image.image_data[i][j];
			}
		}
	}

	~Image() {
		for (uint i = 0; i < this->height; i++)
			delete[] this->image_data[i];
		delete[] this->image_data;
	}
};

class WiiImage : public Image {

public:

	WiiPaletteHeader palette;
	WiiImageHeader image;

	WiiImage(WiiPaletteHeader palette, WiiImageHeader image, Color **image_data) : Image(image.height, image.width, image_data) {
		this->palette = palette;
		this->image = image;
	}

	WiiImage(const WiiImage &image) : Image(image) {
		this->palette = image.palette;
		this->image = image.image;
	}

};

class GCImage : public Image {

public:

	GCImageTableEntry image;

	GCImage(GCImageTableEntry image, Color **image_data) : Image(image.height, image.width, image_data){
		this->image = image;
	}

};

class TPL {

protected:

	uint num_images;
	std::vector<Image> images;

public:

	TPL();
 
	virtual Image get_image(const uint& index) const = 0;
	virtual void to_png(const int& index, const string& filename);
	uint get_num_images() const;

};

class WiiTPL : public TPL {

	std::vector<WiiImageTableEntry> image_table;
	uint identifier, table_offset;

public:

	explicit WiiTPL(string filename);
	
	Image get_image(const uint& index) const override;

};

class GCTPL : public TPL {

	std::vector<GCImageTableEntry> image_table;

public:

	explicit GCTPL(const string& filename);

	Image get_image(const uint& index) const override;

};

}
