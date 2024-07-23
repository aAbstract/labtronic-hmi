#ifndef LT_HT113_H
#define LT_HT113_H

#include "ltd_driver.h"

#define LT_HT113_DRIVER_CONFIG_SIZE 6
#define LT_HT113_PROTOCOL_VERSION 0x1414

MsgTypeConfig lt_ht113_driver_config[LT_HT113_DRIVER_CONFIG_SIZE] = {
    {
        .msg_type = 0,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = 1,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = 2,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = 3,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = 4,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 1,
        .cfg2 = 0,
    },
    {
        .msg_type = 14, // DEVICE_ERROR
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 1,
        .cfg2 = 0,
    },
};

#endif