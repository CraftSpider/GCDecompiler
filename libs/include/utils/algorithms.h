#pragma once

#include "types.h"

namespace util {

/**
 * Generate a crc32 hash of a given character array
 * @param input Array pointer
 * @param length Length of the array
 * @return crc32 hash as an unsigned integer
 */
uint crc32(const uchar *input, const ulong length);

}
