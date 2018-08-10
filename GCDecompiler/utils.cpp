
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "utils.h"
#include "types.h"

// Conversion functions

uint btoi(const uchar *bytes, const uint& len, const Endian& endian) {
	uint out = 0;
	for (uint i = 0; i < len; i++) {
		int multiplier = 1;
		if (endian == BIG) {
			multiplier = len - i - 1;
		} else if (endian == LITTLE) {
			multiplier = i;
		}
		out += bytes[i] << (8 * multiplier);
	}
	return out;
}

uint btoi(const uchar *bytes, const uint& start, const uint& end, const Endian& endian) {
	uint out = 0;
	for (uint i = start; i < end; i++) {
		int multiplier = 1;
		if (endian == BIG) {
			multiplier = end - i - 1;
		} else if (endian == LITTLE) {
			multiplier = i - start;
		}
		out += bytes[i] << (8 * multiplier);
	}
	return out;
}

const uchar* itob(const uint& num, const uint& length) {
	uchar *output = new uchar[length]();
	for (uint i = 0; i < length; i++) {
		output[i] = num >> (8 * (length - i - 1));
	}
	return output;
}

std::string itoh(const uint& num) {
	std::stringstream out;
	out << "0x";
	out << std::hex << std::uppercase << num;
	return out.str();
}

std::string itoh(const int& num) {
	std::stringstream out;
	if (num < 0) {
		out << "-";
	}
	out << "0x";
	out << std::hex << std::uppercase << (num >= 0 ? num : -num);
	return out.str();
}

std::string ctoh(const char& num) {
	std::stringstream out;
	if (num < 16) {
		out << "0";
	}
	out << std::hex << std::uppercase << (int)num;
	return out.str();
}

// Byte Manipulation

bool get_bit(const uchar *chars, const uchar& pos) {
	int loc = (int)floor(pos / 8);
	return chars[loc] & (int)pow(2, 7 - (pos % 8));
}

/**
 * Gets the sum of bits between start and stop, [start, stop]
 */
uint get_range(const uchar *chars, const uchar& start, const uchar& end) {
	uint out = 0;
	char num_bits = (end - start) + 1;
	for (char i = start, j = 1; i <= end; i++, j++) {
		out += (uint)pow(2, (num_bits - j))*get_bit(chars, i);
	}
	return out;
}

int get_signed_range(const uchar *instruction, const uchar& start, const uchar& end) {
	uint value = get_range(instruction, start, end);
	char num_bits = end - start;
	uint mask = 1U << num_bits;
	return -((int)(value & mask)) + (int)(value & (mask - 1));
}

std::string char_format(const uchar *chars, const std::string& to_format) {
	std::stringstream out;
	std::stringstream format;
	bool in_code = false;
	bool mods = true;
	char mod_mask = 0;
	char skip = 0;
	int start = 0, end = 0;
	for (uint i = 0; i < to_format.length(); i++) {
		if (skip > 0) {
			skip--;
		} else if (!in_code && to_format[i] != '{') { // Assuming it's not a format code, just push it to output
			out << to_format[i];
		} else if (!in_code) { // If it's an { then we start processing a format code
			in_code = true;
		} else {
			if (to_format[i] == '}') { // If we find a } then we stop processing a format code, and dump the format result
				in_code = false, mods = true;
				out << format.str();
				format.str("");
				format.clear();
				start = 0, end = 0;
				mod_mask = 0;
				continue;
			} else if (mods && ((to_format[i] >= 'A' && to_format[i] <= 'Z') || (to_format[i] >= 'a' && to_format[i] <= 'z'))) { // If we're in the mod_code section, read in codes.
				if (to_format[i] == 's') {
					mod_mask |= 0b10;
				} else if (to_format[i] == 'a') {
					mod_mask |= 0b1000;
				} else if (to_format[i] == 'x') {
					mod_mask |= 0b1;
				} else if (to_format[i] == 'X') {
					mod_mask |= 0b101;
				} else {
					throw std::runtime_error("Bad Mod Code");
				}
			} else if (mods && (to_format[i] == '|' || mod_mask == 0)) { // If there aren't any mods or we hit section end, move on to next section.
				mods = false;
			}
			if (!mods && to_format[i] != '|') {
				mods = false;
				for (uint j = i; to_format[j] != '}' && to_format[j] != ','; j++) {
					format << to_format[j];
					skip++;
				}
				if (start == 0) {
					format >> start;
					format.str("");
					format.clear();
				} else {
					format >> end;
					format.str("");
					format.clear();
				}
				if (start != 0 && end != 0) {
					uint uresult = get_range(chars, start, end);
					int result = get_signed_range(chars, start, end);
					if (mod_mask & 0b100) {
						format << std::uppercase;
					}
					if (mod_mask & 0b10) {
						if (mod_mask & 0b1) {
							format << itoh(result);
						} else {
							format << result;
						}
					} else if (mod_mask & 0b1000) {
						if (mod_mask & 0b1) {
							format << itoh(std::abs(result));
						} else {
							format << std::abs(result);
						}
					} else {
						if (mod_mask & 0b1) {
							format << itoh(uresult);
						} else {
							format << uresult;
						}
					}
					format << std::nouppercase;
					skip--;
				}
			}
		}
	}
	return out.str();
}

// String Manipulation

bool ends_with(const std::string& val, const std::string& ending) {
	if (ending.size() > val.size()) return false;
	return !val.compare(val.length() - ending.length(), ending.length(), ending);
}

bool is_num(const char& c) {
	return (c >= '0' && c <= '9');
}

bool is_letter(const char& c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool is_hex(const char& c) {
	return is_num(c) || ((c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z'));
}

// File Manipulation

ulong next_long(std::fstream& file, const Endian& endian, const uint& length) {
	uchar *input = new uchar[length]();
	file.read((char*)input, length);
	uint out = btoi(input, length, endian);
	delete[] input;
	return out;
}

uint next_int(std::fstream& file, const Endian& endian, const uint& length) {
	return (uint)next_long(file, endian, length);
}

ushort next_short(std::fstream& file, const Endian& endian, const uint& length) {
	return (ushort)next_long(file, endian, length);
}

uchar next_char(std::fstream& file, const Endian& endian, const uint& length) {
	return (uchar)next_long(file, endian, length);
}

float next_float(std::fstream& file) {
	float data[1];
	file.read(reinterpret_cast<char*>(&data), 4);
	return data[0];
}

void write_int(std::fstream& file, const uint& num, const uint& length) {
	const uchar *to_write = itob(num, length);
	file.write((char*)to_write, length);
	delete[] to_write;
}

void write_string(std::fstream& file, const std::string& out) {
	file.write(out.c_str(), out.length());
}

// Math Operations

uint crc_table[256];
const uint crc_start = 0xFFFFFFFFul;
bool crc_invoked = false;

void gen_crc32_table() {

	for (uint i = 0; i < 256; i++) {

		uint crc = i;

		for (uint j = 0; j < 8; j++) {
			if (crc & 1) crc = (crc >> 1) ^ 0xedb88320; // 0x4C11DB7
			else crc = crc >> 1;
		}

		crc_table[i] = crc; 

	}

}

uint crc32(const uchar *input, const ulong& length) {

	if (!crc_invoked) {
		gen_crc32_table();
		crc_invoked = true;
	}
	
	uint crc = crc_start;

	if (input != nullptr) {
		for (ulong a = 0; a < length; a++) {
			crc = (crc >> 8u) ^ crc_table[(crc ^ input[a]) & 0xFFu];
		}
	}

	return crc ^ crc_start;

}
