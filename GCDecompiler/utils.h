#pragma once

#include <string>
#include "types.h"

enum Endian {
	LITTLE,
	BIG
};

// General utility

template<typename T>
void reverse(T* array, ulong length) {
	T* temp = new T[length];
	for (uint i = 0; i < length; ++i) {
		temp[length - i - 1] = array[i];
	}
	for (uint i = 0; i < length; ++i) {
		array[i] = temp[i];
	}
	delete[] temp;
}

void reverse(std::string& str);

template<typename T>
T* endian_convert(T* array, ulong length) {
	// TODO: this is a memory leak
    T* out = new T[length];
    for (uint i = 0; i < length; ++i) {
        out[(i + 1) - ((i % 2) * 2)] = array[i];
    }
    return out;
}

// Conversion functions

uint btoi(const uchar* bytes, const uint& len, const Endian& endian = BIG);
uint btoi(const uchar* bytes, const uint& start, const uint& end, const Endian& endian = BIG);
const uchar* ltob(const ulong& num, const uint& length = 8);
const uchar* itob(const uint& num, const uint& length = 4);
std::string itoh(const uint& num);
std::string itoh(const int& num);
std::string ctoh(const char& num);

// Byte Manipulation

bool get_bit(const uchar *chars, const uchar& pos);
uint get_range(const uchar *chars, const uchar& start, const uchar& end);
int get_signed_range(const uchar *instruction, const uchar& start, const uchar& end);
std::string char_format(const uchar *chars, const std::string& to_format);

// String Manipulation

bool ends_with(const std::string& val, const std::string& ending);
bool is_num(const char& c);
bool is_letter(const char& c);
bool is_lower(const char& c);
bool is_upper(const char& c);
bool is_hex(const char& c);

// File Manipulation

ulong next_long(std::fstream& file, const Endian& endian = BIG, const uint& length = 8);
uint next_int(std::fstream& file, const Endian& endian = BIG, const uint& length = 4);
ushort next_short(std::fstream& file, const Endian& endian = BIG, const uint& length = 2);
uchar next_char(std::fstream& file, const Endian& endian = BIG, const uint& length = 1);
float next_float(std::fstream& file, const Endian& endian = BIG);
std::string next_string(std::fstream& file, const Endian& endian = BIG, const uint& length = 0);
std::string next_string(std::fstream& file, const uint& length = 0);

void write_long(std::fstream& file, const ulong& num, const uint& length = 8);
void write_int(std::fstream& file, const uint& num, const uint& length = 4);
void write_short(std::fstream& file, const ushort& num, const uint& length = 2);
void write_char(std::fstream& file, const uchar& num, const uint& length = 1);
void write_string(std::fstream& file, const std::string& out);

// Math Operations

uint crc32(const uchar *input, const ulong& length);