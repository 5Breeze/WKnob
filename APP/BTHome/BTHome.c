/*
 * BTHome.c
 *
 *  Created on: 2024骞�10鏈�27鏃�
 *      Author: LQY
 */

#include "CH59x_common.h"
#include "BTHome.h"

void set_uint24(uint24* data,uint32 value){
    if (data == NULL || value > 0xFFFFFF) {
        return; // 妫€鏌ヨ緭鍏ュ弬鏁帮紝纭�淇濅笉瓒呭嚭24浣嶈寖鍥�
    }
    data->data[0] = value & 0xFF;        // 浣庡瓧鑺�
    data->data[1] = (value >> 8) & 0xFF; // 涓�闂村瓧鑺�
    data->data[2] = (value >> 16) & 0xFF; // 楂樺瓧鑺�
}

#ifdef BTHOME_ENABLE_ENCRYPT
#include "mbedtls/include/mbedtls/ccm.h"
#include "stdlib.h"
mbedtls_ccm_context m_encryptCTX; //ccm妯″紡鐨勫姞瀵嗙�橀挜鏁版嵁缁撴瀯
// 灏�16杩涘埗瀛楃�︿覆杞�鎹�涓哄瓧鑺傛暟缁�
void convert_hex_string_to_bytes(const char *hex_string, uint8_t *output_array, size_t output_len) {
  for (size_t i = 0; i < output_len; i++) {
    // 鍒涘缓鐢ㄤ簬瀛樺偍涓や釜瀛楃�︾殑瀛愬瓧绗︿覆
    char hex_str[3] = { hex_string[i * 2], hex_string[i * 2 + 1], '\0' }; // 灏�16杩涘埗瀛楃�︿覆杞�鎹�涓烘暣鏁�
    output_array[i] = (uint8_t)strtol(hex_str, NULL, 16);
  }
}
//鑾峰彇鏈�鏈簃ac鍦板潃, 鍔犲瘑绉橀挜鍒濆�嬪寲
int bthome_encrypt_init()
{
    uint8_t hex_bthome_aes_key[16];
    convert_hex_string_to_bytes(bthome_aes_key, hex_bthome_aes_key , 16 );
    mbedtls_ccm_init(&m_encryptCTX);
	  return mbedtls_ccm_setkey( &m_encryptCTX, MBEDTLS_CIPHER_ID_AES, hex_bthome_aes_key, sizeof(hex_bthome_aes_key) * 8);
}
//鑾峰彇nonce闅忔満鏁�,鍙傜収 https://bthome.io/encryption/ 鑾峰彇鍏堕殢鏈烘暟 nonce
void bthome_get_nonce(const uint8 mac[6], bthome_server_data_type server , uint8 nonce[13]){
	memcpy(nonce,mac,6);
	memcpy(nonce+6,&server.UUID_16Bit,2);
	memcpy(nonce+8,&server.device_information,1);
	memcpy(nonce+9,&server.counter,4);
}
/// @brief 鍔犲瘑鏁版嵁
/// @param mac 			鏈�鏈虹殑钃濈墮MAC鍦板潃
/// @param key_sched 	绉橀挜鍒濆�嬪寲鍚庢寚閽�
/// @param server_in 	杈撳叆鐨刡thome鏈嶅姟鏁版嵁
/// @param server_out 	杈撳嚭鐨刡thome鏈嶅姟鏁版嵁
/// @return 
int bthome_encryption(const uint8 mac[6] , mbedtls_ccm_context *ctx , bthome_server_data_type server_in, bthome_server_data_type *server_out )
{
    uint8 nonce[13] ;
    bthome_get_nonce( mac, server_in ,nonce);  //鑾峰彇nonce闅忔満鏁�
    // 鍔犲瘑
    return mbedtls_ccm_encrypt_and_tag( ctx, sizeof(BTHomeV2_sensor_type) , nonce, sizeof(nonce), 0, 0,
                                        (uint8_t *)&server_in.sensor, (uint8_t *)&server_out->sensor, (uint8_t *)&server_out->mic,
                                          sizeof(server_out->mic));
}
//鎸夌収杈撳叆鐨勪紶鎰熷櫒鏁版嵁鏉ュ埛鏂板苟鍔犲瘑骞挎挱鏁版嵁鍖�
int bthome_sensor_update( const uint8 *mac, BTHomeV2_sensor_type sensor ,  BTHome_advertData_type *advertData ){
  memcpy(&advertData->server.sensor,&sensor,sizeof(BTHomeV2_sensor_type));
	advertData->server.counter ++ ;
  //浣跨敤绉橀挜 m_encryptCTX 鍔犲瘑鏁版嵁
  return bthome_encryption( mac , &m_encryptCTX, advertData->server, &advertData->server ); 

}
#else
//鎸夌収杈撳叆鐨勪紶鎰熷櫒鏁版嵁鏉ュ埛鏂板苟鍔犲瘑骞挎挱鏁版嵁鍖�
int bthome_sensor_update( const uint8 *mac, BTHomeV2_sensor_type sensor ,  BTHome_advertData_type *advertData ){
    memcpy(&advertData->server.sensor,&sensor,sizeof(BTHomeV2_sensor_type)) ;
    return 0 ;
}
int bthome_encrypt_init(){
    return 0 ;
}

#endif
