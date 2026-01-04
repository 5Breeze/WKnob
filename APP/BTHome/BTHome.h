/*
 * BTHome.h
 *
 *  Created on: 2024年10月26日
 *      Author: LQY
 */

#ifndef BTHOME_BTHOME_H_
#define BTHOME_BTHOME_H_


#pragma pack(1) //关闭内存对齐, 将结构体的对齐方式设置为1字节对齐


/**********BTHome数据加密设置************/
//是否启用BTHome数据加密
#define BTHOME_ENABLE_ENCRYPT
//设置BTHome的16字节秘钥,在这里输入其二进制数据对应的字符串(仅由数字字符和A-F组成,此字符串即为homeassistant的BTHome配置界面要求输入的秘钥)
static const uint8_t bthome_aes_key[32] = "48656C6C6F204254486F6D65210D0A00";   //字符串"Hello BTHome!\r\n\0"的16进制串

/**********BTHome蓝牙设备名称设置************/
//注:蓝牙广播包长度为31字节,对于BTHome设备,不加密且带蓝牙名称时最长可以使用字节数为31-10=21字节(名称长度+传感器数据大小)
//若若单个数据包需要携带多个传感器数据时,蓝牙设备名称不宜过长。
//#define BTHOME_NAME  ""  //蓝牙设备名称,注释该宏定义时,蓝牙广播包将不附加设备名称数据

/**********BThome的传感器数据结构体设置************/
//BThome的传感器数据对象编号(Object id)  ,表格详细见https://bthome.io/format/
#include "BTHome_id.h"

//// 定义电压数据结构
typedef struct {    uint8 id;    uint16 data;   } VoltageData;
static const VoltageData VoltageDataInit = { ID_voltage_precise  , 0 };
// 定义温度数据结构
typedef struct {    uint8 id;    sint16 data;   } TemperatureData;
static const TemperatureData TemperatureDataInit = { ID_temperature_precise  , 0 };
// 定义湿度数据结构
typedef struct {    uint8 id;    uint16 data;   } HumidityData;
static const HumidityData HumidityDataInit = { ID_humidity_precise  , 0 };
// 定义光照强度数据结构
typedef struct {    uint8 id;    uint24 data;   } IlluminanceData;
static const IlluminanceData IlluminanceDataInit = { ID_illuminance  , 0 };
// 定义门状态数据结构
typedef struct {    uint8 id;    uint8 data;    } DoorData;
static const DoorData DoorDataInit = { STATE_door  , 0 };
// 定义按钮状态数据结构
typedef struct {    uint8 id;    uint8 data;    } ButtonData;
static const ButtonData ButtonDataInit = { EVENT_button  , 0 };

//BThome的传感器数据结构体(service data),如何定义见https://bthome.io/format/ 的BTHome Data format段
typedef union BTHomeV2_sensor_type {

    //周期唤醒时,蓝牙广播发送的传感器数据数据结构体
    struct cycle {
        VoltageData voltage;
        IlluminanceData illuminance;
        TemperatureData temperature;
        HumidityData humidity;
        DoorData door;
    }  cycle ;

    //中断唤醒时,蓝牙广播发送的传感器数据数据结构体
    struct interrupt {
        VoltageData voltage;
        IlluminanceData illuminance;
        DoorData door;
        ButtonData button;
    }  interrupt ;
}BTHomeV2_sensor_type ;

//BThome的传感器数据结构体初始化值

//中断唤醒时,蓝牙广播发送的传感器数据数据结构体初始值
static const BTHomeV2_sensor_type BTHomeV2_sensor_cycle_init_vaule = {
    .cycle.voltage        = VoltageDataInit ,
    .cycle.temperature    = TemperatureDataInit ,
    .cycle.humidity       = HumidityDataInit ,
    .cycle.illuminance    = IlluminanceDataInit ,
    .cycle.door           = DoorDataInit ,
};

//中断唤醒时,蓝牙广播发送的传感器数据数据结构体初始值
static const BTHomeV2_sensor_type BTHomeV2_sensor_interrupt_init_vaule = {
    .interrupt.voltage        = VoltageDataInit ,
    .interrupt.illuminance    = IlluminanceDataInit ,
    .interrupt.door           = DoorDataInit ,
    .interrupt.button         = ButtonDataInit ,
};


