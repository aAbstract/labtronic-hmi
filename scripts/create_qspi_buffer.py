import re
import sys
import json
import textwrap
from glob import glob


ASSETS_BASE_DIR = './scripts/assets'
HMI_MODULE_NAME = 'lt_ht113'

common_assets_sources = glob(f"{ASSETS_BASE_DIR}/common_assets/*.c")
hmi_module_assets_sources = glob(f"{ASSETS_BASE_DIR}/{HMI_MODULE_NAME}_assets/*.c")


def parse_src_file(file_path: str) -> tuple[list[str], dict]:
    ''' extracts binary data and meta data from C source file '''
    print('Parsing File:', file_path)
    with open(file_path, 'r') as f:
        src_content = f.read()
        bin_data = re.findall('0x[0-9|a|b|c|d|e|f]{2}', src_content)
        if not bin_data:
            print(file_path, '- Can not Find Binary Data')
            sys.exit(1)

        meta_data = {}
        for _key in ['.header.cf', '.header.always_zero', '.header.reserved', '.header.w', '.header.h', '.data_size', '.data']:
            _regex = _key.replace('.', '\.') + ' = (.*),'
            _key_value = re.findall(_regex, src_content)
            if not _key_value:
                print(file_path, '-', _key, 'Key not Found')
                sys.exit(1)
            _key_value: str = _key_value[0]
            meta_data[_key] = int(_key_value) if _key_value.isnumeric() else _key_value

        if meta_data['.data_size'] != len(bin_data):
            print(file_path, '- Data Size Mismatch')
            sys.exit(1)

        meta_data['asset_name'] = meta_data['.data'].replace('_map', '')

    return bin_data, meta_data


def fmt_bin_data(bin_data: list[str]) -> str:
    bin_data_fmt = ''
    for idx, byte in enumerate(bin_data):
        bin_data_fmt += byte + ','
        if (idx + 1) % 16 == 0:
            bin_data_fmt += '\n'
        elif (idx + 1) % 4 == 0:
            bin_data_fmt += ' '
    return bin_data_fmt


def parse_assets(assets_paths: list[str], buffer_type: int) -> tuple[str, list[dict]]:
    ''' returns binary buffer source and meta data for each asset_path '''
    out_src = ''
    out_meta_data: list[dict] = []
    buffer_type_map = {
        8: 'uint8_t',
        16: 'uint16_t',
        32: 'uint32_t',
    }
    for asset_src_file in assets_paths:
        bin_data, meta_data = parse_src_file(asset_src_file)
        out_meta_data.append(meta_data)
        _fmt_bin_data = fmt_bin_data(bin_data)
        out_src += '// ' + json.dumps(meta_data) + '\n'
        out_src += f"static const __attribute__((section(\".ext_data_memory\"))) {buffer_type_map[buffer_type]} {meta_data['asset_name']}_buffer[] = " + '{\n' + textwrap.indent(_fmt_bin_data, '    ') + '\n};\n'
    return out_src, out_meta_data


def create_size_defines(assets_meta_data: list[dict]) -> str:
    ''' converts assets_meta_data to a size defines C code '''
    size_defines = ''
    for _md in assets_meta_data:
        asset_name = _md['.data'].replace('_map', '').upper()
        size_defines += f"#define {asset_name}_SIZE {_md['.data_size']}\n"
        size_defines += f"#define {asset_name}_WIDTH {_md['.header.w']}\n"
        size_defines += f"#define {asset_name}_HEIGHT {_md['.header.h']}\n"
    return size_defines


def create_lv_img_objs(assets_meta_data: list[dict], buffer_type: int) -> str:
    lv_img_obj_template = '''
    lv_img_dsc_t {asset_name} = {{
        .header.cf = {color_format},
        .header.always_zero = 0,
        .header.reserved = 0,
        .header.w = {width},
        .header.h = {height},
        .data_size = {size},
        .data = {data_buffer},
    }};
    '''

    lv_img_obj_src = ''
    for _md in assets_meta_data:
        buffer_type_map: dict[int, str] = {
            8: f"{_md['asset_name']}_buffer",
            16: f"(uint8_t*){_md['asset_name']}_buffer",
            32: f"(uint8_t*){_md['asset_name']}_buffer",
        }
        lv_img_obj_src += textwrap.dedent(lv_img_obj_template.format(
            asset_name=_md['asset_name'],
            color_format=_md['.header.cf'],
            width=_md['.header.w'],
            height=_md['.header.h'],
            size=_md['.data_size'],
            data_buffer=buffer_type_map[buffer_type],
        ))
    return lv_img_obj_src


qspi_buffer_h_template = '''
#ifndef QSPI_BUFFER_UINT{buffer_type}_H
#define QSPI_BUFFER_UINT{buffer_type}_H

#include "lvgl/lvgl.h"

// start_common_assets_buffer
{ca_qspi_buffer}
// end_common_assets_buffer

// start_hmi_module_assets_buffer
{hma_qspi_buffer}
// end_hmi_module_assets_buffer

{ca_defines}

{hma_defines}

{ca_lv_img_objs}

{hma_lv_img_objs}

#endif
'''


def main():
    if len(sys.argv) < 2:
        print('Missing Buffer Type: Usage create_qspi_buffer.py [8 (uint8_t*) | 16 (uint16_t*) | 32 (uint32_t*)]')
        sys.exit(1)

    if sys.argv[1] not in ['8', '16', '32']:
        print('Invalid Buffer Type: Usage create_qspi_buffer.py [8 (uint8_t*) | 16 (uint16_t*) | 32 (uint32_t*)]')
        sys.exit(1)

    buffer_type = int(sys.argv[1])
    ca_qspi_buffer, ca_meta_data = parse_assets(common_assets_sources, buffer_type)
    hma_qspi_buffer, hma_meta_data = parse_assets(hmi_module_assets_sources, buffer_type)
    ca_defines = create_size_defines(ca_meta_data)
    hma_defines = create_size_defines(hma_meta_data)
    ca_lv_img_objs = create_lv_img_objs(ca_meta_data, buffer_type)
    hma_lv_img_objs = create_lv_img_objs(hma_meta_data, buffer_type)

    qspi_buffer_h = f"./src/qspi_buffer_uint{buffer_type}.h"
    open(qspi_buffer_h, 'w').write(qspi_buffer_h_template.format(
        buffer_type=buffer_type,
        ca_qspi_buffer=ca_qspi_buffer,
        hma_qspi_buffer=hma_qspi_buffer,
        ca_defines=ca_defines,
        hma_defines=hma_defines,
        ca_lv_img_objs=ca_lv_img_objs,
        hma_lv_img_objs=hma_lv_img_objs,
    ))
    print('Output File:', qspi_buffer_h)


if __name__ == '__main__':
    main()
