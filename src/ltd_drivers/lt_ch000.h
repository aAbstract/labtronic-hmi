#ifndef LT_CH000_H
#define LT_CH000_H

#include "ltd_driver.h"

#define LT_CH000_DRIVER_CONFIG_SIZE 9

#define WRITE_PISTON_PUMP_MSG_TYPE 0
#define WRITE_PERISTALTIC_PUMP_MSG_TYPE 1
#define WRITE_WEIGHT_MSG_TYPE 2
#define WRITE_TEMPERATURE_MSG_TYPE 3
#define WRITE_PRESSURE_MSG_TYPE 4
#define WRITE_DEVICE_ERROR 14
#define READ_PISTON_PUMP_MSG_TYPE 12
#define READ_PERISTALTIC_PUMP_MSG_TYPE 13
#define READ_RESET_SCALE_MSG_TYPE 15

MsgTypeConfig lt_ch000_driver_config[LT_CH000_DRIVER_CONFIG_SIZE] = {
    {
        .msg_type = WRITE_PISTON_PUMP_MSG_TYPE,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = WRITE_PERISTALTIC_PUMP_MSG_TYPE,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 1,
        .cfg2 = 0,
    },
    {
        .msg_type = WRITE_WEIGHT_MSG_TYPE,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = WRITE_TEMPERATURE_MSG_TYPE,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = WRITE_PRESSURE_MSG_TYPE,
        .data_type = DATA_TYPE_FLOAT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = WRITE_DEVICE_ERROR,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 1,
        .cfg2 = 0,
    },
    {

        .msg_type = READ_PISTON_PUMP_MSG_TYPE,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 4,
        .cfg2 = 0,
    },
    {
        .msg_type = READ_PERISTALTIC_PUMP_MSG_TYPE,
        .data_type = DATA_TYPE_UINT,
        .size_bytes = 1,
        .cfg2 = 0,
    },
    {
        .msg_type = READ_RESET_SCALE_MSG_TYPE,
        .data_type = DATA_TYPE_COMMAND,
        .size_bytes = 1,
        .cfg2 = 0,
    },
};

#endif