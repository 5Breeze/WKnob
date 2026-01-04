/* (C) COPYRIGHT ********************************************************
 * File Name          : broadcaster.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 广播应用程序，初始化广播参数后持续广播
 *
 *****************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *****************************************************************************/

#include "CONFIG.h"
#include "devinfoservice.h"
#include "broadcaster.h"
#include "app_i2c.h"

// 广播间隔 (单位: 625us, 最小值为160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL 1600 * 2 // 2s

// 定时事件周期
#define SBP_PERIODIC_EVT_PERIOD 1600 * 40 // 40s

// opt3001 读取事件周期
#define OPT3001_READ_EVENT_PERIOD 1600 * 1 // 2s

// opt3001 采集事件周期
#define OPT3001_CJ_EVENT_PERIOD 1600 * 60 // 40s

// ADC 粗略校准值
static signed short RoughCalib_Value = 0;
// 电池电压
static uint16_t bat = 0;

// static uint32_t opt3001_lux = 0; // 光照值

uint8_t mag_int_flag = 0;
uint8_t eck_int_flag = 0; 
uint8_t ecr_int_flag = 0; 
uint8_t ecl_int_flag = 0; 

int8_t ecc_count = 0;

uint16_t acc_x = 0;
uint16_t acc_y = 0;
uint16_t acc_z = 0;


// Task ID for internal task/event processing
static uint8_t Broadcaster_TaskID;

#define BAT_OFFSET 9
#define ACCX_OFFSET 11
#define ACCY_OFFSET 14
#define ACCZ_OFFSET 17
#define COUNT_OFFSET 20

static uint8_t advertData[] = {
    0x02, GAP_ADTYPE_FLAGS, GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    0x15, 0x16, 0xD2, 0xFC, // 广播AD类型和UUID (BTHome UUID FCD2)
    0x40, // BTHome v2 无加密，标准广播

    0x01, 0x00, // 电池
    0x51, 0x00, 0x00, // ACCx
    0x51, 0x00, 0x00, // ACCy
    0x51, 0x00, 0x00, // ACCz

    0x59, 0x00, // 计数sin8

    0x05, GAP_ADTYPE_LOCAL_NAME_COMPLETE, // 完整本地名称
    'W', 'k', 'n', 'o', 'b', // 名称 Wireless Knob
};

/*
 * 本地函数声明
 */
static void Broadcaster_ProcessTMOSMsg(tmos_event_hdr_t* pMsg);
static void Broadcaster_StateNotificationCB(gapRole_States_t newState);
extern bStatus_t GAP_UpdateAdvertisingData(uint8_t taskID, uint8_t adType, uint16_t dataLen, uint8_t* pAdvertData);

/*
 * PROFILE CALLBACKS
 */

// GAP Role 回调
static gapRolesBroadcasterCBs_t Broadcaster_BroadcasterCBs = {
    Broadcaster_StateNotificationCB, // Profile State Change Callbacks
    NULL
};

/*
 * PUBLIC FUNCTIONS
 */

// 采样电池电压
__HIGH_CODE
uint16_t sample_battery_voltage()
{
    // VINA 实际电压值 1050=1.05V
    const int vref = 1050;

    ADC_InterBATSampInit();

    // 每200次进行一次粗略校准
    static uint8_t calib_count = 0;
    calib_count++;
    if (calib_count == 1) {
        RoughCalib_Value = ADC_DataCalib_Rough();
    }
    calib_count %= 200;

    ADC_ChannelCfg(CH_INTE_VBAT);
    return (ADC_ExcutSingleConver() + RoughCalib_Value) * vref / 512 - 3 * vref;
}

#include <stdint.h>
#include <stdio.h>

#define VOLTAGE_BUFFER_SIZE 10

typedef struct {
    uint16_t voltage_x100; // 100倍电压，单位0.01V
    uint8_t percentage; // 对应电量
} VoltageLevel;

