#pragma once

#include "types.h"

namespace util {

bool get_bit(const uchar *data, const ulong pos);

ulong get_range(const uchar *data, const ulong start, const ulong end);

long get_signed_range(const uchar *data, const ulong start, const ulong end);

// Bytes to...

template <Endian E = BIG>
ulong btol(const uchar* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
long btol(const char* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
ulong btol(const uchar* bytes, const ulong length = 8);

template <Endian E = BIG>
long btol(const char* bytes, const ulong length = 8);

template <Endian E = BIG>
uint btoi(const uchar* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
int btoi(const char* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
uint btoi(const uchar* bytes, const ulong length = 4);

template <Endian E = BIG>
int btoi(const char* bytes, const ulong length = 4);

template <Endian E = BIG>
ushort btos(const uchar* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
short btos(const char* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
ushort btos(const uchar* bytes, const ulong length = 2);

template <Endian E = BIG>
short btos(const char* bytes, const ulong length = 2);

template <Endian E = BIG>
uchar btoc(const uchar* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
char btoc(const char* bytes, const ulong start, const ulong end);

template <Endian E = BIG>
uchar btoc(const uchar* bytes, const ulong length = 1);

template <Endian E = BIG>
char btoc(const char* bytes, const ulong length = 1);

// ...to Bytes

template <Endian E = BIG>
uchar* ltob(const ulong val, const ulong length = 8);

template <Endian E = BIG>
char* ltob(const long val, const ulong length = 8);

template <Endian E = BIG>
uchar* itob(const uint val, const ulong length = 4);

template <Endian E = BIG>
char* itob(const int val, const ulong length = 4);

template <Endian E = BIG>
uchar* stob(const ushort val, const ulong length = 2);

template <Endian E = BIG>
char* stob(const short val, const ulong length = 2);

template <Endian E = BIG>
uchar* ctob(const uchar val, const ulong length = 1);

template <Endian E = BIG>
char* ctob(const char val, const ulong length = 1);

}

#include "bytes.tpp"
