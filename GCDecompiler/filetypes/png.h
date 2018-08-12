//
// Created by Rune Tynan on 8/11/2018.
//

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "image.h"

namespace types {

class TPL;

struct Chunk {
    uint length;
    std::string type;
    uchar* data;
    
    Chunk(const uint& length, const std::string& type, uchar* data);
    Chunk(const Chunk& chunk);
    ~Chunk();
    
    uint crc();
    
    void write_chunk(std::fstream& output);
};

class PNG {
    
    Image image;
    std::vector<Chunk> chunks;
    
    void add_chunk(uint length, std::string name, uchar* data);
    
    void replace_chunk(uint length, std::string name, uchar* data);

public:
    
    constexpr static uint magic_len = 8;
    constexpr static char magic[magic_len] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    
    PNG(const std::string& filename);
    PNG(const Image& image);
    
    Image get_image();
    
    void add_text(std::string key, std::string content);
    
    void update_time(std::time_t time = 0);
    
    std::vector<Chunk> get_chunks(std::string name);
    
    void remove_chunk(const Chunk& chunk);
    
    TPL* to_tpl();
    
    void save(const std::string& filename);
    
};

}
