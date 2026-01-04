/*
 * BTHome_ID.h
 *
 *  Created on: 2024年11月13日
 *      Author: LQY
 */

#ifndef BTHOME_BTHOME_ID_H_
#define BTHOME_BTHOME_ID_H_

//https://bthome.io/format/表格数据类型映射，直接映射需要注意蓝牙接收端和发送端的数据大小端格式是否匹配，对于CH592和ESP32全是小端计算系统,可以映射
#define uint8   uint8_t
#define sint8   int8_t
#define uint16  uint16_t
#define sint16  int16_t
#define uint32  uint32_t
#define sint32  int32_t
// #define uint24  
typedef struct uint24 {
    uint8 data[3];
}uint24 ;

// 以下ID等数据见 https://bthome.io/format/

//宏定义名                     对象ID  // 对象名          翻译       类型     放大系数 单位    二进制数据示例 示例对应的数值
#define ID_acceleration        0x51   // acceleration    加速度     uint16   0.001    m/s2    518756         22.151
#define ID_battery             0x01   // battery         电池       uint8    1        %       0161           97
#define ID_co2                 0x12   // co2             二氧化碳   uint16   1        ppm     12E204         1250
#define ID_conductivity        0x56   // conductivity    电导率     uint16   1        uS/cm   56E803         1000
#define ID_count_uin8          0x09   // count           数数       uint8    1                0960           96
#define ID_count_uin16         0x3D   // count           数数       uint16   1                3D0960         24585
#define ID_count_uin32         0x3E   // count           数数       uint32   1                3E2A2C0960     1611213866
#define ID_count_sint8         0x59   // count           数数       sint8    1                59EA           -22
#define ID_count_sint16        0x5A   // count           数数       sint16   1                5AEAEA         -5398
#define ID_count_sint32        0x5B   // count           数数       sint32   1                5BEA0234EA     -365690134
#define ID_current             0x43   // current         电流       uint16   0.001    A       434E34         13.39
#define ID_current_signed      0x5D   // current         电流       sint16   0.001    A       5D02EA         -5.63
#define ID_dewpoint            0x08   // dewpoint        露点       sint16   0.01     °C     08CA06         17.38
#define ID_distance_mm         0x40   // distance_mm     距离       uint16   1        mm      400C00         12
#define ID_distance_m          0x41   // distance_m      距离       uint16   0.1      m       414E00         7.8
#define ID_duration            0x42   // duration        期间       uint24   0.001    s       424E3400       13.390
#define ID_energy              0x4D   // energy          功率       uint32   0.001    kWh     4d12138a14     344593.170
#define ID_energy_uint24       0x0A   // energy          功率       uint24   0.001    kWh     0A138A14       1346.067
#define ID_gas                 0x4B   // gas             气体       uint24   0.001    m3      4B138A14       1346.067
#define ID_gas_precise         0x4C   // gas             气体       uint32   0.001    m3      4C41018A01     25821.505
#define ID_gyroscope           0x52   // gyroscope       陀螺仪     uint16   0.001    °/s    528756         22.151
#define ID_humidity_precise    0x03   // humidity        湿度       uint16   0.01     %       03BF13         50.55
#define ID_humidity            0x2E   // humidity        湿度       uint8    1        %       2E23           35
#define ID_illuminance         0x05   // illuminance     照度       uint24   0.01     lux     05138A14       13460.67
#define ID_mass_kg             0x06   // mass_kg         质量       uint16   0.01     kg      065E1F         80.3
#define ID_mass_lb             0x07   // mass_lb         质量       uint16   0.01     lb      073E1D         74.86
#define ID_moisture_precise    0x14   // moisture        水分       uint16   0.01     %       14020C         30.74
#define ID_moisture            0x2F   // moisture        水分       uint8    1        %       2F23           35
#define ID_pm2_5               0x0D   // pm2.5           pm2.5      uint16   1        ug/m3   0D120C         3090
#define ID_pm10                0x0E   // pm10            pm10       uint16   1        ug/m3   0E021C         7170
#define ID_power               0x0B   // power           力量       uint24   0.01     W       0B021B00       69.14
#define ID_power_signed        0x5C   // power           力量       sint32   0.01     W       5C02FBFFFF     -12.78
#define ID_pressure            0x04   // pressure        压力       uint24   0.01     hPa     04138A01       1008.83
#define ID_rotation            0x3F   // rotation        旋转       sint16   0.1      °      3F020C         307.4
#define ID_speed               0x44   // speed           速度       uint16   0.01     m/s     444E34         133.90
#define ID_temperature_rough   0x57   // temperature     温度       sint8    1        °C     57EA           -22
#define ID_temperature_F       0x58   // temperature     温度       sint8    0.35     °C     58EA           -7.7
#define ID_temperature         0x45   // temperature     温度       sint16   0.1      °C     451101         27.3
#define ID_temperature_precise 0x02   // temperature     温度       sint16   0.01     °C     02CA09         25.06
#define ID_tvoc                0x13   // tvoc            TVOC       uint16   1        ug/m3   133301         307
#define ID_voltage_precise     0x0C   // voltage         电压       uint16   0.001    V       0C020C         3.074
#define ID_voltage             0x4A   // voltage         电压       uint16   0.1      V       4A020C         307.4
#define ID_volume_precise      0x4E   // volume          体积       uint32   0.001    L       4E87562A01     19551.879
#define ID_volume              0x47   // volume          体积       uint16   0.1      L       478756         2215.1
#define ID_volume_mL           0x48   // volume          体积       uint16   1        mL      48DC87         34780
#define ID_volume_storage      0x55   // volume_storage  卷存储     uint32   0.001    L       5587562A01     19551.879
#define ID_volume_flow_rate    0x49   // volume_flow_rate体积流量   uint16   0.001    m3/hr   49DC87         34.780
#define ID_UV_index            0x46   // UV_index        紫外线指数 uint8    0.1              4632           5.0
#define ID_water               0x4F   // water           水         uint32   0.001    L       4F87562A01     19551.879


