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
	uchar edge_lod_enable, min_lod, max_lod, unpacked;
    ushort height, width;
	uint format, offset, wrap_s, wrap_t, min_filter, max_filter;
	float lod_bias;
};

//XBox TPL, for SMB deluxe etc.
struct XboxImageTableEntry {
    uint format, offset;
    ushort width, height, mipmaps;
};

struct XboxImageHeader {
    ushort width, height;
    uint format, mipmaps, compression, uncompressed_size, unknown_length;
};

// GC TPL stuff. SMB2 etc
struct GCImageTableEntry {
	uint format, offset;
    ushort width, height, mipmaps;
};

struct Color {
	uchar R, G, B, A;
	
	uint to_int();
	
	static Color lerp_colors(const Color& a, const Color& b, const float& factor);
};

class Image {

public:

	uint height, width;
	Color **image_data;

	Image();

	Image(uint height, uint width, Color **image_data);

	Image(const Image &image);

	~Image();
};

class WiiImage : public Image {

public:

	WiiPaletteHeader palette;
	WiiImageHeader image;

	WiiImage(WiiPaletteHeader palette, WiiImageHeader image, Color **image_data);

	WiiImage(const WiiImage &image);

};

class XboxImage : public Image {

public:
    
    XboxImageTableEntry image;
    XboxImageHeader head;
    
    XboxImage(XboxImageTableEntry image, XboxImageHeader head, Color **image_data);
    
    XboxImage(const XboxImage& image);
    
};

class GCImage : public Image {

public:

	GCImageTableEntry image;

	GCImage(GCImageTableEntry image, Color **image_data);
	
	GCImage(const GCImage& image);

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
	uint table_offset;

public:
    
    const static uint IDENTIFIER = 0x0020AF30;

	explicit WiiTPL(std::fstream& input);
	
	Image get_image(const uint& index) const override;

};

class XboxTPL : public TPL {
    
    std::vector<XboxImageTableEntry> image_table;
    
public:
    
    const static uint IDENTIFIER = 0x5854504C;
    
    explicit XboxTPL(std::fstream& input);
    
    Image get_image(const uint& index) const override;
    
};

class GCTPL : public TPL {

	std::vector<GCImageTableEntry> image_table;

public:

	explicit GCTPL(std::fstream& input);

	Image get_image(const uint& index) const override;

};

TPL* tpl_factory(const std::string& filename);

}
