/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_dtmlib_dtm DTM - Direct Test Mode
 * @{
 * @ingroup ble_sdk_lib
 * @brief Module for testing RF/PHY using DTM commands.
 */

#ifndef BLE_DTM_H__
#define BLE_DTM_H__

#include <stdint.h>
#include <stdbool.h>


/**@brief Configuration parameters. */
#define DEFAULT_TX_POWER                RADIO_TXPOWER_TXPOWER_0dBm      /**< Default Transmission power using in the DTM module. */
#define DEFAULT_TIMER                   NRF_TIMER0                      /**< Default timer used for timing. */
#define DEFAULT_TIMER_IRQn              TIMER0_IRQn                     /**< IRQ used for timer. NOTE: MUST correspond to DEFAULT_TIMER. */

/**@brief BLE DTM command codes. */
typedef uint32_t dtm_cmd_t;                                             /**< DTM command type. */

#define LE_RESET                        0                               /**< DTM command: Reset device. */
#define LE_RECEIVER_TEST                1                               /**< DTM command: Start receive test. */
#define LE_TRANSMITTER_TEST             2                               /**< DTM command: Start transmission test. */
#define LE_TEST_END                     3                               /**< DTM command: End test and send packet report. */

// Configuration options used as parameter 2
// when cmd == LE_TRANSMITTER_TEST and payload == DTM_PKT_VENDORSPECIFIC
// Configuration value, if any, is supplied in parameter 3

#define CARRIER_TEST                    0                               /**< Length=0 indicates a constant, unmodulated carrier until LE_TEST_END or LE_RESET */
#define CARRIER_TEST_STUDIO             1                               /**< nRFgo Studio uses value 1 in length field, to indicate a constant, unmodulated carrier until LE_TEST_END or LE_RESET */
#define SET_TX_POWER                    2                               /**< Set transmission power, value -40..+4 dBm in steps of 4 */
#define SELECT_TIMER                    3                               /**< Select on of the 16 MHz timers 0, 1 or 2 */

#define LE_PACKET_REPORTING_EVENT       0x8000                          /**< DTM Packet reporting event, returned by the device to the tester. */
#define LE_TEST_STATUS_EVENT_SUCCESS    0x0000                          /**< DTM Status event, indicating success. */
#define LE_TEST_STATUS_EVENT_ERROR      0x0001                          /**< DTM Status event, indicating an error. */

#define DTM_PKT_PRBS9                   0x00                            /**< Bit pattern PRBS9. */
#define DTM_PKT_0X0F                    0x01                            /**< Bit pattern 11110000 (LSB is the leftmost bit). */
#define DTM_PKT_0X55                    0x02                            /**< Bit pattern 10101010 (LSB is the leftmost bit). */
#define DTM_PKT_VENDORSPECIFIC          0xFFFFFFFF                      /**< Vendor specific. Nordic: Continuous carrier test, or configuration. */

/**@brief Return codes from dtm_cmd(). */
#define DTM_SUCCESS                     0x00                            /**< Indicate that the DTM function completed with success. */
#define DTM_ERROR_ILLEGAL_CHANNEL       0x01                            /**< Physical channel number must be in the range 0..39. */
#define DTM_ERROR_INVALID_STATE         0x02                            /**< Sequencing error: Command is not valid now. */
#define DTM_ERROR_ILLEGAL_LENGTH        0x03                            /**< Payload size must be in the range 0..37. */
#define DTM_ERROR_ILLEGAL_CONFIGURATION 0x04                            /**< Parameter out of range (legal range is function dependent). */
#define DTM_ERROR_UNINITIALIZED         0x05                            /**< DTM module has not been initialized by the application. */

// Note: DTM_PKT_VENDORSPECIFIC, is not a packet type
#define PACKET_TYPE_MAX                 DTM_PKT_0X55                    /**< Highest value allowed as DTM Packet type. */

/** @brief BLE DTM event type. */
typedef uint32_t dtm_event_t;                                           /**< Type for handling DTM event. */

/** @brief BLE DTM frequency type. */
typedef uint32_t dtm_freq_t;                                            /**< Physical channel, valid range: 0..39. */

/**@brief BLE DTM packet types. */
typedef uint32_t dtm_pkt_type_t;                                        /**< Type for holding the requested DTM payload type.*/


/**@brief Function for initializing or re-initializing DTM module
 *     
 * @return DTM_SUCCESS on successful initialization of the DTM module.
*/
uint32_t dtm_init(void);

/**@brief Function for resetting the DTM module; behaves the same as the LE_RESET DTM command.
 *     
 * @return DTM_SUCCESS always
*/
uint32_t dtm_reset(void);


/**@brief Function for giving control to dtmlib for handling timer and radio events.
 *        Will return to caller at 625us intervals or whenever another event than radio occurs
 *        (such as UART input). Function will put MCU to sleep between events.
 *
 * @return      Time counter, incremented every 625 us.
 */
uint32_t dtm_wait(void);


/**@brief Function for calling when a complete command has been prepared by the Tester.
 *
 * @param[in]   cmd       One of the DTM_CMD values (bits 14:15 in the 16-bit UART format).
 * @param[in]   freq      Phys. channel no - actual frequency = (2402 + freq * 2) MHz (bits 8:13 in
 *                        the 16-bit UART format).
 * @param[in]   length    Payload length, 0..37 (bits 2:7 in the 16-bit UART format).
 * @param[in]   payload   One of the DTM_PKT values (bits 0:1 in the 16-bit UART format).
 *
 * @return      DTM_SUCCESS or one of the DTM_ERROR_ values
 */
uint32_t dtm_cmd(dtm_cmd_t cmd, dtm_freq_t freq, uint32_t length, dtm_pkt_type_t payload);


/**@brief Function for reading the result of a DTM command
 *
 * @param[out]  p_dtm_event   Pointer to buffer for 16 bit event code according to DTM standard.
 * 
 * @return      true: new event, false: no event since last call, this event has been read earlier
 */
bool dtm_event_get(dtm_event_t * p_dtm_event);


/**@brief Function for configuring the timer to use.
 *
 * @note        Must be called when no DTM test is running. 
 * 
 * @param[in]   new_timer   Index (0..2) of timer to be used by the DTM library
 *
 * @return      true: success, new timer was selected, false: parameter error 
 */
bool dtm_set_timer(uint32_t new_timer); 


/**@brief Function for configuring the transmit power.
 *
 * @note        Must be called when no DTM test is running.
 * 
 * @param[in]   new_tx_power   New output level, +4..-40, in steps of 4.
 *
 * @return      true: tx power setting changed, false: parameter error
 */
bool dtm_set_txpower(uint32_t new_tx_power);

#endif // BLE_DTM_H__

/** @} */
