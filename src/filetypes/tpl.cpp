
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>

#include "at_logging"
#include "at_utils"
#include "filetypes/tpl.h"
#include "filetypes/png.h"
#include "zlib.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("tpl");

Color parse_i4(const uchar *block, uchar pixel) {
    uchar tone = (uchar)block[pixel / 2];
    bool which = !(pixel % 2);
    tone = (uchar)(((tone >> 4*which) & 0xF) * 0x11);
    return {tone, tone, tone, 0xFF};
}

Color parse_i8(const uchar *block, uchar pixel) {
    uchar tone = (uchar)block[pixel];
    return {tone, tone, tone, 0xFF};
}

Color parse_rgb565(const uchar *block, uchar pixel) {
    uchar start = (uchar)(pixel * 2);
    uchar rgb[2];
    rgb[0] = block[start];
    rgb[1] = block[start + 1];
    uchar red = (uchar)(0x8 * util::get_range(rgb, 0, 4));
    uchar green = (uchar)(0x4 * util::get_range(rgb, 5, 10));
    uchar blue = (uchar)(0x8 * util::get_range(rgb, 11, 15));
    return {red, green, blue, 0xFF};
}

Color parse_rgb5A3(const uchar *block, uchar pixel) {
    uchar red, green, blue, alpha = 0xFF;
    const uchar *data = block + pixel*2;
    if (!util::get_bit(block, 0)) {
        red = (uchar)(0x11 * util::get_range(data, 4, 7));
        green = (uchar)(0x11 * util::get_range(data, 8, 11));
        blue = (uchar)(0x11 * util::get_range(data, 12, 15));
        alpha = (uchar)(0x20 * util::get_range(data, 1, 3));
        return Color {red, green, blue, alpha};
    } else {
        red = (uchar)(0x8 * util::get_range(data, 1, 5));
        green = (uchar)(0x8 * util::get_range(data, 6, 10));
        blue = (uchar)(0x8 * util::get_range(data, 11, 15));
        return Color {red, green, blue, alpha};
    }
}

// Note: this isn't super efficient as it stands, it recalculates the pallete for every pixel.
// But this way matches with the other formats.
Color parse_cmpr(const uchar *block, uchar pixel) {
    // TODO: Static palette based on block pointer
    // Determine block
    uchar bl_pos = (uchar)(pixel % 8 > 3 ? 1 : 0);
    if (pixel > 31) {
        bl_pos += 2;
    }
    uchar bl_start = bl_pos * 8;
    
    // Get raw ushorts at the positions
    ushort bl_bit = bl_start * 8;
    ushort first, second;
    first = util::get_range(block, bl_bit, bl_bit + 15);
    second = util::get_range(block, bl_bit + 15, bl_bit + 31);
    
    // Read block palette
    Color palette[4];
    palette[0] = parse_rgb565(block, bl_start / 2);
    palette[1] = parse_rgb565(block, (bl_start + 2) / 2);
    if (first >= second) {
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
    uchar *block = new uchar[block_size]();

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
                if (i + (k / block_width) >= height || j + (k % block_width) >= width) {
                    continue;
                }
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
                    case 5:
                        col = parse_rgb5A3(block, k);
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
        }
    }
    delete[] block;
    logger->debug("Parsed " + format_names[format]);
}

TPL::TPL() {
	this->num_images = 0;
	this->images = std::vector<Image*>();
	this->mipmaps = std::vector<uint>();
}

TPL::~TPL() {
    for (auto& image : images) {
        delete[] image;
    }
}

Image TPL::get_image(const uint &index, const uint &mipmap) const {
    return images.at(index)[mipmap];
}

void TPL::add_image(types::Image* image, const uint& mipmaps) {
    images.push_back(image);
    this->mipmaps.push_back(mipmaps);
    generate_table_entries();
}

