# autopep8: off
import os
import sys
import re
from glob import glob
from ctypes import (
    c_uint8,
    c_uint32,
)
sys.path.append(os.getcwd())
from clz4.lz4_ffi import create_lz4_ffi
from clz7.lz7_ffi import create_lz7_ffi
from rle.rle_ffi import create_rle_ffi
# autopep8: on

ASSETS_BASE_DIR = 'assets'
HMI_MODULE_NAME = 'lt_ht107'
ASSETS_OUT_BIN_DIR = 'assets_out_bin'
ASSETS_OUT_SRC_DIR = 'assets_out_src'

lz4 = create_lz4_ffi()
lz7 = create_lz7_ffi()
rle = create_rle_ffi()

asset_src_template = '''
#include "lvgl/lvgl.h"

{extern_decompress_decl}

static __attribute__((section(".ext_data_memory"))) uint8_t {asset_name}{mode}_buffer[] = {{
{asset_buffer}
}};

static uint8_t {asset_name}_buffer[{data_size}];

const lv_img_dsc_t {asset_name} = {{
  .header.cf = LV_IMG_CF_INDEXED_8BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = {width},
  .header.h = {height},
  .data_size = {data_size},
  .data = {asset_name}_buffer,
}};

{decompress_function}
'''


def fmt_buffer(buffer: bytes) -> str:
    out = ''
    for idx, byte in enumerate(buffer):
        out += '0x' + format(byte, '02X') + ','
        if (idx + 1) % 16 == 0:
            out += '\n'
        elif (idx + 1) % 4 == 0:
            out += ' '
    return out


def parse_asset_c_file(file_path: str) -> tuple[list[str], dict]:
    ''' extracts binary data and meta data from C source file '''
    print('Parsing File:', file_path)
    with open(file_path, 'r') as f:
        src_content = f.read()
        bin_data = re.findall('0x[0-9|a|b|c|d|e|f]{2}', src_content)
        if not bin_data:
            print(file_path, '- Can not Find Binary Data')
            sys.exit(1)

        meta_data = {}
        for _key in ['.header.cf', '.header.w', '.header.h', '.data_size', '.data']:
            _regex = _key.replace('.', '\.') + ' = (.*),'
            _key_value = re.findall(_regex, src_content)
            if not _key_value:
                print(file_path, '-', _key, 'Key not Found')
                sys.exit(1)
            _key_value: str = _key_value[0]
            meta_data[_key] = int(_key_value) if _key_value.isnumeric() else _key_value

        meta_data['asset_name'] = meta_data['.data'].replace('_map', '')
        meta_data['.data_size'] = len(bin_data)

    return bin_data, meta_data


def compression_pipeline_lz4(asset_path: str):
    print('Parsing:', asset_path)
    os.system(f"python3 scripts/LVGLImage.py --ofmt C --cf I8 {asset_path} -o {ASSETS_OUT_BIN_DIR} > /dev/null")
    asset_name = asset_path.split('/')[-1].replace('.png', '')
    asset_c = f"{ASSETS_OUT_BIN_DIR}/{asset_path.split('/')[-1].replace('.png', '')}.c"

    print('Loadeding:', asset_c)
    bin_data_str, meta_data = parse_asset_c_file(asset_c)
    src_buffer = bytearray([int(x, 16) for x in bin_data_str])
    data_size = meta_data['.data_size']

    print('Compressing:', asset_name)
    dst_buffer_capacity: int = lz4.LZ4_compressBound(c_uint32(data_size))
    _src_buffer = (c_uint8 * data_size).from_buffer(src_buffer)
    _dst_buffer = (c_uint8 * dst_buffer_capacity)()
    compressed_buffer_size = lz4.LZ4_compress_default(_src_buffer, _dst_buffer, c_uint32(data_size), c_uint32(dst_buffer_capacity))
    print('Compression Ratio:', compressed_buffer_size / data_size)
    print('=' * 100)
    print('Data Size:', data_size)
    print('Dst Capacity:', dst_buffer_capacity)
    print('Compressed Size:', compressed_buffer_size)
    print('=' * 100)

    print('Validating Data...')
    compressed_buffer = _dst_buffer
    decompression_capacity = 1024**2
    decompressed_buffer = (c_uint8 * decompression_capacity)()
    decompressed_data_size = lz4.LZ4_decompress_safe(compressed_buffer, decompressed_buffer, c_uint32(compressed_buffer_size), c_uint32(decompression_capacity))
    if decompressed_data_size != data_size:
        print('ERROR:', 'Data Size Mismatch')
        sys.exit(1)
    for i in range(data_size):
        if src_buffer[i] != decompressed_buffer[i]:
            print(f"Data Mismatch at Offset {i}")
            sys.exit(1)
    print('Validating Data...OK')

    print('Generating Source:', asset_name)
    with open(f"{ASSETS_OUT_SRC_DIR}/{asset_name}.h", 'w') as f:
        f.write(asset_src_template.format(
            asset_name=asset_name,
            mode='_compressed',
            asset_name_uppercase=asset_name.upper(),
            width=meta_data['.header.w'],
            height=meta_data['.header.h'],
            data_size=data_size,
            asset_buffer=fmt_buffer(compressed_buffer[:compressed_buffer_size]),
            decompress_buffer=f'static uint8_t {asset_name}_buffer[{data_size}];',
            extern_decompress_decl="extern void RLE_Uncompress(uint8_t* in, uint8_t* out, uint32_t insize);",
            decompress_function=f"void decompress_{asset_name}() {{ LZ4_decompress_safe({asset_name}_compressed_buffer, {asset_name}_buffer, sizeof({asset_name}_compressed_buffer), {data_size}); }}",
        ))