/*******************************无需更改/自动计算的数据**************************************/
//蓝牙角色数据 对于BTHome设备是固定的 0x02 0x01 0x06
#define BTHome_FLAGS_len  2
#define BTHome_GAP_ADTYPE_FLAGS 0x0601              //标记该设备角色：仅广播无法被连接  (无需更改)
//蓝牙设备名称数据
#ifdef BTHOME_NAME
    #define BTHome_NAME_len (BTHOME_NAME_LEN )       //数据长度计算: 名称长度 + GAP类型1字节
    #define BTHome_GAP_ADTYPE_LOCAL_NAME_COMPLETE 0x09  //广播数据类型：完整设备名称
    #define BTHOME_NAME_LEN (sizeof(BTHOME_NAME))     //蓝牙设备名称字符长度计算宏定义
#endif
//BTHome服务数据
#define BTHomeV2_SENSOR_DATA_len sizeof(BTHomeV2_sensor_type)
#define BTHome_SERVICE_DATA_len (sizeof(bthome_server_data_type)-1)
#define BTHome_GAP_ADTYPE_SERVICE_DATA 0x16         //广播数据类型：服务数据
#define BTHome_UUID_16Bit 0xfcd2                    //BTHome的16位UUID
#ifdef BTHOME_ENABLE_ENCRYPT
#define BTHomeV2_device_information ((1<<0)|(2<<5)) //BTHome 设备信息数据(bit0:是否为加密格式, bit2触发器的设备标志,bit5~7BTHome数据版本)
#else
#define BTHomeV2_device_information ((0<<0)|(2<<5)) //BTHome 设备信息数据(bit0:是否为加密格式, bit2触发器的设备标志,bit5~7BTHome数据版本)
#endif


//BTHome服务数据
typedef struct bthome_server_data_type{
    const uint8   len ;
    const uint8   GAP_server_data ;
    const uint16  UUID_16Bit;
    const uint8   device_information;
    BTHomeV2_sensor_type sensor ;
#ifdef BTHOME_ENABLE_ENCRYPT
    uint32  counter ;   //加密格式的 计数器
    uint8   mic[4]  ;   //加密格式的 消息完整性校验数据
#endif
}bthome_server_data_type ;

typedef struct BTHome_advertData_type{
    //蓝牙角色数据
    struct flags_type{
        const uint8  len ;
        const uint16 GAP_AdType_flags;
    }head;
    //BTHome服务数据
    bthome_server_data_type server;
#ifdef BTHOME_NAME
    //蓝牙设备名称数据
    struct local_name_type{
        const uint8  len ;
        const uint8  GAP_local_name_complete;
        const sint8   name[BTHOME_NAME_LEN];
    }name;
#endif
}BTHome_advertData_type;

#include <assert.h>
//注:蓝牙广播包长度为31字节 ，若需要发送的传感器数据较多时,蓝牙设备名称不宜过长，或者分为多个数据包来发送
//BTHome设备带蓝牙名称、不加密时 最长可以使用字节数为31-10=21字节(名称长度+传感器数据大小)
//编译时校验蓝牙广播数据包长度,不得超过31字节
_Static_assert(sizeof(BTHome_advertData_type) <= 31, "advertData size exceeds 31 bytes 蓝牙广播数据包超过31字节");

//BThome结构体数据初始值
static const BTHome_advertData_type  BTHome_init_vaule = {
 .head  = { BTHome_FLAGS_len ,
            BTHome_GAP_ADTYPE_FLAGS                     },
 .server= { .len = BTHome_SERVICE_DATA_len,
            .GAP_server_data = BTHome_GAP_ADTYPE_SERVICE_DATA,
            .UUID_16Bit = BTHome_UUID_16Bit,
            .device_information = BTHomeV2_device_information,
            .sensor = BTHomeV2_sensor_cycle_init_vaule,       },
#ifdef BTHOME_NAME
 .name  = { BTHome_NAME_len,
            BTHome_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
            .name = BTHOME_NAME                         },
#endif
};

#pragma pack() //恢复编译器的默认内存对齐设置

//按照输入的传感器数据来刷新并加密广播数据包
int bthome_sensor_update( const uint8 *mac, BTHomeV2_sensor_type sensor ,  BTHome_advertData_type *advertData );
//加密秘钥初始化
int bthome_encrypt_init();

void set_uint24(uint24* data,uint32 vaule);
#endif /* BTHOME_BTHOME_H_ */
