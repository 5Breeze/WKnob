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
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// 广播间隔 (单位: 625us, 最小值为160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL (1600 * 10) // 4s
#define FAST_ADVERTISING_INTERVAL (1600 / 10) // 0.1s
// 电池定时事件周期
#define SBP_PERIODIC_EVT_PERIOD (1600 * 60 * 10) // 10min

// ADC 粗略校准值
static signed short RoughCalib_Value = 0;
// 电池电压
static uint16_t bat = 0;

uint8_t mag_int_flag = 0;
uint8_t eck_int_flag = 0;
uint8_t ecr_int_flag = 0;
uint8_t ecl_int_flag = 0;

int8_t ecc_count = 0;

// Task ID for internal task/event processing
uint8_t Broadcaster_TaskID;

#define BAT_OFFSET 9
#define ACCX_OFFSET 11
#define ACCY_OFFSET 14
#define ACCZ_OFFSET 17
#define COUNT_OFFSET 20
#define BTN_OFFSET 22 // 新增 button 事件偏移

static uint8_t advertData[] = {
    // AD结构1: Flags (3字节)
    0x02,
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    // AD结构2: BTHome Service Data
    // length = 后面字节数：1(UUID低)+1(UUID高)+1(device info)+
    //          2(bat)+3(accx)+3(accy)+3(accz)+2(count)+2(button) = 18 = 0x12
    // 但AD length字段含自身后的所有字节，格式是 len, 0x16, uuid_lo, uuid_hi, payload...
    // len = 1(0x16) + 2(uuid) + payload_len = 3 + 15 = 18 = 0x13
    0x13,
    0x16,
    0xD2,
    0xFC,
    0x40, // BTHome v2 device info

    0x01,
    0x64, // [9]  battery %
    0x51,
    0x00,
    0x00, // [11] ACCx (obj 0x51, uint16)
    0x51,
    0x00,
    0x00, // [14] ACCy
    0x51,
    0x00,
    0x00, // [17] ACCz
    0x59,
    0x00, // [20] rotation (obj 0x59, int8)
    0x3A,
    0x00, // [22] button event (obj 0x3A, uint8), 0x00=None

    // AD结构3: 完整本地名称
    0x06,
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'W',
    'K',
    'n',
    'o',
    'b',
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
#define LIS2DW12_ADDR 0x19 // 若SA0接VDD，否则为0x18

#define REG_CTRL1 0x20
#define REG_CTRL2 0x21
#define REG_CTRL3 0x22
#define TAR4_INT1_PAD_CTRL 0x23
#define REG_CTRL5_INT2_PAD_CTRL 0x24
#define REG_CTRL6 0x25
#define REG_STATUS 0x27

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
#define REG_CTRL7 0x3F

__HIGH_CODE
int lis2dw12_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return i2c_write_to(LIS2DW12_ADDR, buf, 2, 1, 1);
}

__HIGH_CODE
int lis2dw12_read_reg(uint8_t reg, uint8_t* buf, uint8_t len)
{
    if (i2c_write_to(LIS2DW12_ADDR, &reg, 1, 1, 0) != 0) {
        return -1;
    }
    if (i2c_read_from(LIS2DW12_ADDR, buf, len, 1, 100) != 0) {
        return -1;
    }
    return 1;
}

void LIS2DW12_Init(void)
{
    i2c_app_init(LIS2DW12_ADDR); // 重新初始化I2C，确保通信正常
    lis2dw12_write_reg(REG_CTRL1, 0x18); // 配置CTRL1: 低功耗模式1 + 单次转换模式
    lis2dw12_write_reg(REG_CTRL3, 0x02); // 配置CTRL3: 使能软件触发 (SLP_MODE_SEL=1)
    lis2dw12_write_reg(REG_CTRL6, 0x00); // 配置CTRL6: ±2g满量程, 禁用低噪声模式
}

