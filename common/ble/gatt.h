/** @file gatt.h
*
* @brief GATT utility commands
* @par
* COPYRIGHT NOTICE: (c) Rigado
*
* All rights reserved. */

#ifndef GATT_H_
#define GATT_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
void gatt_set_runtime_mtu(uint32_t mtu);  //TP changed unit16_t to uint32_t   05272020
uint32_t gatt_get_runtime_mtu(void);    //TP changed unit16_t to uint32_t

#endif
