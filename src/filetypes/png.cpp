//
// Created by Rune Tynan on 8/11/2018.
//

#include <fstream>
#include <sstream>
#include <algorithm>

#include "at_logging"
#include "at_utils"
#include "zlib.h"
#include "filetypes/png.h"
#include "filetypes/tpl.h"

#define PNG_MAGIC 0x89504E470D0A1A0AL

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("png");

const ColorType ColorType::greyscale = ColorType {0, "Greyscale"};
const ColorType ColorType::truecolor = ColorType {2, "Truecolor"};
const ColorType ColorType::indexed = ColorType {3, "Indexed-color"};
const ColorType ColorType::greyalpha = ColorType {4, "Greyscale w/ Alpha"};
const ColorType ColorType::truealpha = ColorType {6, "Truecolor w/ Alpha"};

bool ColorType::operator==(const types::ColorType &color_type) {
    return this->num == color_type.num;
}

ColorType ColorType::from_depth(uchar depth) {
    switch (depth) {
    case 0:
        return greyscale;
    case 2:
        return truecolor;
    case 3:
        return indexed;
    case 4:
        return greyalpha;
    case 6:
        return truealpha;
    default:
        logger->error("Invalid depth for ColorType");
        return truealpha;
    }
}

Chunk::Chunk(const uint& length, const std::string& type, uchar *data) {
    this->length = length;
    this->type = type;
    this->data = data;
}

Chunk::Chunk(const types::Chunk &chunk) {
    this->length = chunk.length;
    this->type = chunk.type;
    this->data = new uchar[length];
    for (uint i = 0; i < length; ++i) {
        data[i] = chunk.data[i];
    }
}

Chunk::~Chunk() {
    delete[] data;
}

bool Chunk::operator==(const types::Chunk &other) const {
    return other.type == type && other.length == length && util::compare(other.data, data, length);
}

uint Chunk::crc() {
    uchar *crc_data = new uchar[4 + length];
    for (int i = 0; i < 4; i++) {
        crc_data[i] = (uchar)type[i];
    }
    for (ulong i = 0; i < length; i++) {
        crc_data[i + 4] = data[i];
    }
    uint crc = util::crc32(crc_data, length + 4);
    delete[] crc_data;
    return crc;
}

void Chunk::write_chunk(std::fstream &output) {
    util::write_uint(output, length);
    util::write_string(output, type);
    output.write((char*)data, length);
    util::write_uint(output, crc());
}

void PNG::add_chunk(uint length, std::string name, uchar *data) {
    Chunk chunk {length, name, data};
    chunks.push_back(chunk);
}

bool PNG::replace_chunk(uint length, std::string name, uchar* data) {
    Chunk new_chunk {length, name, data};
    for (auto& chunk : chunks) {
        if (chunk.type == name) {
            chunk = new_chunk;
            return true;
        }
    }
    return false;
}

bool PNG::remove_chunk(const types::Chunk &chunk) {
    uint i = chunks.size() + 1;
    for (; i < chunks.size(); ++i) {
        if (chunk == chunks[i]) {
            break;
        }
    }
    if (i < chunks.size() + 1) {
        chunks.erase(chunks.begin() + i);
    }
    return i < chunks.size() + 1;
}