__HIGH_CODE
void update_advert_data()
{
    static uint16_t bl_bat, ad_bat;
    bl_bat = sample_battery_voltage();
    ad_bat = get_battery_percentage(bl_bat / 10);
    advertData[BAT_OFFSET] = ad_bat;
}

__HIGH_CODE
void update_acc_ec_advert_data()
{
    static int16_t acc_x_raw, acc_y_raw, acc_z_raw;
    static int16_t acc_x, acc_y, acc_z;

    i2c_app_init(LIS2DW12_ADDR); // 确保I2C已初始化
    // 触发单次转换
    lis2dw12_write_reg(REG_CTRL3, 0x03); // 设置SLP_MODE_SEL=1 和 SLP_MODE_1=1
    // 等待数据就绪标志
    uint16_t timeout = 1000;
    while (timeout--) {
        uint8_t status;
        lis2dw12_read_reg(REG_STATUS, &status, 1);
        if (status & 0x01) break;
    }

// 读取三轴加速度数据
uint8_t buf[6];
lis2dw12_read_reg(REG_OUT_X_L, buf, 6);

// 正确拼接14位有符号数
acc_x_raw = (int16_t)((buf[1] << 8) | buf[0]) >> 2;
acc_y_raw = (int16_t)((buf[3] << 8) | buf[2]) >> 2;
acc_z_raw = (int16_t)((buf[5] << 8) | buf[4]) >> 2;

// 转换为 BTHome 0x51 所需单位：uint16，精度 0.001 m/s²
// raw × 0.244[mg/LSB] × 9.80665/1000[g→m/s²] / 0.001[BTHome精度]
// = raw × 2.3928
// 取绝对值（uint16不能为负，各轴分别上报幅值）
const float scale = 0.244f * 9.80665f; // = 2.3928
uint16_t acc_x_out = (uint16_t)(acc_x_raw >= 0 ?  acc_x_raw * scale : -acc_x_raw * scale);
uint16_t acc_y_out = (uint16_t)(acc_y_raw >= 0 ?  acc_y_raw * scale : -acc_y_raw * scale);
uint16_t acc_z_out = (uint16_t)(acc_z_raw >= 0 ?  acc_z_raw * scale : -acc_z_raw * scale);

advertData[ACCX_OFFSET]     = (uint8_t)(acc_x_out & 0xFF);
advertData[ACCX_OFFSET + 1] = (uint8_t)((acc_x_out >> 8) & 0xFF);
advertData[ACCY_OFFSET]     = (uint8_t)(acc_y_out & 0xFF);
advertData[ACCY_OFFSET + 1] = (uint8_t)((acc_y_out >> 8) & 0xFF);
advertData[ACCZ_OFFSET]     = (uint8_t)(acc_z_out & 0xFF);
advertData[ACCZ_OFFSET + 1] = (uint8_t)((acc_z_out >> 8) & 0xFF);
}

static uint16_t pending_advInt = 0;  // 待生效的广播间隔

// 修改广播间隔的入口，只负责停止广播
static void set_advertising_interval(uint16_t advInt)
{
    static uint16_t last_advInt = 0;
    if (advInt == last_advInt) {
        return;
    }
    last_advInt = advInt;
    pending_advInt = advInt;

    // 只停止广播，剩余操作交给事件处理
    uint8_t adv_off = FALSE;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &adv_off);

    // 延时 100ms 后再重启，给栈足够时间完成关闭
    tmos_start_task(Broadcaster_TaskID, ADV_RESTART_EVENT, MS1_TO_SYSTEM_TIME(100));
}


static void btn_report(uint8_t event_id)
{
    advertData[BTN_OFFSET] = event_id;
    GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);

    set_advertising_interval(FAST_ADVERTISING_INTERVAL);
    tmos_stop_task(Broadcaster_TaskID, CLEAR_END_EVENT);
    tmos_start_task(Broadcaster_TaskID, CLEAR_END_EVENT, MS_TO_TICK(3000));
}

// 编码器状态
static uint8_t encoder_last_state = 0b11; // 上电稳态 AB=11