const VoltageLevel voltage_map[] = {
    { 180, 1 },
    { 185, 5 },
    { 190, 8 },
    { 192, 10 },
    { 195, 12 },
    { 200, 15 },
    { 205, 17 },
    { 210, 18 },
    { 215, 19 },
    { 220, 20 },
    { 225, 25 },
    { 230, 30 },
    { 232, 35 },
    { 235, 40 },
    { 240, 45 },
    { 245, 50 },
    { 250, 55 },
    { 255, 60 },
    { 257, 65 },
    { 260, 70 },
    { 262, 75 },
    { 265, 80 },
    { 270, 85 },
    { 275, 88 },
    { 280, 90 },
    { 285, 93 },
    { 290, 95 },
    { 295, 98 },
    { 300, 100 }
};

#define LEVEL_COUNT (sizeof(voltage_map) / sizeof(VoltageLevel))

// 电压转换为电量百分比
uint8_t voltage_to_percentage(uint16_t voltage_x100)
{
    if (voltage_x100 < voltage_map[0].voltage_x100)
        return 0;

    for (int i = 1; i < LEVEL_COUNT; ++i) {
        if (voltage_x100 < voltage_map[i].voltage_x100) {
            return voltage_map[i - 1].percentage;
        }
    }
    return voltage_map[LEVEL_COUNT - 1].percentage;
}

// 主函数接口
uint8_t get_battery_percentage(uint16_t new_voltage_x100)
{
    static uint16_t voltage_buffer[VOLTAGE_BUFFER_SIZE] = { 0 };
    static uint8_t voltage_count = 0;
    static uint8_t buffer_index = 0;
    static int last_percentage = 100;

    // 存入缓冲区
    voltage_buffer[buffer_index] = new_voltage_x100;
    buffer_index = (buffer_index + 1) % VOLTAGE_BUFFER_SIZE;
    if (voltage_count < VOLTAGE_BUFFER_SIZE)
        voltage_count++;

    // 求均值（整数求和）
    uint32_t sum = 0;
    for (uint8_t i = 0; i < voltage_count; ++i) {
        sum += voltage_buffer[i];
    }
    uint16_t avg_voltage = sum / voltage_count;

    // 查表转换为电量百分比
    uint8_t percentage = voltage_to_percentage(avg_voltage);

    // 电量不允许回升
    if (percentage > last_percentage)
        percentage = last_percentage;

    last_percentage = percentage;
    return percentage;
}

#define LIS2DW12_ADDR 0x19 // 若SA0接VDD，否则为0x18

#define REG_CTRL1 0x20
#define REG_CTRL2 0x21
#define REG_CTRL3 0x22
#define REG_CTRL4_INT1_PAD_CTRL 0x23
#define REG_CTRL5_INT2_PAD_CTRL 0x24
#define REG_CTRL6 0x25

#define REG_TAP_THS_X 0x30
#define REG_TAP_THS_Y 0x31
#define REG_TAP_THS_Z 0x32
#define REG_INT_DUR 0x33
#define REG_WAKE_UP_THS 0x34
#define REG_TAP_SRC 0x39

#define REG_WHO_AM_I 0x0F

#define REG_OUT_X_L 0x28
#define REG_OUT_X_H 0x29
#define REG_OUT_Y_L 0x2A
#define REG_OUT_Y_H 0x2B
#define REG_OUT_Z_L 0x2C
#define REG_OUT_Z_H 0x2D

#define TAR4_INT1_PAD_CTRL 0x23
#define REG_CTRL7 0x3F

__HIGH_CODE
int lis2dw12_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return i2c_write_to(LIS2DW12_ADDR, buf, 2, 1, 1); 
}

void LIS2DW12_Init(void)
{
    lis2dw12_write_reg(REG_CTRL1, 0x20); // 25Hz LP Mode 1
    lis2dw12_write_reg(REG_CTRL6, 0x00);
    lis2dw12_write_reg(REG_TAP_THS_X, 0x05);
    lis2dw12_write_reg(REG_TAP_THS_Y, 0xE5);
    lis2dw12_write_reg(REG_TAP_THS_Z, 0xE5);
    lis2dw12_write_reg(REG_CTRL3, 0x08);
    lis2dw12_write_reg(REG_INT_DUR, 0x40); // 中断持续时间
    lis2dw12_write_reg(REG_WAKE_UP_THS, 0x80); // 唤醒阈值
    lis2dw12_write_reg(TAR4_INT1_PAD_CTRL, 0x08); // INT1引脚配置
    lis2dw12_write_reg(REG_CTRL7, 0x20); // BDU=1, IF_ADD_INC=1
}

