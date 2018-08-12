//
// Created by Rune Tynan on 8/11/2018.
//

#include <fstream>
#include <sstream>
#include "logging.h"
#include "png.h"
#include "utils.h"
#include "zlib.h"
#include "tpl.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("png");

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

uint Chunk::crc() {
    uchar *crc_data = new uchar[4 + length];
    for (int i = 0; i < 4; i++) {
        crc_data[i] = (uchar)type[i];
    }
    for (ulong i = 0; i < length; i++) {
        crc_data[i + 4] = data[i];
    }
    uint crc = crc32(crc_data, length + 4);
    delete[] crc_data;
    return crc;
}

void Chunk::write_chunk(std::fstream &output) {
    write_int(output, length);
    write_string(output, type);
    output.write((char*)data, length);
    write_int(output, crc());
}

void PNG::add_chunk(uint length, std::string name, uchar *data) {
    Chunk chunk {length, name, data};
    chunks.push_back(chunk);
}

void PNG::replace_chunk(uint length, std::string name, uchar *data) {
    Chunk chunk {length, name, data};
}

PNG::PNG(const std::string &filename) {
    // TODO: Read from file
}

PNG::PNG(const types::Image& image) {
    this->image = image;
    
    const uchar *width = itob(image.width);
    const uchar *height = itob(image.height);
    uchar depth = '\x08';
    uchar type = '\x06';
    uchar compression = '\x00';
    uchar filter = '\x00';
    uchar interlace = '\x00';
    
    uchar *ihdr = new uchar[13];
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
    add_chunk(13, "IHDR", ihdr);
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
        std::chrono::time_point now_d = std::chrono::system_clock::now();
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

TPL* PNG::to_tpl() {
    // TODO
}

void PNG::save(const std::string &filename) {
    logger->info("Writing PNG to " + filename);
    
    std::fstream output = std::fstream(filename, ios::out | ios::binary);
    
    output.write(magic, magic_len);
    
    // Write out chunks
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
