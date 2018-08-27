#pragma once

#include "types.h"

namespace util {

template <typename T>
bool compare(T *first, T *second, const ulong length);

template <typename T>
bool compare_2D(T **first, T **second, const ulong height, const ulong width);

template <typename T>
bool compare_2D(T **first, T **second, const ulong size);

template <typename T>
void copy(const T *source, T *dest, const ulong length);

template <typename T>
void copy_2D(T **source, T **dest, const ulong height, const ulong width);

template <typename T>
void copy_2D(T **source, T **dest, const ulong size);

/**
 * Reverse an array in place. Returns the pointer to the array, to allow chaining.
 * @tparam T
 * @param arr
 * @param len
 * @return
 */
template<typename T>
T* reverse(T *arr, const ulong len);

/**
 * Convert an array from one endian into another, reversing every X items.
 * size == len makes this equivalent to reverse.
 * @tparam T
 * @param arr
 * @param len
 * @param size
 * @return
 */
template<typename T>
T* endian_convert(T *arr, const ulong len, const ulong size = 4);

}

#include "arrays.tpp"
