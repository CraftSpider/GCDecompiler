#pragma once

#include <string>
#include "types.h"

namespace util {

std::string reverse(const std::string& str);

bool starts_with(const std::string &val, const std::string &start);

bool ends_with(const std::string &val, const std::string &end);

template <Endian E = BIG>
std::string btoh(const uchar* val, const ulong length, const bool prefix = true);

template <Endian E = BIG>
std::string btoh(const char* val, const ulong length, const bool prefix = true);

template <Endian E = BIG>
std::string ltoh(const ulong val, bool prefix = true);

template <Endian E = BIG>
std::string ltoh(const long val, bool prefix = true);

template <Endian E = BIG>
std::string itoh(const uint val, bool prefix = true);

template <Endian E = BIG>
std::string itoh(const int val, bool prefix = true);

template <Endian E = BIG>
std::string stoh(const ushort val, bool prefix = true);

template <Endian E = BIG>
std::string stoh(const short val, bool prefix = true);

template <Endian E = BIG>
std::string ctoh(const uchar val, bool prefix = true);

template <Endian E = BIG>
std::string ctoh(const char val, bool prefix = true);

std::string char_format(const std::string& format, const uchar *data);

}

#include "strmanip.tpp"
