#ifndef BROADCASTER_H
#define BROADCASTER_H

#ifdef __cplusplus
extern "C" {
#endif

// TMOS 任务事件位
#define SBP_START_DEVICE_EVT      0x0001
#define SBP_PERIODIC_EVT          0x0002
#define SBP_ADV_IN_CONNECTION_EVT 0x0004
#define ECK_SET_EVENT             0x0008  // 按下原始事件
#define ECK_RELEASE_EVENT         0x0010  // 松开原始事件
#define ECN_SET_EVENT             0x0020  // 编码器旋转
#define ACC_SET_EVENT             0x0040  // 加速度计中断
#define CLEAR_END_EVENT           0x0080  // 按键广播恢复
#define ECK_CONFIRM_EVENT         0x0100  // 单击/双击/三击/长按 确认定时器
#define ECK_LONG_EVENT            0x0200  // 长按阈值定时器
#define ECK_HOLD_EVENT            0x0400  // hold_press 持续定时器
#define ECN_CLEAR_EVENT           0x0800  // 编码器广播恢复（独立，不与按键共用）
#define ADV_RESTART_EVENT         0x1000  // 重启广播事件

// BTHome button 事件值
#define BTN_NONE                0x00
#define BTN_PRESS               0x01
#define BTN_DOUBLE_PRESS        0x02
#define BTN_TRIPLE_PRESS        0x03
#define BTN_LONG_PRESS          0x04
#define BTN_LONG_DOUBLE_PRESS   0x05
#define BTN_LONG_TRIPLE_PRESS   0x06
#define BTN_HOLD_PRESS          0x80

// 时间阈值（单位 ms）
#define ECK_LONG_PRESS_MS   500   // 按住超过此时间视为长按
#define ECK_MULTI_GAP_MS    400   // 普通点击多击等待窗口
#define ECK_LONG_GAP_MS     500   // 长按序列多击等待窗口
                                  // （用户需要松开→再次按住500ms，至少要800ms）
#define ECK_HOLD_ENTER_MS   600   // 进入长按后再持续此时间才进 hold 模式
                                  // （总计 500+600=1100ms 才触发 hold）
#define ECK_HOLD_REPEAT_MS  200   // hold_press 重复上报间隔

#define MS_TO_TICK(ms)  ((ms) * 1000 / 625)

typedef enum {
    ECK_IDLE,
    ECK_PRESSING,       // 按下中，等待长按超时或松开
    ECK_RELEASED,       // 普通点击松开，等多击窗口
    ECK_LONG_PRESSING,  // 已达长按阈值，手还没松
    ECK_LONG_RELEASED,  // 长按后松开，等看是否再次长按
    ECK_HOLDING,        // hold_press 持续中
} eck_fsm_t;

static eck_fsm_t eck_fsm      = ECK_IDLE;
static uint8_t   eck_clicks   = 0;  // 普通点击次数（1/2/3）
static uint8_t   eck_long_clicks = 0; // 长按次数（1/2/3）

extern void Broadcaster_Init(void);
extern uint8_t Broadcaster_TaskID;
extern uint16_t Broadcaster_ProcessEvent(uint8_t task_id, uint16_t events);

#ifdef __cplusplus
}
#endif

#endif

