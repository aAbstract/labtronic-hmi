#include <math.h>
#include <stdint.h>

#define PACKET_MIN_SIZE 11
#define MAX_MSG_TYPES 16 // 4 msg_type bits

#define PKT_OFST_PV 0
#define PKT_OFST_LEN 2
#define PKT_OFST_SN 3
#define PKT_OFST_CFG1 5
#define PKT_OFST_CFG2 6
#define PKT_OFST_DATA 7

#define RC_OK 0
#define RC_ERR_UNK_MSG_TYPE 1
#define RC_ERR_PKT_TOO_SMALL 2
#define RC_ERR_INV_CRC16 3
#define RC_ERR_INV_PV 4
#define RC_ERR_INV_PKT_LEN 5

#define DATA_TYPE_INT 0
#define DATA_TYPE_UINT 1
#define DATA_TYPE_FLOAT 2
#define DATA_TYPE_COMMAND 3

typedef struct MsgTypeConfig {
  uint8_t msg_type;
  uint8_t data_type;
  uint8_t size_bytes;
  uint8_t cfg2;
} MsgTypeConfig;

typedef struct DeviceMsg {
  MsgTypeConfig config;
  uint16_t seq_number;
  uint8_t msg_value_buffer[8];
} DeviceMsg;

uint8_t init_ltd_driver(uint16_t _protocol_version, const MsgTypeConfig* driver_config, uint8_t arr_size);
uint8_t encode_packet(uint16_t msg_seq_number, uint8_t msg_type, const uint8_t* msg_value_ptr, uint8_t* out_packet);
uint8_t decode_packet(const uint8_t* packet, DeviceMsg* out_device_msg);

static uint16_t protocol_version = 0;
static MsgTypeConfig config_hash_map[MAX_MSG_TYPES];

// clang-format off
static const uint16_t CRC16_POLYNOMIAL[] = {
  0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
  0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
  0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
  0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
  0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
  0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
  0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
  0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
  0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
  0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
  0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
  0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
  0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
  0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
  0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
  0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
  0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
  0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
  0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
  0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
  0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
  0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
  0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
  0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
  0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
  0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
  0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
  0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
  0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
  0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
  0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
  0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78,
};
// clang-format on

uint8_t gen_cfg1(const MsgTypeConfig* _config) {
  uint8_t data_type_bits = (uint8_t)(_config->data_type & 0x03);
  uint8_t data_length_bits = (uint8_t)log2(_config->size_bytes) & 0x03;
  uint8_t msg_type_bits = (uint8_t)(_config->msg_type & 0x0F);
  uint8_t cfg1_byte = (data_type_bits << 6) | (data_length_bits << 4) | msg_type_bits;
  return cfg1_byte;
}

uint16_t compute_crc16(const uint8_t* data_ptr, uint8_t data_len) {
  uint16_t res = 0xffff;
  for (uint8_t i = 0; i < data_len; i++)
    res = (res >> 8) ^ CRC16_POLYNOMIAL[(res ^ data_ptr[i]) & 0xff];
  return ~res;
}

MsgTypeConfig get_msg_type_config(uint8_t config_idx) {
  if (config_idx >= MAX_MSG_TYPES)
    return (MsgTypeConfig){.msg_type = 0, .data_type = 0, .size_bytes = 0};
  return config_hash_map[config_idx];
}

uint8_t init_ltd_driver(uint16_t _protocol_version, const MsgTypeConfig* driver_config, uint8_t arr_size) {
  protocol_version = _protocol_version;
  for (uint8_t i = 0; i < arr_size; i++) {
    MsgTypeConfig _config = driver_config[i];
    config_hash_map[_config.msg_type] = _config;
  }
  return RC_OK;
}