//宏定义名                     对象ID  // 对象名          翻译       类型     二进制数据示例                  示例对应的意义
#define ID_timestamp           0x50	  // timestamp       时间戳	   uint32	 505d396164	                    见下文
// 时间戳传感器需要更多解释。时间戳传感器定义为自 1970-1-1, 00:00:00; UTC(也称为纪元时间)以来的秒数.并返回时区为 UTC 的日期时间对象。
// 二进制数据示例中数值 0x5D396164 从字节(小端)转换为整数后为 1684093277秒，对应于 2023-5-14,19:41:17。

//宏定义名                     对象ID  // 对象名          翻译              类型          二进制数据示例                  示例对应的意义
#define ID_raw                 0x54	  // raw             未经处理的信息	   可变长度	      540C48656C6C6F20576F726C6421	 48656c6c6f20576f726c6421
#define ID_text                0x53	  // text            文本	          可变长度 	     530C48656C6C6F20576F726C6421	Hello World!
// text 和 raw 传感器的长度可变。因此，您必须在 '对象ID' 之后的第一个字节中指定长度。
// 在示例 0x530C48656C6C6F20576F726C6421 中,第2个字节 (0x0C) 表示文本字符串的长度(12 字节)。
// 文本必须以 UTF-8 编码。示例中二进制串 0x48656C6C6F20576F726C6421 代表字符串 "Hello World!"。


