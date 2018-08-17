
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include "logging.h"
#include "tpl.h"
#include "utils.h"
#include "zlib.h"
#include "png.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("tpl");

Color parse_i4(const uchar *block, const uchar& pixel) {
    uchar tone = (uchar)block[pixel / 2];
    bool which = !(pixel % 2);
    tone = (uchar)(((tone >> 4*which) & 0xF) * 0x11);
    return {tone, tone, tone, 0xFF};
}

Color parse_i8(const uchar *block, const uchar& pixel) {
    uchar tone = (uchar)block[pixel];
    return {tone, tone, tone, 0xFF};
}

Color parse_rgb565(const uchar *block, const uchar& pixel) {
    uchar start = (uchar)(pixel * 2);
    uchar rgb[2];
    rgb[0] = block[start];
    rgb[1] = block[start + 1];
    uchar red = (uchar)(0x8 * get_range(rgb, 0, 4));
    uchar green = (uchar)(0x4 * get_range(rgb, 5, 10));
    uchar blue = (uchar)(0x8 * get_range(rgb, 11, 15));
    return {red, green, blue, 0xFF};
}

Color parse_rgb5A3(const uchar *block, const uchar& pixel) {
    uchar red, green, blue, alpha = 0xFF;
    if (!get_bit(block, 0)) {
        red = (uchar)(0x11 * get_range(block, 4, 7));
        green = (uchar)(0x11 * get_range(block, 8, 11));
        blue = (uchar)(0x11 * get_range(block, 12, 15));
        alpha = (uchar)(0x20 * get_range(block, 1, 3));
        return Color {red, green, blue, alpha};
    } else {
        red = (uchar)(0x8 * get_range(block, 1, 5));
        green = (uchar)(0x8 * get_range(block, 6, 10));
        blue = (uchar)(0x8 * get_range(block, 11, 15));
        return Color {red, green, blue, alpha};
    }
}

// Note: this isn't super efficient as it stands, it recalculates the pallete for every pixel.
// But this way matches with the other formats.
Color parse_cmpr(const uchar *block, const uchar& pixel) {
    // Determine block
    uchar bl_pos = (uchar)(pixel % 8 > 3 ? 1 : 0);
    if (pixel > 31) {
        bl_pos += 2;
    }
    uchar bl_start = bl_pos * 8;
    // Read block palette
    Color palette[4];
    palette[0] = parse_rgb565(block, bl_start / 2);
    palette[1] = parse_rgb565(block, (bl_start + 2) / 2);
    if (palette[0].to_int() > palette[1].to_int()) {
        palette[2] = Color::lerp_colors(palette[0], palette[1], 1.f/3.f);
        palette[3] = Color::lerp_colors(palette[0], palette[1], 2.f/3.f);
    } else {
        palette[2] = Color::lerp_colors(palette[0], palette[1], .5f);
        palette[3] = Color {0, 0, 0, 0};
    }
    // Get palette for pixel in block
    uchar bl_pix = pixel % 32;
    if (bl_pix % 8 >= 4) {
        bl_pix -= 4;
    }
    bl_pix -= (bl_pix / 8) * 4;
    
    uchar index = (uchar)block[(bl_start + 4) + (bl_pix / 4)];
    char which = 3 - (bl_pix % 4);
    index = (index >> 2*which) & 0x3;
    return palette[index];
}

