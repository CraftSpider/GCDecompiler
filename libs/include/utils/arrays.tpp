
namespace util {

template <typename T>
bool compare(T *first, T *second, const ulong length) {
    for (ulong i = 0; i < length; ++i) {
        if (first[i] != second[i]) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool compare_2D(T **first, T **second, const ulong height, const ulong width) {
    for (ulong i = 0; i < height; ++i) {
        for (ulong j = 0; j < width; ++j) {
            if (first[i][j] != second[i][j]) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
bool compare_2D(T **first, T **second, const ulong size) {
    return compare_2D(first, second, size, size);
}

template <typename T>
void copy(const T *source, T *dest, const ulong length) {
    for (ulong i = 0; i < length; ++i) {
        dest[i] = source[i];
    }
}

template <typename T>
void copy_2D(T **source, T **dest, const ulong height, const ulong width) {
    for (ulong i = 0; i < height; ++i) {
        for (ulong j = 0; j < width; ++j) {
            dest[i][j] = source[i][j];
        }
    }
}

template <typename T>
void copy_2D(T **source, T **dest, const ulong size) {
    copy_2D(source, dest, size, size);
}

template <typename T>
T* reverse(T *arr, const ulong len) {
    T *tmp = new T[len];
    for (ulong i = 0; i < len; ++i) {
        tmp[i] = arr[len - i - 1];
    }
    for (ulong i = 0; i < len; ++i) {
        arr[i] = tmp[i];
    }
    delete[] tmp;
    return arr;
}

template <typename T>
T* endian_convert(T *arr, const ulong len, const ulong size) {
    T *tmp = new T[len];
    for (ulong i = 0; i < len; ++i) {
        tmp[(i + (size - 1)) - ((i % size) * 2)] = arr[i];
    }
    for (ulong i = 0; i < len; ++i) {
        arr[i] = tmp[i];
    }
    delete[] tmp;
    return arr;
}

}