//宏定义名                     对象ID  //  对象名            翻译       类型    数值0的意义            数值1的意义           二进制数据示例
#define STATE_bbattery         0x15   //  battery           电池       uint8   0（False=正常）        1（True=低）          1501
#define STATE_battery_charging 0x16   //  battery_charging  电池充电   uint8   0（False=未充电）      1（True=正在充电）    1601
#define STATE_carbon_monoxide  0x17   //  carbon_monoxide   一氧化碳   uint8   0（False=未检测到）    1（True=检测到）      1700
#define STATE_cold             0x18   //  cold              冷         uint8   0（False=正常）        1（True=冷）          1801
#define STATE_connectivity     0x19   //  connectivity      连接       uint8   0（False=断开连接）    1（True=已连接）      1900
#define STATE_door             0x1A   //  door              门         uint8   0（False=已关闭）      1（True=已打开）      1A00
#define STATE_garage_door      0x1B   //  garage_door       车库门     uint8   0（False=已关闭）      1（True=已打开）      1B01
#define STATE_gas              0x1C   //  gas               气         uint8   0（False=清除）        1（True=检测到）      1C01
#define STATE_generic_boolean  0x0F   //  generic_boolean   通用布尔值 uint8   0（False=Off）         1（True=On）          0F01
#define STATE_heat             0x1D   //  heat              热         uint8   0（False=正常）        1（True=热）          1D00
#define STATE_light            0x1E   //  light             光         uint8   0（False=无光）        1（True=检测到光线）  1E01
#define STATE_lock             0x1F   //  lock              锁         uint8   0（False=锁定）        1（True=解锁）        1F01
#define STATE_moisture         0x20   //  moisture          湿气       uint8   0（False=干）          1（True=湿）          2001
#define STATE_motion           0x21   //  motion            运动       uint8   0（False=清除）        1（True=检测到）      2100
#define STATE_moving           0x22   //  moving            移动       uint8   0（False=不移动）      1（True=移动）        2201
#define STATE_occupancy        0x23   //  occupancy         占有       uint8   0（False=清除）        1（True=检测到）      2301
#define STATE_opening          0x11   //  opening           开放       uint8   0（False=已关闭）      1（True=打开）        1100
#define STATE_plug             0x24   //  plug              插头       uint8   0（False=未插电）      1（True=已接通）      2400
#define STATE_power            0x10   //  power             电力       uint8   0（False=关闭）        1（True=打开）        1001
#define STATE_presence         0x25   //  presence          存在       uint8   0（False=离开）        1（True=存在）        2500
#define STATE_problem          0x26   //  problem           问题       uint8   0（False=没问题）      1（True=有问题）      2601
#define STATE_running          0x27   //  running           运行       uint8   0（False=未运行）      1（True=正在运行）    2701
#define STATE_safety           0x28   //  safety            安全       uint8   0（False=不安全）      1（True=安全）        2800
#define STATE_smoke            0x29   //  smoke             烟         uint8   0（False=清洁的）      1（True=检测到）      2901
#define STATE_sound            0x2A   //  sound             声音       uint8   0（False=安静的）      1（True=检测到）      2A00
#define STATE_tamper           0x2B   //  tamper            篡改       uint8   0（False=Off）         1（True=On）          2B00
#define STATE_vibration        0x2C   //  vibration         振动       uint8   0（False=无）          1（True=检测到）      2C01
#define STATE_window           0x2D   //  window            窗         uint8   0（False=已关闭）      1（True=打开）        2D01

//宏定义名                                  事件ID   事件名 翻译    编号    事件类型   示例   示例对应的事件
#define EVENT_button                        0x3A  // button 按钮
#define EVENT_BUTTON_NONE                                           0x00  //未触发     3A00   未触发
#define EVENT_BUTTON_PRESS                                          0x01  //按         3A01   按
#define EVENT_BUTTON_DOUBLE_PRESS                                   0x02  //双击       3A02   双击
#define EVENT_BUTTON_TRIPLE_PRESS                                   0x03  //三重按下   3A03   三重按下
#define EVENT_BUTTON_LONG_PRESS                                     0x04  //长按       3A04   长按
#define EVENT_BUTTON_LONG_DOUBLE_PRESS                              0x05  //长按两次   3A05   长按两次
#define EVENT_BUTTTON_LONG_TRIPLE_PRESS                             0x06  //长按三次   3A06   长按三次
#define EVENT_BUTTTON_HOLD_PRESS                                    0x80  //按住按下   3A80   按住按下


//宏定义名                   事件ID    事件名 翻译     编号   事件类型   参数    示例           示例对应的事件
#define EVENT_dimmer         0x3C   //dimmer 调光器
#define EVENT_DIMMER_NONE                            0x00 //未触发             3C0000         未触发
#define EVENT_DIMMER_LEFT                            0x01 //向左旋转   uint8   3C0103         向左旋转3步
#define EVENT_DIMMER_RIGHT                           0x02 //向右旋转   uint8   3C020A         向右旋转10步

#endif /* BTHOME_BTHOME_ID_H_ */
