/********************************** (C) COPYRIGHT *******************************
 * File Name          : CONFIG.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        : ����˵����Ĭ��ֵ�������ڹ����������Ԥ�������޸ĵ�ǰ�?
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#define	ID_CH592							0x92

#define CHIP_ID								ID_CH592

#define DCDC_ENABLE TRUE

#ifdef CH59xBLE_ROM
#include "CH59xBLE_ROM.h"
#else
#include "CH59xBLE_LIB.h"
#endif

#include "CH59x_common.h"

/*********************************************************************
 ��MAC��
 BLE_MAC                                    - �Ƿ��Զ�������Mac��ַ ( Ĭ��:FALSE - ʹ��оƬMac��ַ )����Ҫ��main.c�޸�Mac��ַ����

 ��DCDC��
 DCDC_ENABLE                                - �Ƿ�ʹ��DCDC ( Ĭ��:FALSE )

 ��SLEEP��
 HAL_SLEEP                                  - �Ƿ���˯�߹��� ( Ĭ��:FALSE )
 SLEEP_RTC_MIN_TIME                         - �ǿ���ģʽ��˯�ߵ���Сʱ�䣨��λ��һ��RTC���ڣ�
 SLEEP_RTC_MAX_TIME                         - �ǿ���ģʽ��˯�ߵ����ʱ��?��λ��һ��RTC���ڣ�
 WAKE_UP_RTC_MAX_TIME                       - �ȴ�32M�����ȶ�ʱ�䣨��λ��һ��RTC���ڣ�
                                                                                                                            ���ݲ�ͬ˯������ȡֵ�ɷ�Ϊ�� ˯��ģʽ/�µ�ģʽ  - 45 (Ĭ��)
                                                                                                                                                                                                  ��ͣģʽ    - 45
                                                                                                                                                                                                  ����ģʽ    - 5
 ��TEMPERATION��
 TEM_SAMPLE                                 - �Ƿ��?�����¶ȱ仯У׼�Ĺ��ܣ�����У׼��ʱС��10ms( Ĭ��:TRUE )
 
 ��CALIBRATION��
 BLE_CALIBRATION_ENABLE                     - �Ƿ��?��ʱУ׼�Ĺ��ܣ�����У׼��ʱС��10ms( Ĭ��:TRUE )
 BLE_CALIBRATION_PERIOD                     - ��ʱУ׼�����ڣ���λms( Ĭ��:120000 )
 
 ��SNV��
 BLE_SNV                                    - �Ƿ���SNV���ܣ����ڴ�������?( Ĭ��:TRUE )
 BLE_SNV_ADDR                               - SNV��Ϣ�����ַ��ʹ��data flash���?512�ֽ�( Ĭ��:0x77E00 )
 BLE_SNV_BLOCK                              - SNV��Ϣ������С( Ĭ��:256 )
 BLE_SNV_NUM                                - SNV��Ϣ��������( Ĭ��:1 )

 ��RTC��
 CLK_OSC32K                                 - RTCʱ��ѡ��������������?����ʹ���ⲿ32K( 0 �ⲿ(32768Hz)��Ĭ��:1���ڲ�(32000Hz)��2���ڲ�(32768Hz) )

 ��MEMORY��
 BLE_MEMHEAP_SIZE                           - ����Э��ջʹ�õ�RAM��С����С��6K ( Ĭ��:(1024*6) )

 ��DATA��
 BLE_BUFF_MAX_LEN                           - ����������������( Ĭ��:27 (ATT_MTU=23)��ȡֵ��Χ[27~516] )
 BLE_BUFF_NUM                               - ����������İ�����?( Ĭ��:5 )
 BLE_TX_NUM_EVENT                           - ���������¼������Է����ٸ����ݰ�( Ĭ��:1 )
 BLE_TX_POWER                               - ���书��( Ĭ��:LL_TX_POWEER_0_DBM (0dBm) )
 
 ��MULTICONN��
 PERIPHERAL_MAX_CONNECTION                  - ����ͬʱ�����ٴӻ���ɫ( Ĭ��:1 )
 CENTRAL_MAX_CONNECTION                     - ����ͬʱ������������ɫ( Ĭ��:3 )

 **********************************************************************/

/*********************************************************************
 * Ĭ������ֵ
 */
#ifndef BLE_MAC
#define BLE_MAC                             FALSE
#endif
#ifndef DCDC_ENABLE
#define DCDC_ENABLE                         TRUE
#endif
#ifndef HAL_SLEEP
#define HAL_SLEEP                           TRUE
#endif
#ifndef SLEEP_RTC_MIN_TIME                   
#define SLEEP_RTC_MIN_TIME                  US_TO_RTC(1000)
#endif
#ifndef SLEEP_RTC_MAX_TIME                   
#define SLEEP_RTC_MAX_TIME                  (RTC_MAX_COUNT - 1000 * 1000 * 30)
#endif
#ifndef WAKE_UP_RTC_MAX_TIME
#define WAKE_UP_RTC_MAX_TIME                US_TO_RTC(1600)
#endif
#ifndef HAL_KEY
#define HAL_KEY                             FALSE
#endif
#ifndef HAL_LED
#define HAL_LED                             FALSE
#endif
#ifndef TEM_SAMPLE
#define TEM_SAMPLE                          FALSE
#endif
#ifndef BLE_CALIBRATION_ENABLE
#define BLE_CALIBRATION_ENABLE              FALSE
#endif
#ifndef BLE_CALIBRATION_PERIOD
#define BLE_CALIBRATION_PERIOD              120000
#endif
#ifndef BLE_SNV
#define BLE_SNV                             FALSE
#endif
#ifndef BLE_SNV_ADDR
#define BLE_SNV_ADDR                        0x77E00-FLASH_ROM_MAX_SIZE
#endif
#ifndef BLE_SNV_BLOCK
#define BLE_SNV_BLOCK                       256
#endif
#ifndef BLE_SNV_NUM
#define BLE_SNV_NUM                         1
#endif
#ifndef CLK_OSC32K
#define CLK_OSC32K                          1   // ���������ڴ��޸ģ������ڹ����������Ԥ�������޸ģ������������ɫ����ʹ���ⲿ32K
#endif
#ifndef BLE_MEMHEAP_SIZE
#define BLE_MEMHEAP_SIZE                    (1024*6)
#endif
#ifndef BLE_BUFF_MAX_LEN
#define BLE_BUFF_MAX_LEN                    27
#endif
#ifndef BLE_BUFF_NUM
#define BLE_BUFF_NUM                        5
#endif
#ifndef BLE_TX_NUM_EVENT
#define BLE_TX_NUM_EVENT                    1
#endif
#ifndef BLE_TX_POWER
#define BLE_TX_POWER                        LL_TX_POWEER_0_DBM
#endif
#ifndef PERIPHERAL_MAX_CONNECTION
#define PERIPHERAL_MAX_CONNECTION           1
#endif
#ifndef CENTRAL_MAX_CONNECTION
#define CENTRAL_MAX_CONNECTION              3
#endif

extern uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];
extern const uint8_t MacAddr[6];

#endif