static inline uint8_t read_encoder_state(void)
{
    uint8_t a = GPIOB_ReadPortPin(GPIO_Pin_10) ? 1 : 0; // A相 (ECK, PB10)
    uint8_t b = GPIOB_ReadPortPin(GPIO_Pin_11) ? 1 : 0; // B相 (ECL, PB11)
    return (a << 1) | b;
}

/*
 * 编码器计数：使用累积步数法，每 4 个有效步才提交一格。
 *
 * 机械编码器每格 = 4 个格雷码跳变：
 *   顺时针: 11->01->00->10->11  (每步 +1, 累计 +4 时提交 ecc_count+1)
 *   逆时针: 11->10->00->01->11  (每步 -1, 累计 -4 时提交 ecc_count-1)
 *
 * 优点：不依赖"必须捕获到稳态边沿"，任意边沿丢失只会导致
 * 那一格不被提交，而不会误计或漏计已完成的格。
 *
 * 格雷码跳变步长表（combined = last<<2 | current）:
 *   +1(CW) : 0b1101, 0b0100, 0b0010, 0b0111
 *   -1(CCW): 0b1110, 0b1000, 0b0001, 0b1011
 *   0(无效): 其余（同状态或非法跳变）
 */
static const int8_t ENC_TABLE[16] = {
    //  cur: 00  01  10  11
    /*last=00*/ 0,
    -1,
    +1,
    0,
    /*last=01*/ +1,
    0,
    0,
    -1,
    /*last=10*/ -1,
    0,
    0,
    +1,
    /*last=11*/ 0,
    +1,
    -1,
    0,
};
static int8_t encoder_accum = 0; // 累积步数，±4 提交一格

