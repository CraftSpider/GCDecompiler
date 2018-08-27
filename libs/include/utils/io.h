#pragma once

#include <fstream>
#include "types.h"

namespace util {

template <Endian E = BIG>
ulong next_ulong(std::istream &file, const ulong length = 8);

template <Endian E = BIG>
uint next_uint(std::istream &file, const ulong length = 4);

template <Endian E = BIG>
ushort next_ushort(std::istream &file, const ulong length = 2);

template <Endian E = BIG>
uchar next_uchar(std::istream &file, const ulong length = 1);

template <Endian E = BIG>
long next_long(std::istream &file, const ulong length = 8);

template <Endian E = BIG>
int next_int(std::istream &file, const ulong length = 4);

template <Endian E = BIG>
short next_short(std::istream &file, const ulong length = 2);

template <Endian E = BIG>
char next_char(std::istream &file, const ulong length = 1);

template <Endian E = BIG>
float next_float(std::istream &file, const ulong length = 4);

template <Endian E = BIG>
double next_double(std::istream &file, const ulong length = 8);

template <Endian E = BIG>
std::string next_string(std::istream &file, const ulong length = 0);

template <Endian E = BIG>
void write_ulong(std::ostream &file, const ulong out, const ulong length = 8);

template <Endian E = BIG>
void write_uint(std::ostream &file, const uint out, const ulong length = 4);

template <Endian E = BIG>
void write_ushort(std::ostream &file, const ushort out, const ulong length = 2);

template <Endian E = BIG>
void write_uchar(std::ostream &file, const uchar out, const ulong length = 1);

template <Endian E = BIG>
void write_long(std::ostream &file, const long out, const ulong length = 8);

template <Endian E = BIG>
void write_int(std::ostream &file, const int out, const ulong length = 4);

template <Endian E = BIG>
void write_short(std::ostream &file, const short out, const ulong length = 2);

template <Endian E = BIG>
void write_char(std::ostream &file, const char out, const ulong length = 1);

template <Endian E = BIG>
void write_float(std::ostream &file, const float out, const ulong length = 4);

template <Endian E = BIG>
void write_double(std::ostream &file, const double out, const ulong length = 8);

template <Endian E = BIG>
void write_string(std::ostream &file, const std::string out, const ulong length = 0);

}

#include "io.tpp"
