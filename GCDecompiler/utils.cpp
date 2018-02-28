
#include <string>
#include <fstream>
#include <sstream>
#include "types.h"
#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

uint btoi(char *bytes, uint len) {
	uint out = 0;
	for (uint i = 0; i < len; i++) {
		out += bytes[i] << (8 * (len - i - 1));
	}
	return out;
}

const char* itob(uint num, uint length = 4) {
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

uint next_int(std::fstream *file, uint length) {
	char *input = new char[length]();
	file->read(input, length);
	uint out = btoi(input, length);
	delete[] input;
	return out;
}

void write_int(std::fstream *file, uint num, uint length = 4) {
	const char *to_write;
	to_write = itob(num, length);
	file->write(to_write, length);
	delete[] to_write;
}

void create_directory(std::string name) {
	const char *path = name.c_str();
	#if defined(_WIN32) || defined(WIN32)
	CreateDirectory(path, nullptr);
	#else
	mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#endif
}