#ifndef SCTRUCT_H
#define SCTRUCT_H

#include "stdint.h"
#include "string.h"

#define CMD_SET_POSITION 0x42//Установить положение
#define CMD_SET_CNTRL_DEV 0x40//Выбрать управляющее устройство
#define CMD_RESET_DEV 0x41// Перезагрузить контроллер
#define CMD_STOP_MOVING 0x43//Остановить движение
#define CMD_ADJUST_POS_SENSOR 0x44//Коррекция текущего местоположения
#define CMD_MOVE_PARK_POSITION 0x45//Перейти в парковочное положение
#define CMD_ADJUST_PARK_POSITION 0x46//Настроить парковочное положение
#define CMD_SET_RELAY_STATE 0x47//Переключение реле
#define CMD_TEST_SEVO_CONNECTION 0x48//Запуск тестов
#define CMD_SET_SERVO_WORK_MODE 0x49//Настроить режим работы привода
#define CMD_READ_STATUS 0x34//Прочитать статус
#define CMD_CHANGE_NETPARAMS 0x05//Изменить настройки подключения

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_set_position
{
    uint16_t Lenght;
    uint8_t Message_ID;
    float AZ_Speed;
    float AZ_Position;
    float EL_Speed;
    float EL_Position;
} cmd_set_position_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_set_cntrl_dev
{
    uint16_t Lenght;
    uint8_t Message_ID;
    int8_t Device_ID;
} cmd_set_cntrl_dev_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_reset_dev
{
    uint16_t Lenght;
    uint8_t Message_ID;
} cmd_reset_dev_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_stop_moving
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint8_t Mask;
} cmd_stop_moving_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_adjust_pos_sensor
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint8_t Axis;
    float Position;
} cmd_adjust_pos_sensor_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_move_park_position
{
    uint16_t Lenght;
    uint8_t Message_ID;
    float AZ_Speed;
    float EL_Speed;
} cmd_move_park_position_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_adjust_park_position
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint8_t Axis;
} cmd_adjust_park_position_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_set_relay_state
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint8_t RelayNum;
    uint8_t RelayState;
} cmd_set_relay_state_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_test_sevo_connection
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint8_t TypeOfTest;
} cmd_test_sevo_connection_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_set_servo_work_mode
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint8_t Axis;
    uint8_t Mode_Axis;
    uint8_t Mode_Complate;
    uint8_t Mode_Brake;
    uint8_t Mode_Error;
} cmd_set_servo_work_mode_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_read_status
{
    uint16_t Lenght;
    uint8_t Message_ID;
} cmd_read_status_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_change_netparams
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint32_t Gateway;
    uint32_t Netmask;
    uint32_t IPdevice;
    uint8_t  RAMofFlash;
} cmd_change_netparams_t;
#pragma pack(pop)


#endif // SCTRUCT_H









