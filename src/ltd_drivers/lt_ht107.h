#ifndef LT_HT107_H
#define LT_HT107_H

#include "ltd_driver.h"

#define LT_HT107_DRIVER_CONFIG_SIZE 12
#define LT_HT107_PROTOCOL_VERSION 0x1413

MsgTypeConfig lt_ht107_driver_config[LT_HT107_DRIVER_CONFIG_SIZE] = {
    {
        // T1
        .msg_type = 0,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T2
        .msg_type = 1,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T3
        .msg_type = 2,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T4
        .msg_type = 3,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T5
        .msg_type = 4,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T6
        .msg_type = 5,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T7
        .msg_type = 6,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T8
        .msg_type = 7,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T9
        .msg_type = 8,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // T_H
        .msg_type = 9,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // P_Heater
        .msg_type = 13,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        // DEVICE_ERROR
        .msg_type = 14,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 1,
        .cfg2 = 0,
    },
};

#endif