#pragma once

#include "types.h"

enum Endian {
	LITTLE,
	BIG
};

// Conversion functions

uint btoi(const char* bytes, const uint& len, const Endian& endian = BIG);
uint btoi(const char* bytes, const uint& start, const uint& end, const Endian& endian = BIG);
const char* itob(const uint& num, const uint& length = 4);
std::string itoh(const uint& num);
std::string itoh(int num);
std::string ctoh(const char& num);

// Byte Manipulation

bool get_bit(const char *chars, const char& pos);
uint get_range(const char *chars, const char& start, const char& end);
int get_signed_range(const char *instruction, const char& start, const char& end);
std::string char_format(char *chars, std::string to_format);

// String Manipulation

bool ends_with(const std::string& val, const std::string& ending);
bool is_num(const char& c);
bool is_letter(const char& c);
bool is_hex(const char& c);

// File Manipulation

ulong next_long(std::fstream& file, const uint& length = 8);
uint next_int(std::fstream& file, const uint& length = 4);
ushort next_short(std::fstream& file, const uint& length = 2);
uchar next_char(std::fstream& file, const uint& length = 1);
float next_float(std::fstream& file);
void write_int(std::fstream *file, const uint& num, const uint& length = 4);
void write_string(std::fstream *file, const std::string& out);

// Math Operations

uint crc32(const char *input, const int& length);