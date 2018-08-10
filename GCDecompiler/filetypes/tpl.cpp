
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include "logging.h"
#include "tpl.h"
#include "utils.h"
#include "zlib.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("tpl");

Color parse_i4(const uchar *block, const uchar& pixel) {
    uchar tone = (uchar)block[pixel / 2];
    bool which = !(pixel % 2);
    tone = ((tone >> 4*which) & 0xF) * 0x11;
    return {tone, tone, tone, 0xFF};
}

Color parse_i8(const uchar *block, const uchar& pixel) {
    uchar tone = (uchar)block[pixel];
    return {tone, tone, tone, 0xFF};
}

Color parse_rgb565(const uchar *block, const uchar& pixel) {
    uchar start = pixel * 2;
    uchar rgb[2];
    rgb[0] = block[start];
    rgb[1] = block[start + 1];
    uchar red = 0x8 * get_range(rgb, 0, 4);
    uchar green = 0x4 * get_range(rgb, 5, 10);
    uchar blue = 0x8 * get_range(rgb, 11, 15);
    return {red, green, blue, 0xFF};
}

Color parse_rgb5A3(const uchar *block, const uchar& pixel) {
    // TODO
    return Color {};
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
    uchar fheight = format_heights[format];
    uchar fwidth = format_widths[format];
    for (ushort i = 0; i < height; i += fheight) {
        for (ushort j = 0; j < width; j += fwidth) {
            ushort block_height = height - i;
            if (block_height > fheight)
                block_height = fheight;
            ushort block_width = width - j;
            if (block_width > fwidth)
                block_width = fwidth;
            
            uchar num_pixels = block_height * block_width;
            uchar block_size = (uchar)(num_pixels * bits_per_pixel[format] / 8);
            uchar *block = new uchar[block_size];
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
                image_data[i + (k / fwidth)][j + (k % fwidth)] = col;
            }
            
            delete[] block;
        }
    }
    logger->debug("Parsed " + format_names[format]);
}

void write_png_chunk(std::fstream &output, ulong len, string type, const uchar *data) {
    write_int(output, len);
    write_string(output, type);
    output.write((const char*)data, len);
    
    uchar *crc_data = new uchar[4 + len];
    for (int i = 0; i < 4; i++) {
        crc_data[i] = (uchar)type[i];
    }
    for (ulong i = 0; i < len; i++) {
        crc_data[i + 4] = data[i];
    }
    uint crc = crc32(crc_data, len + 4);
    delete[] crc_data;
    
    write_int(output, crc);
}

uint Color::to_int() {
    uint out = 0;
    out += R << 24U;
    out += G << 16U;
    out += B << 8U;
    out += A;
    return out;
}

Color Color::lerp_colors(const types::Color &a, const types::Color &b, const float &factor) {
    return Color {
        (uchar)(a.R + (b.R - a.R) * factor),
        (uchar)(a.G + (b.G - a.G) * factor),
        (uchar)(a.B + (b.B - a.B) * factor),
        (uchar)(a.A + (b.A - a.A) * factor)
    };
}

Image::Image() {
	this->image_data = nullptr;
}

Image::Image(uint height, uint width, types::Color **image_data) {
	this->height = height;
	this->width = width;
	this->image_data = image_data;
}

Image::Image(const types::Image &image) {
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

Image::~Image() {
	for (uint i = 0; i < this->height; i++)
		delete[] this->image_data[i];
	delete[] this->image_data;
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

void TPL::to_png(const int& index, const string& filename) {
    logger->info("Writing PNG to " + filename);
    
	Image image = this->images[index];

	std::fstream output(filename, ios::out | ios::binary);
 
	// Write out magic header data
	const char magic[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	output.write(magic, 8);
	
	// Write out IHDR chunk
	const uchar *width = itob(image.width);
	const uchar *height = itob(image.height);
	uchar depth = '\x08';
	uchar type = '\x06';
	uchar compression = '\x00';
	uchar filter = '\x00';
	uchar interlace = '\x00';

	uchar ihdr[13];
	for (int i = 0; i < 4; i++) {
		ihdr[i] = width[i];
	}
	delete[] width;
	for (int i = 0; i < 4; i++) {
		ihdr[i + 4] = height[i];
	}
	delete[] height;
	ihdr[8] = depth;
	ihdr[9] = type;
	ihdr[10] = compression;
	ihdr[11] = filter;
	ihdr[12] = interlace;
    
    write_png_chunk(output, 13, "IHDR", ihdr);
 
	// Write out IDAT chunk
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
    write_png_chunk(output, len, "IDAT", idat);
	delete[] data;
	delete[] idat;
	
	// Write out tIME chunk
	uchar *time = new uchar[7];
	std::chrono::time_point now_d = std::chrono::system_clock::now();
	std::time_t now = std::chrono::system_clock::to_time_t(now_d);
	std::tm local = *std::localtime(&now);
	uint year = (uint)local.tm_year + 1900;
	time[0] = year >> 8;
	time[1] = year & 0xFF;
	time[2] = local.tm_mon + 1;
	time[3] = local.tm_mday;
	time[4] = local.tm_hour;
	time[5] = local.tm_min;
	time[6] = local.tm_sec;
    write_png_chunk(output, 7, "tIME", time);
	delete[] time;
 
	// Write out tEXt chunks
    {
        using namespace std::string_literals;
        std::stringstream s_title;
        s_title << "Title\0TPL Extract "s << index;
        std::string title = s_title.str();
        write_png_chunk(output, title.size(), "tEXt", (uchar *) title.c_str());
        std::string author = "Author\0CraftSpider"s;
        write_png_chunk(output, author.size(), "tEXt", (uchar *) author.c_str());
        std::string software = "Software\0GameCube Decompiler"s;
        write_png_chunk(output, software.size(), "tEXt", (uchar *) software.c_str());
    }
	
	// Write out IEND chunk
    write_png_chunk(output, 0, "IEND", nullptr);
	
	output.close();
    
    logger->info("PNG " + filename + " written successfully");
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