def compression_pipeline_lz7(asset_path: str) -> float:
    ''' returns compresstion ratio '''
    print('Parsing:', asset_path)
    os.system(f"python3 scripts/LVGLImage.py --ofmt C --cf I8 {asset_path} -o {ASSETS_OUT_BIN_DIR} > /dev/null")
    asset_name = asset_path.split('/')[-1].replace('.png', '')
    asset_c = f"{ASSETS_OUT_BIN_DIR}/{asset_path.split('/')[-1].replace('.png', '')}.c"

    print('Loadeding:', asset_c)
    bin_data_str, meta_data = parse_asset_c_file(asset_c)
    src_buffer = bytearray([int(x, 16) for x in bin_data_str])
    data_size = meta_data['.data_size']

    print('Compressing:', asset_name)
    BUFFER_CAP = 512 * 1024
    _src_buffer = (c_uint8 * data_size).from_buffer(src_buffer)
    _dst_buffer = (c_uint8 * BUFFER_CAP)()
    compressed_buffer_size = lz7.LZ_Compress(_src_buffer, _dst_buffer, c_uint32(data_size))
    compression_ratio = compressed_buffer_size / data_size
    print('Compression Ratio:', compression_ratio)
    print('=' * 100)
    print('Data Size:', data_size)
    print('Dst Capacity:', BUFFER_CAP)
    print('Compressed Size:', compressed_buffer_size)
    print('=' * 100)

    print('Validating Data...')
    compressed_buffer = _dst_buffer
    decompressed_buffer = (c_uint8 * BUFFER_CAP)()
    lz7.LZ_Uncompress(compressed_buffer, decompressed_buffer, c_uint32(compressed_buffer_size))
    for i in range(data_size):
        if src_buffer[i] != decompressed_buffer[i]:
            print(f"Data Mismatch at Offset {i}")
            sys.exit(1)
    print('Validating Data...OK')

    print('Generating Source:', asset_name)
    with open(f"{ASSETS_OUT_SRC_DIR}/{asset_name}.h", 'w') as f:
        f.write(asset_src_template.format(
            asset_name=asset_name,
            mode='_compressed',
            asset_name_uppercase=asset_name.upper(),
            width=meta_data['.header.w'],
            height=meta_data['.header.h'],
            data_size=data_size,
            asset_buffer=fmt_buffer(compressed_buffer[:compressed_buffer_size]),
            decompress_buffer=f'static uint8_t {asset_name}_buffer[{data_size}];',
            extern_decompress_decl="extern void LZ_Uncompress(uint8_t* in, uint8_t* out, uint32_t insize);",
            decompress_function=f"void decompress_{asset_name}() {{ LZ_Uncompress({asset_name}_compressed_buffer, {asset_name}_buffer, {compressed_buffer_size}); }}",
        ))

    return compression_ratio