static void encoder_update(void)
{
    uint8_t current = read_encoder_state();
    int8_t step = ENC_TABLE[(encoder_last_state << 2) | current];
    encoder_last_state = current;

    if (step == 0) return; // 无效跳变，忽略

    encoder_accum += step;

    if (encoder_accum >= 4) {
        ecc_count++;
        encoder_accum = 0;
    } else if (encoder_accum <= -4) {
        ecc_count--;
        encoder_accum = 0;
    }
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
    }

    // 启动延迟profile
    // tmos_set_event(Broadcaster_TaskID, SBP_START_DEVICE_EVT);

    LIS2DW12_Init(); // 初始化LIS2DW12传感器
    
    tmos_start_task(Broadcaster_TaskID, SBP_START_DEVICE_EVT, DEFAULT_ADVERTISING_INTERVAL);

    // 定时采集数据并更新广播
    tmos_start_task(Broadcaster_TaskID, SBP_PERIODIC_EVT, 2 * DEFAULT_ADVERTISING_INTERVAL - 320);
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
        // 采集数据并更新广播
        update_advert_data();
        GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);

        return (events ^ SBP_PERIODIC_EVT);
    }

    /////////////////////////////

    //          按下                    松开(click++)
    //   IDLE ────────→ PRESSING ────────────────→ RELEASED
    //                     │                           │
    //                     │ 超时500ms                  │ 再次按下 → PRESSING
    //                     ↓                           │
    //              LONG_PRESSING                      │ 超时300ms
    //                     │                           ↓
    //            上报LONG/LONG_DBL/           上报PRESS/DBL/TRIPLE
    //            LONG_TRIPLE，启动            → IDLE
    //            HOLD定时器200ms
    //                     │
    //                     └─ hold_press 每200ms上报一次
    //                          直到松开 → IDLE
    /*
     * 按键状态机
     *
     * 普通点击时序（ECK_MULTI_GAP_MS=400ms 等待窗口）：
     *   单击:  按下 → 松开 → 等400ms → 上报PRESS
     *   双击:  按下 → 松开 → 按下 → 松开 → 等400ms → 上报DOUBLE_PRESS
     *   三击:  同上再按一次
     *
     * 长按时序（ECK_LONG_PRESS_MS=500ms，ECK_LONG_GAP_MS=800ms 等待窗口）：
     *   长按一次:   按住500ms → 松开 → 等800ms → 上报LONG_PRESS
     *   长按双击:   按住500ms → 松开 → 按住500ms → 松开 → 等800ms → 上报LONG_DOUBLE
     *   长按三击:   同上再来一次
     *
     * Hold（按住超过 500+ECK_HOLD_ENTER_MS=1100ms）：
     *   每 ECK_HOLD_REPEAT_MS=200ms 上报一次 HOLD_PRESS，直到松开
     *
     * 关键修复：
     *  1. ECK_RELEASED/ECK_LONG_RELEASED 再次按下时必须清对应计数器
     *  2. 长按的等待窗口(ECK_LONG_GAP_MS)独立于普通点击窗口，要足够长
     *  3. 进入 ECK_LONG_PRESSING 之前先停掉 ECK_CONFIRM_EVENT，防止残留触发
     */

    // ── 按下 ──────────────────────────────────────────────
    if (events & ECK_SET_EVENT) {
        switch (eck_fsm) {
        case ECK_IDLE:
            // 全新序列，清所有计数
            eck_clicks = 0;
            eck_long_clicks = 0;
            eck_fsm = ECK_PRESSING;
            tmos_stop_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT);
            tmos_start_task(Broadcaster_TaskID, ECK_LONG_EVENT,
                MS_TO_TICK(ECK_LONG_PRESS_MS));
            break;

        case ECK_RELEASED:
            // 普通点击序列中再次按下，clicks 已经累积，不清零
            eck_fsm = ECK_PRESSING;
            tmos_stop_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT);
            tmos_start_task(Broadcaster_TaskID, ECK_LONG_EVENT,
                MS_TO_TICK(ECK_LONG_PRESS_MS));
            break;

        case ECK_LONG_RELEASED:
            // 长按序列中再次按下，long_clicks 已累积，普通 clicks 清零（防止串扰）
            eck_clicks = 0;
            eck_fsm = ECK_PRESSING;
            tmos_stop_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT);
            tmos_start_task(Broadcaster_TaskID, ECK_LONG_EVENT,
                MS_TO_TICK(ECK_LONG_PRESS_MS));
            break;

        default:
            // PRESSING / LONG_PRESSING / HOLDING 时忽略抖动重入
            break;
        }
        return (events ^ ECK_SET_EVENT);
    }

    // ── 松开 ──────────────────────────────────────────────
    if (events & ECK_RELEASE_EVENT) {
        tmos_stop_task(Broadcaster_TaskID, ECK_LONG_EVENT);
        tmos_stop_task(Broadcaster_TaskID, ECK_HOLD_EVENT);

        switch (eck_fsm) {
        case ECK_PRESSING:
            // 没到长按阈值 → 普通点击，累积 clicks，等多击窗口
            eck_clicks++;
            eck_long_clicks = 0; // 确保进入普通点击路径时长按计数为0
            eck_fsm = ECK_RELEASED;
            tmos_stop_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT);
            tmos_start_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT,
                MS_TO_TICK(ECK_MULTI_GAP_MS));
            break;

        case ECK_LONG_PRESSING:
            // 长按后松开，累积长按次数，等待是否还有下一次长按
            eck_long_clicks++;
            eck_clicks = 0; // 确保进入长按路径时普通计数为0
            eck_fsm = ECK_LONG_RELEASED;
            tmos_stop_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT);
            tmos_start_task(Broadcaster_TaskID, ECK_CONFIRM_EVENT,
                MS_TO_TICK(ECK_LONG_GAP_MS));
            break;

        case ECK_HOLDING:
            // hold 松开，结束，hold 已逐帧上报，此处不再上报
            eck_fsm = ECK_IDLE;
            break;

        default:
            eck_fsm = ECK_IDLE;
            break;
        }
        return (events ^ ECK_RELEASE_EVENT);
    }

    // ── 单/双/三击 或 长按 确认（等待窗口超时）────────────
    if (events & ECK_CONFIRM_EVENT) {
        switch (eck_fsm) {
        case ECK_LONG_RELEASED:
            switch (eck_long_clicks) {
            case 1:
                btn_report(BTN_LONG_PRESS);
                break;
            case 2:
                btn_report(BTN_LONG_DOUBLE_PRESS);
                break;
            default:
                btn_report(BTN_LONG_TRIPLE_PRESS);
                break;
            }
            break;

        case ECK_RELEASED:
            switch (eck_clicks) {
            case 1:
                btn_report(BTN_PRESS);
                break;
            case 2:
                btn_report(BTN_DOUBLE_PRESS);
                break;
            default:
                btn_report(BTN_TRIPLE_PRESS);
                break;
            }
            break;

        default:
            // 其他状态（IDLE / 被外部干扰）不上报
            break;
        }
        eck_clicks = 0;
        eck_long_clicks = 0;
        eck_fsm = ECK_IDLE;
        return (events ^ ECK_CONFIRM_EVENT);
    }

    // ── 长按阈值到达（手还未松开）─────────────────────────
    if (events & ECK_LONG_EVENT) {
        eck_fsm = ECK_LONG_PRESSING;
        // 继续按住 ECK_HOLD_ENTER_MS 后进入 hold 模式
        tmos_start_task(Broadcaster_TaskID, ECK_HOLD_EVENT,
            MS_TO_TICK(ECK_HOLD_ENTER_MS));
        return (events ^ ECK_LONG_EVENT);
    }

    // ── hold_press（长按阈值后继续按着）──────────────────
    if (events & ECK_HOLD_EVENT) {
        eck_fsm = ECK_HOLDING;
        eck_clicks = 0;
        eck_long_clicks = 0;
        update_acc_ec_advert_data(); // hold_press 时也更新一次编码器数据，确保广告里是最新的
        ecc_count = 0; // hold_press 开始时重置编码器计数
        advertData[COUNT_OFFSET] = ecc_count; // 更新计数
        btn_report(BTN_HOLD_PRESS);
        tmos_start_task(Broadcaster_TaskID, ECK_HOLD_EVENT,
            MS_TO_TICK(ECK_HOLD_REPEAT_MS));
        return (events ^ ECK_HOLD_EVENT);
    }
    if (events & ECN_SET_EVENT) {
        encoder_update();
        advertData[COUNT_OFFSET] = ecc_count; // 更新计数
        GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);
        set_advertising_interval(FAST_ADVERTISING_INTERVAL);
        // 使用独立的 ECN_CLEAR_EVENT，避免覆盖按键的 CLEAR_END_EVENT 3秒定时器
        tmos_stop_task(Broadcaster_TaskID, ECN_CLEAR_EVENT);
        tmos_start_task(Broadcaster_TaskID, ECN_CLEAR_EVENT, MS_TO_TICK(5000));
        return (events ^ ECN_SET_EVENT);
    }

    // if (events & ACC_SET_EVENT) {
    //     update_acc_ec_advert_data();
    //     GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);
    //     return (events ^ ACC_SET_EVENT);
    // }

    if (events & CLEAR_END_EVENT) {
        // 按键事件广播清除：3秒后把 BTN 字段恢复为 NONE，恢复慢速广播
        advertData[BTN_OFFSET] = BTN_NONE;
        GAP_UpdateAdvertisingData(0, TRUE, sizeof(advertData), advertData);
        set_advertising_interval(DEFAULT_ADVERTISING_INTERVAL);
        return (events ^ CLEAR_END_EVENT);
    }

    if (events & ECN_CLEAR_EVENT) {
        // 编码器广播恢复：5秒后恢复慢速广播（独立定时器，不干扰按键清除）
        set_advertising_interval(DEFAULT_ADVERTISING_INTERVAL);
        return (events ^ ECN_CLEAR_EVENT);
    }

    if (events & ADV_RESTART_EVENT)
{
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, pending_advInt);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, pending_advInt);

    uint8_t adv_on = TRUE;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &adv_on);

    return (events ^ ADV_RESTART_EVENT);
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