uint8_t encode_packet(uint16_t msg_seq_number, uint8_t msg_type, const uint8_t* msg_value_ptr, uint8_t* out_packet) {
  // check if the msg_type exists in the config_hash_map
  MsgTypeConfig _config = get_msg_type_config(msg_type);
  if (_config.size_bytes == 0)
    return RC_ERR_UNK_MSG_TYPE;

  // load start segment
  uint8_t* msg_seq_number_ptr = (uint8_t*)&msg_seq_number;
  uint8_t cfg1_byte = gen_cfg1(&_config);
  out_packet[PKT_OFST_PV] = ((uint8_t*)&protocol_version)[0];
  out_packet[PKT_OFST_PV + 1] = ((uint8_t*)&protocol_version)[1];
  out_packet[PKT_OFST_LEN] = PACKET_MIN_SIZE + _config.size_bytes;
  out_packet[PKT_OFST_SN] = msg_seq_number_ptr[0];
  out_packet[PKT_OFST_SN + 1] = msg_seq_number_ptr[1];
  out_packet[PKT_OFST_CFG1] = cfg1_byte;
  out_packet[PKT_OFST_CFG2] = _config.cfg2;

  // load data segment
  for (uint8_t i = 0; i < _config.size_bytes; i++)
    out_packet[PKT_OFST_DATA + i] = msg_value_ptr[i];

  // load end segment
  uint16_t crc16 = compute_crc16(out_packet, PKT_OFST_DATA + _config.size_bytes);
  uint8_t* crc16_ptr = (uint8_t*)&crc16;
  out_packet[PKT_OFST_DATA + _config.size_bytes] = crc16_ptr[0];
  out_packet[PKT_OFST_DATA + _config.size_bytes + 1] = crc16_ptr[1];
  out_packet[PKT_OFST_DATA + _config.size_bytes + 2] = 0x0D;
  out_packet[PKT_OFST_DATA + _config.size_bytes + 3] = 0x0A;

  return RC_OK;
}

uint8_t decode_packet(const uint8_t* packet, DeviceMsg* out_device_msg) {
  // check PROTOCOL_VERSION
  if (packet[PKT_OFST_PV] != ((uint8_t*)&protocol_version)[0] || packet[PKT_OFST_PV + 1] != ((uint8_t*)&protocol_version)[1])
    return RC_ERR_INV_PV;

  // check packet length
  uint8_t packet_len = packet[PKT_OFST_LEN];
  if (packet[packet_len - 2] != 0x0D || packet[packet_len - 1] != 0x0A)
    return RC_ERR_INV_PKT_LEN;

  // check msg_type
  uint8_t cfg1_byte = packet[PKT_OFST_CFG1];
  uint8_t msg_type = cfg1_byte & 0x0F;
  MsgTypeConfig _config = get_msg_type_config(msg_type);
  if (_config.size_bytes == 0)
    return RC_ERR_UNK_MSG_TYPE;

  // check crc16
  uint16_t packet_crc16 = 0xFFFF;
  ((uint8_t*)&packet_crc16)[0] = packet[PKT_OFST_DATA + _config.size_bytes];
  ((uint8_t*)&packet_crc16)[1] = packet[PKT_OFST_DATA + _config.size_bytes + 1];
  uint16_t target_crc16 = compute_crc16(packet, PKT_OFST_DATA + _config.size_bytes);
  if (packet_crc16 != target_crc16)
    return RC_ERR_INV_CRC16;

  // construct device_msg object
  // load config
  out_device_msg->config = _config;
  out_device_msg->config.cfg2 = packet[PKT_OFST_CFG2];
  // load seq_number
  uint16_t packet_seq_number = 0xFFFF;
  ((uint8_t*)&packet_seq_number)[0] = packet[PKT_OFST_SN];
  ((uint8_t*)&packet_seq_number)[1] = packet[PKT_OFST_SN + 1];
  out_device_msg->seq_number = packet_seq_number;
  // load data
  for (uint8_t i = 0; i < _config.size_bytes; i++)
    out_device_msg->msg_value_buffer[i] = packet[PKT_OFST_DATA + i];

  return RC_OK;
}