import os
from ctypes import (
    CDLL,
    c_uint32,
    c_uint8,
    POINTER,
)


def create_rle_ffi() -> CDLL | None:
    rle_lib_path = 'rle/rle.so'
    if not os.path.exists(rle_lib_path):
        return None
    rle = CDLL(rle_lib_path)

    # int RLE_Compress(uint8_t* in, uint8_t* out, uint32_t insize);
    rle.RLE_Compress.argtypes = [
        POINTER(c_uint8),
        POINTER(c_uint8),
        c_uint32,
    ]
    rle.RLE_Compress.restype = c_uint32

    # void RLE_Uncompress(uint8_t* in, uint8_t* out, uint32_t insize);
    rle.RLE_Uncompress.argtypes = [
        POINTER(c_uint8),
        POINTER(c_uint8),
        c_uint32,
    ]

    return rle