PNG::PNG(const std::string &filename) {
    std::fstream input = std::fstream(filename, ios::binary | ios::in);
    
    uint height = 0, width = 0;
    Color **image_data = nullptr;
    
    if (util::next_ulong(input) != PNG_MAGIC) {
        logger->warn("PNG magic doesn't match expected, file is likely corrupted or wrong type.");
    }
    
    bool first = true;
    while (true) {
        uint length = util::next_uint(input);
        std::string header = util::next_string(input, 4);
        if (first && header != "IHDR") {
            logger->error("First chunk not header chunk, expected IHDR but got " + header);
            break;
        }
        if (header == "IHDR") {
            first = false;
            if (length != 17) {
                logger->error("Incorrect header length");
                break;
            }
            width = util::next_uint(input);
            height = util::next_uint(input);
            image_data = new Color*[height];
            for (uint i = 0; i < height; ++i)
                image_data[i] = new Color[width];
            bit_depth = util::next_uchar(input);
            color_type = ColorType::from_depth(util::next_uchar(input));
            compression = util::next_uchar(input);
            filter = util::next_uchar(input);
            interlace = util::next_uchar(input);
        } else if (header == "PLTE") {
            // TODO
        } else if (header == "IDAT") {
            uchar *data = new uchar[length];
    
            z_stream strm = {};
            strm.next_in = data;
            strm.avail_in = length;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            inflateInit(&strm);
            
            ulong out_size = width*height*4;
            uchar *out_data = new uchar[out_size];
            strm.next_out = out_data;
            strm.avail_out = (uint)out_size;
            int result = inflate(&strm, Z_FINISH);
            if (result != Z_STREAM_END) {
                logger->error("Inflate operation not allocated a large enough array. Please send a bug report to the developers.");
            }
            uint len = (uint)strm.total_out;
            if (color_type == ColorType::truealpha) {
                for (uint i = 0; i < len; i += 4) {
                    image_data[i / width][i % width] = Color {out_data[i], out_data[i + 1], out_data[i + 2], out_data[i + 3]};
                }
            } else {
                logger->error("Unsupported color type for image. Please send a bug report to the developers.");
            }
        } else if (header == "IEND") {
            break;
        } else {
            if (std::islower(header[0])) {
                logger->warn("Unknown ancillary chunk encountered");
            } else if (std::isupper(header[0])) {
                logger->error("PNG encountered unknown critical chunk");
                break;
            } else {
                logger->error("PNG encountered corrupted chunk");
                break;
            }
        }
    }
}

PNG::PNG(const types::Image& image) {
    this->image = image;
    bit_depth = 0;
    color_type = ColorType::truealpha;
    compression = 0;
    filter = 0;
    interlace = 0;
}

Image PNG::get_image() {
    return image;
}

void PNG::add_text(std::string key, std::string content) {
    std::stringstream strm = std::stringstream();
    strm << key;
    strm.write("\0", 1);
    strm << content;
    std::string str = strm.str();
    add_chunk(str.size(), "tEXt", (uchar*)str.c_str());
}

void PNG::update_time(std::time_t timet) {
    
    if (timet == 0) {
        std::chrono::time_point<std::chrono::system_clock> now_d = std::chrono::system_clock::now();
        timet = std::chrono::system_clock::to_time_t(now_d);
    }
    std::tm local = *std::localtime(&timet);
    uint year = (uint)local.tm_year + 1900;
    
    uchar *time = new uchar[7];
    time[0] = year >> 8;
    time[1] = year & 0xFF;
    time[2] = local.tm_mon + 1;
    time[3] = local.tm_mday;
    time[4] = local.tm_hour;
    time[5] = local.tm_min;
    time[6] = local.tm_sec;
    replace_chunk(7, "tIME", time);
}

std::vector<Chunk> PNG::get_chunks() {
    return chunks;
}

std::vector<Chunk> PNG::get_chunks(const std::string& name) {
    std::vector<Chunk> out;
    std::copy_if(chunks.begin(), chunks.end(), std::back_inserter(out), [name](Chunk c){return c.type == name;});
    return out;
}

void PNG::save(const std::string &filename) {
    logger->info("Writing PNG to " + filename);
    
    std::fstream output = std::fstream(filename, ios::out | ios::binary);
    
    output.write(magic, magic_len);
    
    // Write out header
    uchar *ihdr = new uchar[13];
    
    const uchar *width = util::itob(image.width);
    const uchar *height = util::itob(image.height);
    
    for (int i = 0; i < 4; i++) {
        ihdr[i] = width[i];
    }
    delete[] width;
    for (int i = 0; i < 4; i++) {
        ihdr[i + 4] = height[i];
    }
    delete[] height;
    ihdr[8] = bit_depth;
    ihdr[9] = color_type.num;
    ihdr[10] = compression;
    ihdr[11] = filter;
    ihdr[12] = interlace;
    
    Chunk {13, "IHDR", ihdr}.write_chunk(output);
    
    // Write out ancillary chunkks
    for (auto chunk : chunks) {
        chunk.write_chunk(output);
    }
    
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
    Chunk {len, "IDAT", idat}.write_chunk(output);
    delete[] data;
    
    // write out IEND chunk
    Chunk {0, "IEND", nullptr}.write_chunk(output);
}

}
