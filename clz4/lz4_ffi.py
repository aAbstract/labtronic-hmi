import os
from ctypes import (
    CDLL,
    c_uint32,
    c_uint8,
    POINTER,
)


def create_lz4_ffi() -> CDLL | None:
    lz4_lib_path = 'clz4/lz4.so'
    if not os.path.exists(lz4_lib_path):
        return None
    lz4 = CDLL(lz4_lib_path)

    # int LZ4_compressBound(int inputSize);
    lz4.LZ4_compressBound.argtypes = [
        c_uint32,
    ]
    lz4.LZ4_compressBound.restype = c_uint32

    # int LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity);
    lz4.LZ4_compress_default.argtypes = [
        POINTER(c_uint8),
        POINTER(c_uint8),
        c_uint32,
        c_uint32,
    ]
    lz4.LZ4_compress_default.restype = c_uint32

    # int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize);
    lz4.LZ4_decompress_safe.argtypes = [
        POINTER(c_uint8),
        POINTER(c_uint8),
        c_uint32,
        c_uint32,
    ]

    return lz4
