#ifndef SCTRUCT_CMD_H
#define SCTRUCT_CMD_H

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
#define CMD_CHECK_LINK 0x01//Проверка связи (пинг)
#define CMD_GET_DEVICE_INFO 0x06//
#define CMD_GET_MAC_ADDRESS 0x02//Получить MAC-адрес устройства
#define CMD_GET_NETPARAMS 0x04//Получить настройки сети
#define CMD_ANS_STATUS 0x8A//Идентификатор ответа на команду CMD_READ_STATUS
#define CMD_ANS_NETPARAMS 0x82//Идентификатор от вета на команду CMD_GET_NETPARAMS

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
typedef struct  __attribute__((packed)) cmd_simple
{
    uint16_t Lenght;
    uint8_t Message_ID;
} cmd_simple_t;
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

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_ans_status
{
    uint16_t Lenght;
    uint8_t Message_ID;
    int8_t Device_ID;
    int8_t Temperaure;
    uint8_t StateRelay1;
    uint8_t StateRelay2;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    float CurrentAZ;
    float SetAZ;
    float ParkAZ;
    uint8_t StateDriveAZ;
    uint8_t EndSwitchesAZ;
    uint8_t ConnectAZ;
    int8_t TemperatureAZ;
    uint8_t ErrosCodeAZ;
    uint8_t SignalAZ;
    float ErrorAZ;
    float SpeedAZ;
    uint8_t DiriveModeAZ;
    uint8_t CountCmdSendDrAZ;
    uint8_t CountErrorAnsDrAZ;
    uint8_t CountMissedAnsDrAZ;
    float CurrentEL;
    float SetEL;
    float ParkEL;
    uint8_t StateDriveEL;
    uint8_t EndSwitchesEL;
    uint8_t ConnectEL;
    int8_t TemperatureEL;
    uint8_t ErrosCodeEL;
    uint8_t SignalEL;
    float ErrorEL;
    float SpeedEL;
    uint8_t DiriveModeEL;
    uint8_t CountCmdSendDrEL;
    uint8_t CountErrorAnsDrEL;
    uint8_t CountMissedAnsDrEL;
    float CurrentPOL;
    float SetPOL;
    float ParkPOL;
    uint8_t StateDrivePOL;
    uint8_t EndSwitchesPOL;
    uint8_t ConnectPOL;
    int8_t TemperaturePOL;
    uint8_t ErrosCodePOL;
    uint8_t SignalPOL;
    float ErrorPOL;
    float SpeedPOL;
    uint8_t DiriveModePOL;
    uint8_t CountCmdSendDrPOL;
    uint8_t CountErrorAnsDrPOL;
    uint8_t CountMissedAnsDrPOL;
} cmd_ans_status_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  __attribute__((packed)) cmd_ans_netparams
{
    uint16_t Lenght;
    uint8_t Message_ID;
    uint32_t Gateway;
    uint32_t Netmask;
    uint32_t IPdevice;
} cmd_ans_netparams_t;
#pragma pack(pop)


#endif // SCTRUCT_CMD_H