void parse_image_data(std::fstream& input, ushort height, ushort width, uint offset, uint format, Color **image_data, const Endian& endian = BIG) {
    logger->debug("Parsing " + format_names[format]);
    input.seekg(offset);

    ushort block_height = format_heights[format];
    ushort block_width = format_widths[format];
    uchar num_pixels = block_height * block_width;
    uchar block_size = (uchar)(num_pixels * bits_per_pixel[format] / 8);
    uchar *block = new uchar[block_size];

    for (ushort i = 0; i < height; i += block_height) {
        for (ushort j = 0; j < width; j += block_width) {
            
            if (endian == BIG) {
                input.read((char*)block, block_size);
            } else if (endian == LITTLE && format == 14) { // This is a mess of weird math. Hate the xbox CMPR format.
                if (i && !j) {
                    input.seekg((ulong)input.tellg() + width*2);
                }
                input.read((char*)block, block_size / 2);
                input.seekg((ulong)input.tellg() + width*2 - block_size/2);
                input.read((char*)block + block_size / 2, block_size / 2);
                input.seekg((ulong)input.tellg() - (width*2 - block_size/2 + block_size / 2));
                if (j % 2) {
                    input.seekg((ulong)input.tellg() + block_size);
                }
                
                uchar *new_block = new uchar[block_size];
                for (uint k = 0; k < block_size; ++k) {
                    if (k % 8 < 4) {
                        new_block[(k + 1) - ((k % 2) * 2)] = block[k];
                    } else {
                        uchar right = block[k] >> 4;
                        right = ((right & 0b1100) >> 2) + ((right & 0b0011) << 2);
                        uchar left = block[k] & 0xF;
                        left = ((left & 0b1100) >> 2) + ((left & 0b0011) << 2);
                        left = left << 4u;
                        new_block[k] = left + right;
                    }
                }
                delete[] block;
                block = new_block;
            }
            
            for (uchar k = 0; k < num_pixels; ++k) {
                Color col {};
                switch (format) {
                    case 0:
                        col = parse_i4(block, k);
                        break;
                    case 1:
                        col = parse_i8(block, k);
                        break;
                    case 4:
                        col = parse_rgb565(block, k);
                        break;
                    case 14:
                        col = parse_cmpr(block, k);
                        break;
                    default:
                        logger->warn("No pixel parser available");
                        return;
                }
                image_data[i + (k / block_width)][j + (k % block_width)] = col;
            }
            
            delete[] block;
        }
    }
    logger->debug("Parsed " + format_names[format]);
}

WiiImage::WiiImage(types::WiiPaletteHeader palette, types::WiiImageHeader image, types::Color **image_data) : Image(image.height, image.width, image_data) {
	this->palette = palette;
	this->image = image;
}

WiiImage::WiiImage(const types::WiiImage &image) : Image(image) {
	this->palette = image.palette;
	this->image = image.image;
}

XboxImage::XboxImage(XboxImageTableEntry image, XboxImageHeader head, Color **image_data) : Image(image.height, image.width, image_data) {
    this->image = image;
    this->head = head;
}

XboxImage::XboxImage(const types::XboxImage &image) : Image(image) {
    this->image = image.image;
}

GCImage::GCImage(types::GCImageTableEntry image, types::Color **image_data) : Image(image.height, image.width, image_data) {
	this->image = image;
}

GCImage::GCImage(const types::GCImage &image) : Image(image) {
    this->image = image.image;
}

TPL::TPL() {
	this->num_images = 0;
	this->images = std::vector<Image>();
}

PNG* TPL::to_png(const int& index) {
    logger->trace("Converting TPL to PNG");
	Image image = this->images[index];
	PNG* out = new PNG(image);
    out->bit_depth = 8;
    out->color_type = ColorType::truealpha;
    out->compression = 0;
    out->filter = 0;
    out->interlace = 0;
	
	out->update_time();
    std::stringstream title;
    title << "TPL Extract " << index;
    out->add_text("Title", title.str());
    out->add_text("Author", "CraftSpider");
    out->add_text("Software", "GameCube Decompiler");
    logger->trace("PNG converted to TPL successfully");
    return out;
}

uint TPL::get_num_images() const {
    return num_images;
}

WiiTPL::WiiTPL(std::fstream& input) {

	num_images = next_int(input);
	table_offset = next_int(input);

	input.seekg(this->table_offset);

	// Build image table
    logger->trace("Building image table");
	this->image_table = std::vector<WiiImageTableEntry>();
	for (uint i = 0; i < this->num_images; ++i) {
		uint image_header = next_int(input);
		uint palette_header = next_int(input);
		WiiImageTableEntry entry = {image_header, palette_header};
		this->image_table.push_back(entry);
	}

	// Build images from image table data
	logger->trace("Reading images");
	for (auto entry : this->image_table) {
		
		// Build Palette Header
		input.seekg(entry.palette_header);
		uint entry_count = next_short(input);
		uchar unpacked = next_char(input);
		input.seekg(1, ios::cur); // Skip padding
		uint format = next_int(input);
		uint offset = next_int(input);

		WiiPaletteHeader palette = {unpacked, entry_count, format, offset};

		// Build Image Header
		input.seekg(entry.image_header);

		WiiImageHeader image_head {};
		image_head.height = next_short(input);
		image_head.width = next_short(input);
		image_head.format = next_int(input);
		image_head.offset = next_int(input);
		image_head.wrap_s = next_int(input);
		image_head.wrap_t = next_int(input);
		image_head.min_filter = next_int(input);
		image_head.max_filter = next_int(input);
		image_head.lod_bias = next_float(input);
		image_head.edge_lod_enable = next_char(input);
		image_head.min_lod = next_char(input);
		image_head.max_lod = next_char(input);
		image_head.unpacked = next_char(input);

		// Now, based on image type, pass the file, header, and palette into the right parser
		Color **image_data = new Color* [image_head.height];
		for (uint i = 0; i < image_head.width; i++)
			image_data[i] = new Color[image_head.width];
		parse_image_data(input, image_head.height, image_head.width, image_head.offset, image_head.format, image_data);

		// Now create an image from that data
		WiiImage image(palette, image_head, image_data);
		this->images.push_back(image);
	}
    logger->debug("Finished reading TPL");
}

