#pragma once

#include <string>
#include <vector>
#include <map>
#include "types.h"
#include "imagetype.h"

namespace types {

using std::string;

// Forward reference
class PNG;

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
	ushort entry_count;
	uint format, offset;
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

Color parse_i4(const uchar* block, uchar pixel);
Color parse_i8(const uchar* block, uchar pixel);
Color parse_rgb565(const uchar* block, uchar pixel);

Color parse_cmpr(const uchar* block, uchar pixel);

class TPL {

protected:

	uint num_images;
	std::vector<Image*> images;
	std::vector<uint> mipmaps;
	
	virtual void generate_table_entries() = 0;

public:

	TPL();
	virtual ~TPL();
	
	virtual void save(const std::string& filename) const = 0;
	virtual Image get_image(const uint& index, const uint& mipmap = 0) const;
	virtual void add_image(Image* image, const uint& mipmaps = 1);
	virtual PNG* to_png(int index, int mipmap = 0);
	uint get_num_images() const;
	uint get_num_mipmaps(const uint& index) const;

};

class WiiTPL : public TPL {

protected:

	std::vector<WiiImageTableEntry> image_table;
	std::vector<WiiPaletteHeader> palette_heads;
	std::vector<WiiImageHeader> image_heads;
	uint table_offset;
    
    void generate_table_entries() override;

public:
    
    constexpr static uint IDENTIFIER = 0x0020AF30;

	explicit WiiTPL(std::fstream& input);
	WiiTPL(std::vector<Image*> images);
	void save(const std::string& filename) const override;
};

class XboxTPL : public TPL {

protected:
    
    std::vector<XboxImageTableEntry> image_table;
    std::vector<XboxImageHeader> image_heads;
    
    void generate_table_entries() override;
    
public:
    
    constexpr static uint IDENTIFIER = 0x5854504C;
    
    explicit XboxTPL(std::fstream& input);
    XboxTPL(std::vector<Image*> images);
	void save(const std::string& filename) const override;
};

class GCTPL : public TPL {

protected:

	std::vector<GCImageTableEntry> image_table;
	
	void generate_table_entries() override;

public:

	explicit GCTPL(std::fstream& input);
	GCTPL(std::vector<Image*> images);
	void save(const std::string& filename) const override;
};

TPL* tpl_factory(const std::string& filename);

}
