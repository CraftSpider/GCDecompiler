
#include <sstream>
#include "bytes.h"
#include "arrays.h"

namespace util {

template <Endian E>
std::string btoh(const uchar* val, const ulong length, const bool prefix) {
    std::stringstream out;
    if (prefix)
        out << "0x";
    out << std::hex << std::uppercase;
    if (E == LITTLE) {
        for (ulong i = length - 1; i < length; --i) {
            if (val[i] < 0x10)
                out << "0";
            out << (uint)val[i];
        }
    } else {
        for (ulong i = 0; i < length; ++i) {
            if (val[i] < 0x10)
                out << "0";
            out << (uint)val[i];
        }
    }
    return out.str();
}

template <Endian E>
std::string btoh(const char* val, const ulong length, const bool prefix) {
    std::stringstream out;
    if (prefix)
        out << "0x";
    out << std::hex << std::uppercase;
    if (E == LITTLE) {
        for (ulong i = length - 1; i < length; --i) {
            uchar outc = (uchar)val[i];
            if (outc < 0x10)
                out << "0";
            out << (uint)outc;
        }
    } else {
        for (ulong i = 0; i < length; ++i) {
            uchar outc = (uchar)val[i];
            if (outc < 0x10)
                out << "0";
            out << (uint)outc;
        }
    }
    return out.str();
}

template <Endian E>
std::string ltoh(ulong val, bool prefix) {
    uchar* temp = ltob(val);
    std::string out = btoh<E>(temp, 8, prefix);
    delete[] temp;
    return out;
}

template <Endian E>
std::string ltoh(long val, bool prefix) {
    std::stringstream out;
    if (val < 0) {
        out << "-";
        val = -val;
    }
    char* temp = ltob(val);
    out << btoh<E>(temp, 8, prefix);
    delete[] temp;
    return out.str();
    
}

template <Endian E>
std::string itoh(uint val, bool prefix) {
    uchar* temp = itob(val);
    std::string out = btoh<E>(temp, 4, prefix);
    delete[] temp;
    return out;
}

template <Endian E>
std::string itoh(int val, bool prefix) {
    std::stringstream out;
    if (val < 0) {
        out << "-";
        val = -val;
    }
    char* temp = itob(val);
    out << btoh<E>(temp, 4, prefix);
    delete[] temp;
    return out.str();
}

template <Endian E>
std::string stoh(ushort val, bool prefix) {
    uchar* temp = stob(val);
    std::string out = btoh<E>(temp, 2, prefix);
    delete[] temp;
    return out;
}

template <Endian E>
std::string stoh(short val, bool prefix) {
    std::stringstream out;
    if (val < 0) {
        out << "-";
        val = -val;
    }
    char* temp = stob(val);
    out << btoh<E>(temp, 2, prefix);
    delete[] temp;
    return out.str();
}

template <Endian E>
std::string ctoh(uchar val, bool prefix) {
    uchar* temp = ctob(val);
    std::string out = btoh<E>(temp, 1, prefix);
    delete[] temp;
    return out;
}

template <Endian E>
std::string ctoh(char val, bool prefix) {
    std::stringstream out;
    if (val < 0) {
        out << "-";
        val = -val;
    }
    char* temp = ctob(val);
    out << btoh<E>(temp, 1, prefix);
    delete[] temp;
    return out.str();
}

}