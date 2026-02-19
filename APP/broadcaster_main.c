/* (C) COPYRIGHT ********************************************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 广播应用主入口，完成系统初始化
 *****************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *****************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "HAL.h"
#include "broadcaster.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if (defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = { 0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02 };
#endif

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   主循环
 *
 * @return  none
 */
__HIGH_CODE
__attribute__((noinline)) void Main_Circulation()
{
    while (1) {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   程序入口
 *
 * @return  none
 */
int main(void)
{
#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#if (defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
#ifdef DEBUG
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif
    /* 配置外部中断源为 GPIO - PB10-13 */
    // //PB13-ACC
    // GPIOB_ModeCfg(GPIO_Pin_13, GPIO_ModeIN_Floating);
    // GPIOB_ITModeCfg(GPIO_Pin_13, GPIO_ITMode_FallEdge); // 下降沿触发
    //PB12-ECK
    GPIOB_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_Floating);
    GPIOB_ITModeCfg(GPIO_Pin_12, GPIO_ITMode_FallEdge); // 下降沿触发
    //PB11-ECL
    GPIOB_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_Floating);
    GPIOB_ITModeCfg(GPIO_Pin_11, GPIO_ITMode_FallEdge); // 下降沿触发
    //PB10-ECR
    GPIOB_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_Floating);
    GPIOB_ITModeCfg(GPIO_Pin_10, GPIO_ITMode_FallEdge); // 下降沿触发

    PFIC_EnableIRQ(GPIO_B_IRQn);
    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, RB_SLP_GPIO_WAKE);

    mDelaymS(100);  // 给电容充电、电源稳定时间
    // PRINT("%s\n", VER_LIB);
    CH59x_BLEInit();
    HAL_Init();
    GAPRole_BroadcasterInit();
    Broadcaster_Init();
    Main_Circulation();
}

//中断服务函数
__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )
{
    // // 检测到PB12中断
    // if(GPIOB_ReadITFlagBit(GPIO_Pin_13))
    // {
    //     PRINT("ENTER ACC_IRQHandler\n");
    //     tmos_set_event(Broadcaster_TaskID, ACC_SET_EVENT); // 触发加速度事件
    //     GPIOB_ClearITFlagBit(GPIO_Pin_13);
    // } 
if (GPIOB_ReadITFlagBit(GPIO_Pin_12))
{
    mDelaymS(5); // 简单消抖
    uint8_t level = GPIOB_ReadPortPin(GPIO_Pin_12) ? 1 : 0;
    if (!level) { // 低电平 = 按下（假设按下接地）
        tmos_set_event(Broadcaster_TaskID, ECK_SET_EVENT);
    } else {      // 高电平 = 松开
        tmos_set_event(Broadcaster_TaskID, ECK_RELEASE_EVENT);
    }
        // 切换到相反边沿，下次捕捉另一个动作
    GPIOB_ITModeCfg(GPIO_Pin_12,
        level ? GPIO_ITMode_FallEdge : GPIO_ITMode_RiseEdge);
    GPIOB_ClearITFlagBit(GPIO_Pin_12);
}

else if (GPIOB_ReadITFlagBit(GPIO_Pin_10))
{
    tmos_set_event(Broadcaster_TaskID, ECN_SET_EVENT);
    // 根据当前电平动态切换触发边沿，模拟双边沿检测
    GPIOB_ITModeCfg(GPIO_Pin_10,
    GPIOB_ReadPortPin(GPIO_Pin_10) ? GPIO_ITMode_FallEdge : GPIO_ITMode_RiseEdge);
    GPIOB_ClearITFlagBit(GPIO_Pin_10);
}
else if (GPIOB_ReadITFlagBit(GPIO_Pin_11))
{
    tmos_set_event(Broadcaster_TaskID, ECN_SET_EVENT);
    // 根据当前电平动态切换触发边沿，模拟双边沿检测
    GPIOB_ITModeCfg(GPIO_Pin_11,
    GPIOB_ReadPortPin(GPIO_Pin_11) ? GPIO_ITMode_FallEdge : GPIO_ITMode_RiseEdge);
    GPIOB_ClearITFlagBit(GPIO_Pin_11);
}
}


/******************************** endfile @ main ******************************/
