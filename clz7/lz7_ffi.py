import os
from ctypes import (
    CDLL,
    c_uint32,
    c_uint8,
    POINTER,
)


def create_lz7_ffi() -> CDLL | None:
    lz7_lib_path = 'clz7/lz7.so'
    if not os.path.exists(lz7_lib_path):
        return None
    lz7 = CDLL(lz7_lib_path)

    # int LZ_Compress( unsigned char *in, unsigned char *out, unsigned int insize );
    lz7.LZ_Compress.argtypes = [
        POINTER(c_uint8),
        POINTER(c_uint8),
        c_uint32,
    ]
    lz7.LZ_Compress.restype = c_uint32

    # void LZ_Uncompress( unsigned char *in, unsigned char *out, unsigned int insize );
    lz7.LZ_Uncompress.argtypes = [
        POINTER(c_uint8),
        POINTER(c_uint8),
        c_uint32,
    ]

    return lz7
