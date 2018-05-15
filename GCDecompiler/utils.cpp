
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "utils.h"
#include "types.h"

uint btoi(char *bytes, uint len, Endian endian) {
	uint out = 0;
	for (uint i = 0; i < len; i++) {
		int multiplier;
		if (endian == BIG) {
			multiplier = len - i - 1;
		} else if (endian == LITTLE) {
			multiplier = i;
		}
		out += bytes[i] << (8 * multiplier);
	}
	return out;
}

uint btoi(char *bytes, uint start, uint end, Endian endian) {
	uint out = 0;
	for (uint i = start; i < end; i++) {
		int multiplier;
		if (endian == BIG) {
			multiplier = end - i - 1;
		} else if (endian == LITTLE) {
			multiplier = i - start;
		}
		out += bytes[i] << (8 * multiplier);
	}
	return out;
}

const char* itob(uint num, uint length) {
	char *output = new char[length]();
	for (uint i = 0; i < length; i++) {
		output[i] = num >> (8 * (length - i - 1));
	}
	return output;
}

std::string ctoh(char num) {
	std::stringstream out;
	if (num < 16) {
		out << "0";
	}
	out << std::hex << std::uppercase << (int)num;
	return out.str();
}

std::string itoh(uint num) {
	std::stringstream out;
	out << "0x";
	out << std::hex << std::uppercase << num;
	return out.str();
}

std::string itoh(int num) {
	std::stringstream out;
	if (num < 0) {
		out << "-";
		num = -num;
	}
	out << "0x";
	out << std::hex << std::uppercase << num;
	return out.str();
}

bool get_bit(char *chars, char pos) {
	int loc = (int)floor(pos / 8);
	return chars[loc] & (int)pow(2, 7 - (pos % 8));
}

uint get_range(char *chars, char start, char end) {
	uint out = 0;
	char num_bits = (end - start) + 1;
	for (char i = start, j = 1; i <= end; i++, j++) {
		out += (uint)pow(2, (num_bits - j))*get_bit(chars, i);
	}
	return out;
}

int get_signed_range(char *instruction, char start, char end) {
	uint value = get_range(instruction, start, end);
	char num_bits = end - start;
	uint mask = 1 << num_bits;
	return -((int)(value & mask)) + (int)(value & (mask - 1));
}

std::string char_format(char *chars, std::string to_format) {
	std::stringstream out;
	std::stringstream format;
	bool in_code = false;
	bool mods = true;
	char mod_mask = 0;
	char skip = 0;
	int start = 0, end = 0;
	for (int i = 0; i < to_format.length(); i++) {
		if (skip > 0) {
			skip--;
		} else if (in_code == false && to_format[i] != '{') { // Assuming it's not a format code, just push it to output
			out << to_format[i];
		} else if (in_code == false) { // If it's an { then we start processing a format code
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
			} else if (mods == true && ((to_format[i] >= 'A' && to_format[i] <= 'Z') || (to_format[i] >= 'a' && to_format[i] <= 'z'))) { // If we're in the mod_code section, read in codes.
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
			} else if (mods == true && (to_format[i] == '|' || mod_mask == 0)) { // If there aren't any mods or we hit section end, move on to next section.
				mods = false;
			}
			if (mods == false && to_format[i] != '|') {
				mods = false;
				for (int j = i; to_format[j] != '}' && to_format[j] != ','; j++) {
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

bool ends_with(std::string val, std::string ending) {
	if (ending.size() > val.size()) return false;
	return !val.compare(val.length() - ending.length(), ending.length(), ending);
}

bool is_num(char c) {
	return (c >= '0' && c <= '9');
}

bool is_letter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

uint next_int(std::fstream *file, uint length) {
	char *input = new char[length]();
	file->read(input, length);
	uint out = btoi(input, length);
	delete[] input;
	return out;
}

void write_int(std::fstream *file, uint num, uint length) {
	const char *to_write;
	to_write = itob(num, length);
	file->write(to_write, length);
	delete[] to_write;
}

