#pragma once

#include "types.h"

enum Endian {
	LITTLE,
	BIG
};

uint btoi(char *bytes, uint len, Endian endian = BIG);
uint btoi(char *bytes, uint start, uint end, Endian endian = BIG);
const char* itob(uint num, uint length = 4);
std::string itoh(uint num);
std::string itoh(int num);
std::string ctoh(char num);

bool get_bit(char *instruction, char pos);
uint get_range(char *instruction, char start, char end);
int get_signed_range(char *instruction, char start, char end);
std::string char_format(char *chars, std::string to_format);

bool ends_with(std::string val, std::string ending);
bool is_num(char c);
bool is_letter(char c);

uint next_int(std::fstream *file, uint length = 4);
void write_int(std::fstream *file, uint num, uint length = 4);