Image WiiTPL::get_image(const uint& index) const {
    return images.at(index);
}

XboxTPL::XboxTPL(std::fstream &input) {
    // NOTE: Xbox TPL is in opposite endian to Wii and GC. Be careful.
    num_images = next_int(input, LITTLE);
    
    logger->trace("Building image table");
    image_table = std::vector<XboxImageTableEntry>();
    for (uint i = 0; i < num_images; ++i) {
        uint format = next_int(input, LITTLE);
        uint offset = next_int(input, LITTLE);
        ushort width = next_short(input, LITTLE);
        ushort height = next_short(input, LITTLE);
        ushort mipmaps = next_short(input, LITTLE);
        ushort check = next_short(input);
        if (check != 0x1234) {
            logger->warn("Invalid TPL check. TPL may not load.");
        }
        XboxImageTableEntry entry = {format, offset, width, height, mipmaps};
        image_table.push_back(entry);
    }
    
    logger->trace("Reading images");
    uint j = 0;
    for (auto entry : image_table) {
        input.seekg(entry.offset);
        
        XboxImageHeader head {};
        head.format = next_int(input, LITTLE);
        head.width = next_short(input, LITTLE);
        next_short(input);
        head.height = next_short(input, LITTLE);
        next_short(input);
        head.mipmaps = next_int(input, LITTLE);
        head.compression = next_int(input, LITTLE);
        head.uncompressed_size = next_int(input, LITTLE);
        head.unknown_length = next_int(input, LITTLE);
        next_int(input);
        
        if (head.compression) {
            std::stringstream error;
            error << "Compressed TPL images not yet supported, Image " << j << " can't be processed.";
            logger->error(error.str());
        }
        ++j;
        
        Color** image_data = new Color*[entry.height];
        for (ushort i = 0; i < entry.height; ++i)
            image_data[i] = new Color[entry.width];
        parse_image_data(input, entry.height, entry.width, entry.offset + 32, entry.format, image_data, LITTLE);
        
        XboxImage image(entry, head, image_data);
        images.push_back(image);
    }
    logger->debug("Finished reading TPL");
}

Image XboxTPL::get_image(const uint &index) const {
    return images.at(index);
}

GCTPL::GCTPL(std::fstream& input) {

	num_images = next_int(input);
    
    logger->trace("Building image table");
	image_table = std::vector<GCImageTableEntry>();
	for (uint i = 0; i < num_images; ++i) {
		uint format = next_int(input);
		uint offset = next_int(input);
		ushort width = next_short(input);
		ushort height = next_short(input);
		ushort mipmaps = next_short(input);
		ushort check = next_short(input);
		if (check != 0x1234) { // TODO: Fix later for other games than SMB2
			logger->warn("Invalid TPL check. TPL may not load.");
		}
		GCImageTableEntry entry = {format, offset, width, height, mipmaps};
		image_table.push_back(entry);
	}
    
    logger->trace("Reading images");
	for (auto entry : image_table) {
	    
		Color **image_data = new Color*[entry.height];
		for (ushort i = 0; i < entry.height; i++)
			image_data[i] = new Color[entry.width];
		parse_image_data(input, entry.height, entry.width, entry.offset, entry.format, image_data);

		GCImage image(entry, image_data);
		images.push_back(image);
	}
    logger->debug("Finished reading TPL");
}

Image GCTPL::get_image(const uint& index) const {
	return images.at(index);
}

TPL* tpl_factory(const std::string& filename) {
    std::fstream input = std::fstream(filename, ios::in | ios::binary);
    if (input.fail()) {
        logger->error("Failed to open TPL file");
    }
    
    logger->debug("Parsing TPL");
    if (!ends_with(filename, ".tpl")) {
        logger->warn("File " + filename + " is not a TPL, reading will likely fail.");
    }
    
    if (next_int(input) == WiiTPL::IDENTIFIER) {
        return new WiiTPL(input);
    }
    input.seekg(0);
    if (next_int(input) == XboxTPL::IDENTIFIER) {
        return new XboxTPL(input);
    }
    input.seekg(0);
    return new GCTPL(input);
}

}