void LIS2DW12_InitForSingleConversion(void)
{
    lis2dw12_write_reg(REG_CTRL2, 0x40);
    DelayMs(10);
    lis2dw12_write_reg(REG_CTRL2, 0x0C);
    lis2dw12_write_reg(REG_CTRL1, 0x50);
    lis2dw12_write_reg(REG_CTRL3, 0x02);
    lis2dw12_write_reg(REG_CTRL6, 0x10);
}

void LIS2DW12_ReadSingleConversion(void)
{
    uint8_t status = 0;
    uint8_t data[6] = {0};

    do {
        lis2dw12_read_reg(REG_STATUS, &status, 1);
    } while (!(status & 0x01));

    lis2dw12_read_reg(REG_OUT_X_L | 0x80, data, 6);

    int16_t x = ((int16_t)data[1] << 8 | data[0]) >> 4;
    int16_t y = ((int16_t)data[3] << 8 | data[2]) >> 4;
    int16_t z = ((int16_t)data[5] << 8 | data[4]) >> 4;
	
    float x_g = x * 0.000976f;
    float y_g = y * 0.000976f;
    float z_g = z * 0.000976f;

    acc_x = (uint16_t)abs((x_g * 1000)); // 转换为mg
    acc_y = (uint16_t)abs((y_g * 1000));
    acc_z = (uint16_t)abs((z_g * 1000));
}
__HIGH_CODE
void update_advert_data()
{
    static uint16_t bl_bat, ad_bat;
    bl_bat = sample_battery_voltage();
    ad_bat = get_battery_percentage(bl_bat/10);
    advertData[BAT_OFFSET] = ad_bat;
}


void update_acc_ec_advert_data()
{
    LIS2DW12_InitForSingleConversion();
    LIS2DW12_ReadSingleConversion();
    LIS2DW12_Init()
    advertData[ACCX_OFFSET] = (uint8_t)(acc_x & 0xFF);
    advertData[ACCX_OFFSET + 1] = (uint8_t)((acc_x >> 8) & 0xFF);
    advertData[ACCY_OFFSET] = (uint8_t)(acc_y & 0xFF);
    advertData[ACCY_OFFSET + 1] = (uint8_t)((acc_y >> 8) & 0xFF);
    advertData[ACCZ_OFFSET] = (uint8_t)(acc_z & 0xFF);
    advertData[ACCZ_OFFSET + 1] = (uint8_t)((acc_z >> 8) & 0xFF);

    advertData[COUNT_OFFSET] = (uint8_t)(ecc_count & 0xFF);
}
/*
 * @fn      Broadcaster_Init
 *
 * @brief   广播应用初始化函数
 *          该函数在初始化时调用，包含应用相关初始化（如硬件初始化、表初始化、上电通知等）。
 *
 * @param   task_id - 由TMOS分配的ID。该ID用于发送消息和设置定时器。
 *
 * @return  none
 */