PNG* TPL::to_png(int index, int mipmap) {
    logger->trace("Converting TPL to PNG");
	Image *image = this->images[index];
    PNG *out = new PNG(image[mipmap]);
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

uint TPL::get_num_mipmaps(const uint &index) const {
    return mipmaps[index];
}

WiiTPL::WiiTPL(std::fstream& input) {

	num_images = util::next_uint(input);
	table_offset = util::next_uint(input);

	input.seekg(this->table_offset);

	// Build image table
    logger->trace("Building image table");
	this->image_table = std::vector<WiiImageTableEntry>();
	for (uint i = 0; i < this->num_images; ++i) {
		uint image_header = util::next_uint(input);
		uint palette_header = util::next_uint(input);
		WiiImageTableEntry entry = {image_header, palette_header};
		this->image_table.push_back(entry);
	}

	// Build images from image table data
	logger->trace("Reading images");
	palette_heads = std::vector<WiiPaletteHeader>();
	image_heads = std::vector<WiiImageHeader>();
	for (auto entry : this->image_table) {
		
		// Build Palette Header
		input.seekg(entry.palette_header);
		ushort entry_count = util::next_ushort(input);
		uchar unpacked = util::next_uchar(input);
		input.seekg(1, ios::cur); // Skip padding
		uint format = util::next_uint(input);
		uint offset = util::next_uint(input);

		WiiPaletteHeader palette = {unpacked, entry_count, format, offset};
		palette_heads.push_back(palette);

		// Build Image Header
		input.seekg(entry.image_header);

		WiiImageHeader image_head {};
		image_head.height = util::next_ushort(input);
		image_head.width = util::next_ushort(input);
		image_head.format = util::next_uint(input);
		image_head.offset = util::next_uint(input);
		image_head.wrap_s = util::next_uint(input);
		image_head.wrap_t = util::next_uint(input);
		image_head.min_filter = util::next_uint(input);
		image_head.max_filter = util::next_uint(input);
		image_head.lod_bias = util::next_float(input);
		image_head.edge_lod_enable = util::next_uchar(input);
		image_head.min_lod = util::next_uchar(input);
		image_head.max_lod = util::next_uchar(input);
		image_head.unpacked = util::next_uchar(input);
		image_heads.push_back(image_head);

		// Now, based on image type, pass the file, header, and palette into the right parser
		Color **image_data = new Color* [image_head.height];
		for (uint i = 0; i < image_head.width; i++)
			image_data[i] = new Color[image_head.width];
		parse_image_data(input, image_head.height, image_head.width, image_head.offset, image_head.format, image_data);

		// Now create an image from that data
		Image *image = new Image(image_head.height, image_head.width, image_data);
		this->images.push_back(image);
	}
    logger->debug("Finished reading TPL");
}

void WiiTPL::generate_table_entries() {
    // TODO
}

void WiiTPL::save(const std::string &filename) const {
    // TODO
}

XboxTPL::XboxTPL(std::fstream &input) {
    // NOTE: Xbox TPL is in opposite endian to Wii and GC. Be careful.
    num_images = util::next_uint<LITTLE>(input);
    
    logger->trace("Building image table");
    image_table = std::vector<XboxImageTableEntry>();
    for (uint i = 0; i < num_images; ++i) {
        uint format = util::next_uint<LITTLE>(input);
        uint offset = util::next_uint<LITTLE>(input);
        ushort width = util::next_ushort<LITTLE>(input);
        ushort height = util::next_ushort<LITTLE>(input);
        ushort mipmaps = util::next_ushort<LITTLE>(input);
        ushort check = util::next_ushort(input);
        if (check != 0x1234) {
            logger->warn("Invalid TPL check. TPL may not load.");
        }
        XboxImageTableEntry entry = {format, offset, width, height, mipmaps};
        image_table.push_back(entry);
    }
    
    logger->trace("Reading images");
    image_heads = std::vector<XboxImageHeader>();
    uint j = 0;
    for (auto entry : image_table) {
        input.seekg(entry.offset);
        
        XboxImageHeader head {};
        head.format = util::next_uint<LITTLE>(input);
        head.width = util::next_ushort<LITTLE>(input);
        util::next_ushort(input);
        head.height = util::next_ushort<LITTLE>(input);
        util::next_ushort(input);
        head.mipmaps = util::next_uint<LITTLE>(input);
        head.compression = util::next_uint<LITTLE>(input);
        head.uncompressed_size = util::next_uint<LITTLE>(input);
        head.unknown_length = util::next_uint<LITTLE>(input);
        util::next_uint(input);
        
        if (head.compression) {
            std::stringstream error;
            error << "Compressed TPL images not yet supported, Image " << j << " can't be processed.";
            logger->error(error.str());
        }
        ++j;
        image_heads.push_back(head);
        
        Color** image_data = new Color*[entry.height];
        for (ushort i = 0; i < entry.height; ++i)
            image_data[i] = new Color[entry.width];
        parse_image_data(input, entry.height, entry.width, entry.offset + 32, entry.format, image_data, LITTLE);
        
        Image *image = new Image(entry.height, entry.width, image_data);
        images.push_back(image);
    }
    logger->debug("Finished reading TPL");
}

void XboxTPL::generate_table_entries() {
    // TODO
}

void XboxTPL::save(const std::string &filename) const {
    // TODO
}

GCTPL::GCTPL(std::fstream& input) {

	num_images = util::next_uint(input);
    
    logger->trace("Building image table");
	image_table = std::vector<GCImageTableEntry>();
	for (uint i = 0; i < num_images; ++i) {
		uint format = util::next_uint(input);
		uint offset = util::next_uint(input);
		ushort width = util::next_ushort(input);
		ushort height = util::next_ushort(input);
		ushort mipmaps = util::next_ushort(input);
		ushort check = util::next_ushort(input);
		if (check != 0x1234) { // TODO: Fix later for other games than SMB2
			logger->warn("Invalid TPL check. TPL may not load.");
		}
		GCImageTableEntry entry = {format, offset, width, height, mipmaps};
		image_table.push_back(entry);
		this->mipmaps.push_back(mipmaps);
	}
    
    logger->trace("Reading images");
	for (auto entry : image_table) {
	    Image *imagelist = new Image[entry.mipmaps];
	    ulong offset = entry.offset;
	    for (uint i = 0; i < entry.mipmaps; ++i) {
	        
	        ushort height = entry.height / (uint)std::pow(2, i);
	        ushort width = entry.width / (uint)std::pow(2, i);
	        
            Color **image_data = new Color *[height];
            for (ushort j = 0; j < height; ++j)
                image_data[j] = new Color[width];
            parse_image_data(input, height, width, offset, entry.format, image_data);
            
            offset = (ulong)input.tellg();
            
            imagelist[i] = Image(height, width, image_data);
        }
        images.push_back(imagelist);
	}
    logger->debug("Finished reading TPL");
}

GCTPL::GCTPL(std::vector<Image*> images) {
    for (auto const &image : images) {
        add_image(image);
    }
}

void GCTPL::generate_table_entries() {
    image_table = std::vector<GCImageTableEntry>();
    for (uint i = 0; i < images.size(); ++i) {
        Image *image = images[i];
        GCImageTableEntry entry {};
        entry.width = image->width;
        entry.height = image->height;
        entry.offset = 0;
        entry.format = 0;  // TODO: correct values for these 2
        entry.mipmaps = mipmaps[i];
        image_table.push_back(entry);
    }
}

void GCTPL::save(const std::string &filename) const {
    
    std::fstream output(filename, ios::binary | ios::out);
    
    util::write_uint(output, num_images);
    for (uint i = 0; i < num_images; ++i) {
        GCImageTableEntry header = image_table[i];
        util::write_uint(output, header.format);
        util::write_uint(output, header.offset);
        util::write_ushort(output, header.width);
        util::write_ushort(output, header.height);
        util::write_ushort(output, header.mipmaps);
        util::write_ushort(output, 0x1234);
    }
    
    for (Image *image : images) {
        // TODO: convert image to charstream
    }
}

TPL* tpl_factory(const std::string& filename) {
    std::fstream input = std::fstream(filename, ios::in | ios::binary);
    if (input.fail()) {
        logger->error("Failed to open TPL file");
        return nullptr;
    }
    
    logger->debug("Parsing TPL");
    if (!util::ends_with(filename, ".tpl")) {
        logger->warn("File " + filename + " is not a TPL, reading will likely fail.");
    }
    
    if (util::next_uint(input) == WiiTPL::IDENTIFIER) {
        return new WiiTPL(input);
    }
    input.seekg(0);
    if (util::next_uint(input) == XboxTPL::IDENTIFIER) {
        return new XboxTPL(input);
    }
    input.seekg(0);
    return new GCTPL(input);
}

}