def compression_pipeline_rle(asset_path: str) -> float:
    ''' returns compresstion ratio '''
    print('Parsing:', asset_path)
    os.system(f"python3 scripts/LVGLImage.py --ofmt C --cf I8 {asset_path} -o {ASSETS_OUT_BIN_DIR} > /dev/null")
    asset_name = asset_path.split('/')[-1].replace('.png', '')
    asset_c = f"{ASSETS_OUT_BIN_DIR}/{asset_path.split('/')[-1].replace('.png', '')}.c"

    print('Loadeding:', asset_c)
    bin_data_str, meta_data = parse_asset_c_file(asset_c)
    src_buffer = bytearray([int(x, 16) for x in bin_data_str])
    data_size = meta_data['.data_size']

    print('Compressing:', asset_name)
    BUFFER_CAP = 512 * 1024
    _src_buffer = (c_uint8 * data_size).from_buffer(src_buffer)
    _dst_buffer = (c_uint8 * BUFFER_CAP)()
    compressed_buffer_size = rle.RLE_Compress(_src_buffer, _dst_buffer, c_uint32(data_size))
    compression_ratio = compressed_buffer_size / data_size
    print('Compression Ratio:', compression_ratio)
    print('=' * 100)
    print('Data Size:', data_size)
    print('Dst Capacity:', BUFFER_CAP)
    print('Compressed Size:', compressed_buffer_size)
    print('=' * 100)

    print('Validating Data...')
    compressed_buffer = _dst_buffer
    decompressed_buffer = (c_uint8 * BUFFER_CAP)()
    rle.RLE_Uncompress(compressed_buffer, decompressed_buffer, c_uint32(compressed_buffer_size))
    for i in range(data_size):
        if src_buffer[i] != decompressed_buffer[i]:
            print(f"Data Mismatch at Offset {i}")
            sys.exit(1)
    print('Validating Data...OK')

    print('Generating Source:', asset_name)
    with open(f"{ASSETS_OUT_SRC_DIR}/{asset_name}.h", 'w') as f:
        f.write(asset_src_template.format(
            asset_name=asset_name,
            mode='_compressed',
            asset_name_uppercase=asset_name.upper(),
            width=meta_data['.header.w'],
            height=meta_data['.header.h'],
            data_size=data_size,
            asset_buffer=fmt_buffer(compressed_buffer[:compressed_buffer_size]),
            extern_decompress_decl="extern void RLE_Uncompress(uint8_t* in, uint8_t* out, uint32_t insize);",
            decompress_function=f"void decompress_{asset_name}() {{ RLE_Uncompress({asset_name}_compressed_buffer, {asset_name}_buffer, {compressed_buffer_size}); }}",
        ))

    return compression_ratio


def raw_pipeline(asset_path: str):
    print('Parsing:', asset_path)
    os.system(f"python3 scripts/LVGLImage.py --ofmt C --cf I8 {asset_path} -o {ASSETS_OUT_BIN_DIR} > /dev/null")
    asset_name = asset_path.split('/')[-1].replace('.png', '')
    asset_c = f"{ASSETS_OUT_BIN_DIR}/{asset_path.split('/')[-1].replace('.png', '')}.c"

    print('Loadeding:', asset_c)
    bin_data_str, meta_data = parse_asset_c_file(asset_c)
    src_buffer = bytearray([int(x, 16) for x in bin_data_str])
    data_size = meta_data['.data_size']

    print('Generating Source:', asset_name)
    with open(f"{ASSETS_OUT_SRC_DIR}/{asset_name}.h", 'w') as f:
        f.write(asset_src_template.format(
            asset_name=asset_name,
            mode='',
            asset_name_uppercase=asset_name.upper(),
            width=meta_data['.header.w'],
            height=meta_data['.header.h'],
            data_size=data_size,
            asset_buffer=fmt_buffer(src_buffer),
            decompress_buffer='',
            extern_decompress_decl='',
            decompress_function='',
        ))


def clear_assets_out():
    for gp in [
        f"{ASSETS_OUT_BIN_DIR}/*",
        f"{ASSETS_OUT_SRC_DIR}/*",
    ]:
        [os.remove(f) for f in glob(gp)]
        print(f"Clearing {gp}...OK")


if __name__ == '__main__':
    compression_pipeline = compression_pipeline_rle

    if len(sys.argv) < 2:
        clear_assets_out()
        assets = glob(f"{ASSETS_BASE_DIR}/{HMI_MODULE_NAME}/*.png")
        compression_ratio_list = [compression_pipeline(asset) for asset in assets]
        print('Avg Compression Ratio:', sum(compression_ratio_list) / len(compression_ratio_list))
        sys.exit(0)

    if len(sys.argv) == 2:
        if sys.argv[1] == 'clear':
            clear_assets_out()
            sys.exit(0)

        file = sys.argv[1]
        compression_pipeline(file)
        sys.exit(0)

    if len(sys.argv) == 3:
        mode = sys.argv[1]
        file = sys.argv[2]
        if mode == 'c':
            compression_pipeline(file)
        elif mode == 'r':
            raw_pipeline(file)
        else:
            print('File Handle Mode Error')
            sys.exit(2)
        sys.exit(0)

    print('CLI Args Error')
    sys.exit(1)