void Broadcaster_Init()
{
    Broadcaster_TaskID = TMOS_ProcessEventRegister(Broadcaster_ProcessEvent);

    // 设置GAP广播角色参数
    {
        // 设备初始化后立即广播
        uint8_t initial_advertising_enable = TRUE;
        uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;
        // uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_IND;
        // uint8_t initial_adv_event_type = GAP_ADTYPE_EXT_NONCONN_NONSCAN_UNDIRECT;

        // 设置GAP角色参数
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);
        GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &initial_adv_event_type);
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
    }

    // 设置广播间隔
    {
        uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

        GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);

        // GAP_SetParamValue(TGAP_ADV_SECONDARY_PHY, GAP_PHY_VAL_LE_CODED); // 125K
        // GAP_SetParamValue(TGAP_ADV_PRIMARY_PHY, GAP_PHY_VAL_LE_CODED); // 125K
    }

    // 启动延迟profile
    // tmos_set_event(Broadcaster_TaskID, SBP_START_DEVICE_EVT);
    spl06_init(); // 初始化SPL06传感器
    LIS2DW12_Init(); // 初始化LIS2DW12传感器

    tmos_start_task(Broadcaster_TaskID, SBP_START_DEVICE_EVT, DEFAULT_ADVERTISING_INTERVAL);

    // 定时采集数据并更新广播
    tmos_start_task(Broadcaster_TaskID, SBP_PERIODIC_EVT, 2 * DEFAULT_ADVERTISING_INTERVAL - 320);

    tmos_start_task(Broadcaster_TaskID, OPT3001_START_EVENT, 1600);

    tmos_start_task(Broadcaster_TaskID, ACC_ASK_EVENT, 800);
}

/*
 * @fn      Broadcaster_ProcessEvent
 *
 * @brief   广播应用事件处理函数。该函数处理所有任务事件，包括定时器、消息和用户自定义事件。
 *
 * @param   task_id  - TMOS分配的任务ID。
 * @param   events - 事件位图，可能包含多个事件。
 *
 * @return  未处理的事件
 */
uint16_t Broadcaster_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG) {
        uint8_t* pMsg;

        if ((pMsg = tmos_msg_receive(Broadcaster_TaskID)) != NULL) {
            Broadcaster_ProcessTMOSMsg((tmos_event_hdr_t*)pMsg);

            // 释放TMOS消息
            tmos_msg_deallocate(pMsg);
        }

        // 返回未处理的事件
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & SBP_START_DEVICE_EVT) {
        // 启动设备
        GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);

        return (events ^ SBP_START_DEVICE_EVT);
    }

    if (events & SBP_PERIODIC_EVT) {
        tmos_start_task(Broadcaster_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD);
        // //PRINT("SBP_PERIODIC_EVT\n");
        // 采集数据并更新广播
        update_advert_data();
        GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);

        return (events ^ SBP_PERIODIC_EVT);
    }

    if (events & ACC_ASK_EVENT) {
        if (mag_int_flag) {
            tmos_start_task(Broadcaster_TaskID, ACC_END_EVENT, 1600 * 2); // 2秒后重置事件标志位
            mag_int_flag = 0; // 清除中断标志
            advertData[KEY_OFFSET] = 0x02; // 双击事件
            // PRINT("ACC_ASK_EVENT: mag_int_flag=%d\n", mag_int_flag);
            GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);
        } else {
            tmos_start_task(Broadcaster_TaskID, ACC_ASK_EVENT, 800);
        }
        return (events ^ ACC_ASK_EVENT);
    }

    if (events & ACC_END_EVENT) {
        advertData[KEY_OFFSET] = 0x00; // 清除按键事件
        GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);
        tmos_start_task(Broadcaster_TaskID, ACC_ASK_EVENT, 800);
        return (events ^ ACC_END_EVENT);
    }

    return 0;
}

/*
 * @fn      Broadcaster_ProcessTMOSMsg
 *
 * @brief   处理收到的TMOS消息。
 *
 * @param   pMsg - 待处理的消息
 *
 * @return  none
 */
static void Broadcaster_ProcessTMOSMsg(tmos_event_hdr_t* pMsg)
{
    switch (pMsg->event) {
    default:
        break;
    }
}

/*
 * @fn      Broadcaster_StateNotificationCB
 *
 * @brief   GAP状态变化回调。
 *
 * @param   newState - 新状态
 *
 * @return  none
 */
static void Broadcaster_StateNotificationCB(gapRole_States_t newState)
{
    switch (newState) {
    case GAPROLE_STARTED:
        // PRINT("Initialized..\n");
        break;

    case GAPROLE_ADVERTISING:
        // PRINT("Advertising..\n");
        break;

    case GAPROLE_WAITING:
        // PRINT("Waiting for advertising..\n");
        break;

    case GAPROLE_ERROR:
        // PRINT("Error..\n");
        break;

    default:
        break;
    }
}
