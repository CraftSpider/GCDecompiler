namespace util {

template <Endian E>
ulong btol (const uchar* bytes, const ulong length) {
    return btol<E>(bytes, 0, length);
}

template <Endian E>
long btol(const char* bytes, const ulong length) {
    return btol<E>(bytes, 0, length);
}

template <Endian E>
uint btoi(const uchar* bytes, const ulong start, const ulong end) {
    return (uint)btol<E>(bytes, start, end);
}

template <Endian E>
int btoi(const char* bytes, const ulong start, const ulong end) {
    return (uint)btol<E>(bytes, start, end);
}

template <Endian E>
uint btoi(const uchar* bytes, const ulong length) {
    return (uint)btol<E>(bytes, length);
}

template <Endian E>
int btoi(const char* bytes, const ulong length) {
    return (int)btol<E>(bytes, length);
}

template <Endian E>
ushort btos(const uchar* bytes, const ulong start, const ulong end) {
    return (ushort)btol<E>(bytes, start, end);
}

template <Endian E>
short btos(const char* bytes, const ulong start, const ulong end) {
    return (short)btol<E>(bytes, start, end);
}

template <Endian E>
ushort btos(const uchar* bytes, const ulong length) {
    return (ushort)btol<E>(bytes, length);
}

template <Endian E>
short btos(const char* bytes, const ulong length) {
    return (short)btol<E>(bytes, length);
}

template <Endian E>
uchar btoc(const uchar* bytes, const ulong start, const ulong end) {
    return (uchar)btol<E>(bytes, start, end);
}

template <Endian E>
char btoc(const char* bytes, const ulong start, const ulong end) {
    return (char)btol<E>(bytes, start, end);
}

template <Endian E>
uchar btoc(const uchar* bytes, const ulong length) {
    return (uchar)btol<E>(bytes, length);
}

template <Endian E>
char btoc(const char* bytes, const ulong length) {
    return (char)btol<E>(bytes, length);
}

template <Endian E>
uchar* itob(const uint val, const ulong length) {
    return ltob<E>((ulong)val, length);
}

template <Endian E>
char* itob(const int val, const ulong length) {
    return ltob<E>((long)val, length);
}

template <Endian E>
uchar* stob(const ushort val, const ulong length) {
    return ltob<E>((ulong)val, length);
}

template <Endian E>
char* stob(const short val, const ulong length) {
    return ltob<E>((long)val, length);
}

template <Endian E>
uchar* ctob(const uchar val, const ulong length) {
    return ltob<E>((ulong)val, length);
}

template <Endian E>
char* ctob(const char val, const ulong length) {
    return ltob<E>((long)val, length);
}

}