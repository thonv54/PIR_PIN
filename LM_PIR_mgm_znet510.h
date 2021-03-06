// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __APP_LM_PIR_MGM_ZNET510_H__
#define __APP_LM_PIR_MGM_ZNET510_H__


/**** Included Header Section ****/

/**** ZCL Section ****/
#define ZA_PROMPT "LM_PIR_mgm_znet510"
#define ZCL_USING_BASIC_CLUSTER_SERVER
#define ZCL_USING_IDENTIFY_CLUSTER_CLIENT
#define ZCL_USING_IDENTIFY_CLUSTER_SERVER
#define ZCL_USING_ON_OFF_CLUSTER_CLIENT
#define ZCL_USING_THERMOSTAT_CLUSTER_SERVER
#define ZCL_USING_ILLUM_MEASUREMENT_CLUSTER_SERVER
#define ZCL_USING_TEMP_MEASUREMENT_CLUSTER_SERVER
#define ZCL_USING_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_SERVER
#define ZCL_USING_IAS_ZONE_CLUSTER_SERVER
#define ZCL_USING_IAS_WD_CLUSTER_SERVER
#define ZCL_USING_LM_DEV_OPT_SERVER
/**** Optional Attributes ****/
#define ZCL_USING_BASIC_CLUSTER_MANUFACTURER_NAME_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_MODEL_IDENTIFIER_ATTRIBUTE 
#define EMBER_AF_DEFAULT_RESPONSE_POLICY_CONDITIONAL

/**** Cluster endpoint counts ****/
#define EMBER_AF_BASIC_CLUSTER_SERVER_ENDPOINT_COUNT (7)
#define EMBER_AF_IDENTIFY_CLUSTER_CLIENT_ENDPOINT_COUNT (7)
#define EMBER_AF_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT (7)
#define EMBER_AF_ON_OFF_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_THERMOSTAT_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_ILLUM_MEASUREMENT_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_TEMP_MEASUREMENT_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_IAS_ZONE_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_IAS_WD_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_LM_DEV_OPT_SERVER_ENDPOINT_COUNT (1)

/**** CLI Section ****/
#define EMBER_AF_GENERATE_CLI
#define EMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD

/**** Security Section ****/
#define EMBER_AF_HAS_SECURITY_PROFILE_HA

/**** Network Section ****/
#define EMBER_SUPPORTED_NETWORKS (1)
#define EMBER_AF_NETWORK_INDEX_PRIMARY (0)
#define EMBER_AF_DEFAULT_NETWORK_INDEX EMBER_AF_NETWORK_INDEX_PRIMARY
#define EMBER_AF_HAS_END_DEVICE_NETWORK
#define EMBER_AF_HAS_SLEEPY_NETWORK
#define EMBER_AF_TX_POWER_MODE EMBER_TX_POWER_MODE_USE_TOKEN

/**** HAL Section ****/

/**** Callback Section ****/
#define EMBER_CALLBACK_HAL_BUTTON_ISR
#define EMBER_CALLBACK_MAIN_INIT
#define EMBER_CALLBACK_PRE_MSG
#define EMBER_CALLBACK_STACK_STATUS
#define EMBER_CALLBACK_INCOMING_COMMAND_HANDLER
#define EMBER_APPLICATION_HAS_INCOMING_COMMAND_HANDLER
#define EMBER_CALLBACK_LM_DEV_OPT_LM_DEV_OPT_SERVER_ATTRIBUTE_CHANGED
#define EMBER_CALLBACK_COUNTER_HANDLER
#define EMBER_APPLICATION_HAS_COUNTER_HANDLER
#define EMBER_CALLBACK_EZSP_COUNTER_ROLLOVER_HANDLER
#define EZSP_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER
#define EMBER_CALLBACK_POLL_COMPLETE
#define EMBER_APPLICATION_HAS_POLL_COMPLETE_HANDLER
#define EMBER_CALLBACK_PRE_NCP_RESET
#define EMBER_CALLBACK_EZSP_POLL_COMPLETE
#define EZSP_APPLICATION_HAS_POLL_COMPLETE_HANDLER
#define EMBER_CALLBACK_ADD_TO_CURRENT_APP_TASKS
#define EMBER_CALLBACK_REMOVE_FROM_CURRENT_APP_TASKS
#define EMBER_CALLBACK_GET_CURRENT_APP_TASKS
#define EMBER_CALLBACK_GET_LONG_POLL_INTERVAL_MS
#define EMBER_CALLBACK_GET_LONG_POLL_INTERVAL_QS
#define EMBER_CALLBACK_SET_LONG_POLL_INTERVAL_MS
#define EMBER_CALLBACK_SET_LONG_POLL_INTERVAL_QS
#define EMBER_CALLBACK_GET_SHORT_POLL_INTERVAL_MS
#define EMBER_CALLBACK_GET_SHORT_POLL_INTERVAL_QS
#define EMBER_CALLBACK_SET_SHORT_POLL_INTERVAL_MS
#define EMBER_CALLBACK_SET_SHORT_POLL_INTERVAL_QS
#define EMBER_CALLBACK_GET_CURRENT_POLL_INTERVAL_MS
#define EMBER_CALLBACK_GET_CURRENT_POLL_INTERVAL_QS
#define EMBER_CALLBACK_GET_WAKE_TIMEOUT_MS
#define EMBER_CALLBACK_GET_WAKE_TIMEOUT_QS
#define EMBER_CALLBACK_SET_WAKE_TIMEOUT_MS
#define EMBER_CALLBACK_SET_WAKE_TIMEOUT_QS
#define EMBER_CALLBACK_GET_WAKE_TIMEOUT_BITMASK
#define EMBER_CALLBACK_SET_WAKE_TIMEOUT_BITMASK
#define EMBER_CALLBACK_GET_CURRENT_POLL_CONTROL
#define EMBER_CALLBACK_GET_DEFAULT_POLL_CONTROL
#define EMBER_CALLBACK_SET_DEFAULT_POLL_CONTROL
#define EMBER_CALLBACK_START_MOVE
#define EMBER_CALLBACK_STOP_MOVE
#define EMBER_CALLBACK_GET_CURRENT_SLEEP_CONTROL
#define EMBER_CALLBACK_GET_DEFAULT_SLEEP_CONTROL
#define EMBER_CALLBACK_SET_DEFAULT_SLEEP_CONTROL
#define EMBER_CALLBACK_UNUSED_PAN_ID_FOUND
#define EMBER_CALLBACK_SCAN_ERROR
#define EMBER_CALLBACK_FIND_UNUSED_PAN_ID_AND_FORM
#define EMBER_CALLBACK_START_SEARCH_FOR_JOINABLE_NETWORK
#define EMBER_CALLBACK_GET_FORM_AND_JOIN_EXTENDED_PAN_ID
#define EMBER_CALLBACK_SET_FORM_AND_JOIN_EXTENDED_PAN_ID
#define EMBER_CALLBACK_CONFIGURE_REPORTING_COMMAND
#define EMBER_CALLBACK_READ_REPORTING_CONFIGURATION_COMMAND
#define EMBER_CALLBACK_CLEAR_REPORT_TABLE
#define EMBER_CALLBACK_REPORTING_ATTRIBUTE_CHANGE
#define EMBER_CALLBACK_ENERGY_SCAN_RESULT
#define EMBER_CALLBACK_SCAN_COMPLETE
#define EMBER_CALLBACK_NETWORK_FOUND
/**** Debug printing section ****/

// Global switch
// #define EMBER_AF_PRINT_ENABLE
// Individual areas
#define EMBER_AF_PRINT_CORE 0x0001
#define EMBER_AF_PRINT_APP 0x0002
#define EMBER_AF_PRINT_ATTRIBUTES 0x0004
#define EMBER_AF_PRINT_BITS { 0x07 }
#define EMBER_AF_PRINT_NAMES { \
  "Core",\
  "Application",\
  "Attributes",\
  NULL\
}
#define EMBER_AF_PRINT_NAME_NUMBER 3


#define EMBER_AF_SUPPORT_COMMAND_DISCOVERY


// Generated plugin macros

// Use this macro to check if Binding Table Library plugin is included
#define EMBER_AF_PLUGIN_BINDING_TABLE_LIBRARY
// User options for plugin Binding Table Library
#define EMBER_BINDING_TABLE_SIZE 12

// Use this macro to check if Debug Basic Library plugin is included
#define EMBER_AF_PLUGIN_DEBUG_BASIC_LIBRARY

// Use this macro to check if Packet Validate Library plugin is included
#define EMBER_AF_PLUGIN_PACKET_VALIDATE_LIBRARY

// Use this macro to check if Security Core Library plugin is included
#define EMBER_AF_PLUGIN_SECURITY_LIBRARY_CORE
// User options for plugin Security Core Library
#define EMBER_TRANSIENT_KEY_TIMEOUT_S 300

// Use this macro to check if ZigBee PRO Stack Library plugin is included
#define EMBER_AF_PLUGIN_ZIGBEE_PRO_LIBRARY
// User options for plugin ZigBee PRO Stack Library
#define EMBER_MAX_END_DEVICE_CHILDREN 6
#define EMBER_PACKET_BUFFER_COUNT 75
#define EMBER_END_DEVICE_POLL_TIMEOUT 5
#define EMBER_END_DEVICE_POLL_TIMEOUT_SHIFT 6
#define EMBER_APS_UNICAST_MESSAGE_COUNT 10

// Use this macro to check if Address Table plugin is included
#define EMBER_AF_PLUGIN_ADDRESS_TABLE
// User options for plugin Address Table
#define EMBER_AF_PLUGIN_ADDRESS_TABLE_SIZE 2
#define EMBER_AF_PLUGIN_ADDRESS_TABLE_TRUST_CENTER_CACHE_SIZE 2

// Use this macro to check if Counters plugin is included
#define EMBER_AF_PLUGIN_COUNTERS
// User options for plugin Counters

// Use this macro to check if Ember Minimal Printf plugin is included
#define EMBER_AF_PLUGIN_EMBER_MINIMAL_PRINTF

// Use this macro to check if End Device Support plugin is included
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT
// User options for plugin End Device Support
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_SHORT_POLL_INTERVAL_SECONDS 1
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_LONG_POLL_INTERVAL_SECONDS 300
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_SECONDS 2
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_BITMASK 24
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS 100
#define EMBER_AF_REJOIN_ATTEMPTS_MAX 255

// Use this macro to check if Form and Join Library plugin is included
#define EMBER_AF_PLUGIN_FORM_AND_JOIN

// Use this macro to check if Network Find plugin is included
#define EMBER_AF_PLUGIN_NETWORK_FIND
#define EMBER_AF_DISABLE_FORM_AND_JOIN_TICK
// User options for plugin Network Find
#define EMBER_AF_PLUGIN_NETWORK_FIND_CHANNEL_MASK 0x0318C800
#define EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_POWER 10
#define EMBER_AF_PLUGIN_NETWORK_FIND_EXTENDED_PAN_ID { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define EMBER_AF_PLUGIN_NETWORK_FIND_DURATION 5
#define EMBER_AF_PLUGIN_NETWORK_FIND_JOINABLE_SCAN_TIMEOUT_MINUTES 1

// Use this macro to check if Reporting plugin is included
#define EMBER_AF_PLUGIN_REPORTING
// User options for plugin Reporting
#define EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE 10

// Use this macro to check if Scan Dispatch plugin is included
#define EMBER_AF_PLUGIN_SCAN_DISPATCH
// User options for plugin Scan Dispatch
#define EMBER_AF_PLUGIN_SCAN_DISPATCH_SCAN_QUEUE_SIZE 10

// Use this macro to check if Simple Main plugin is included
#define EMBER_AF_PLUGIN_SIMPLE_MAIN

// Use this macro to check if Idle/Sleep plugin is included
#define EMBER_AF_PLUGIN_IDLE_SLEEP
// User options for plugin Idle/Sleep
#define EMBER_AF_PLUGIN_IDLE_SLEEP_MINIMUM_SLEEP_DURATION_MS 5

// Use this macro to check if ADC plugin is included
#define EMBER_AF_PLUGIN_ADC

// Use this macro to check if Antenna Stub plugin is included
#define EMBER_AF_PLUGIN_ANTENNA_STUB

// Use this macro to check if RAIL Library plugin is included
#define EMBER_AF_PLUGIN_RAIL_LIBRARY

// Use this macro to check if Debug JTAG plugin is included
#define EMBER_AF_PLUGIN_DEBUG_JTAG

// Use this macro to check if HAL Configuration plugin is included
#define EMBER_AF_PLUGIN_HAL_CONFIG
// User options for plugin HAL Configuration
#define HAL_CONFIG_PLUGIN_OPTIONS_ENABLE
#define EMBER_AF_PLUGIN_HAL_CONFIG_ANTDIV_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_PTA_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_RHO_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_RX_WAKE_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_VCOM_ENABLE 1
#define EMBER_AF_PLUGIN_HAL_CONFIG_VUART_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_WDOG_ENABLE 1
#define EMBER_AF_PLUGIN_HAL_CONFIG_LEUART0_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_LEUART0_FLOW_CONTROL HAL_USART_FLOW_CONTROL_NONE
#define EMBER_AF_PLUGIN_HAL_CONFIG_LEUART1_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_LEUART1_FLOW_CONTROL HAL_USART_FLOW_CONTROL_NONE
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART0_ENABLE 1
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART0_FLOW_CONTROL HAL_USART_FLOW_CONTROL_HW
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART1_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART1_FLOW_CONTROL HAL_USART_FLOW_CONTROL_NONE
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART2_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART2_FLOW_CONTROL HAL_USART_FLOW_CONTROL_NONE
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART3_ENABLE 0
#define EMBER_AF_PLUGIN_HAL_CONFIG_USART3_FLOW_CONTROL HAL_USART_FLOW_CONTROL_NONE
#define EMBER_AF_PLUGIN_HAL_CONFIG_BSP_UART_NCP_PORT HAL_SERIAL_PORT_USART0

// Use this macro to check if HAL Library plugin is included
#define EMBER_AF_PLUGIN_HAL_LIBRARY

// Use this macro to check if Simulated EEPROM version 1 Library plugin is included
#define EMBER_AF_PLUGIN_SIM_EEPROM1
// User options for plugin Simulated EEPROM version 1 Library


// Generated API headers

// API scan-dispatch from Scan Dispatch plugin
#define EMBER_AF_API_SCAN_DISPATCH "protocol/zigbee_5.10/app/framework/plugin/scan-dispatch/scan-dispatch.h"

// API adc-cortexm3 from ADC plugin
#define EMBER_AF_API_ADC_CORTEXM3 "platform/base/hal/plugin/adc/adc-cortexm3.h"

// API adc from ADC plugin
#define EMBER_AF_API_ADC "platform/base/hal/plugin/adc/adc.h"

// API antenna from Antenna Stub plugin
#define EMBER_AF_API_ANTENNA "platform/base/hal/plugin/antenna/antenna.h"

// API rail-library from RAIL Library plugin
#define EMBER_AF_API_RAIL_LIBRARY "platform/radio/rail_lib/common/rail.h"


// Custom macros
#ifdef APP_SERIAL
#undef APP_SERIAL
#endif
#define APP_SERIAL 1

#ifdef EMBER_ASSERT_SERIAL_PORT
#undef EMBER_ASSERT_SERIAL_PORT
#endif
#define EMBER_ASSERT_SERIAL_PORT 1

#ifdef EMBER_AF_BAUD_RATE
#undef EMBER_AF_BAUD_RATE
#endif
#define EMBER_AF_BAUD_RATE 115200

#ifdef EMBER_SERIAL0_MODE
#undef EMBER_SERIAL0_MODE
#endif
#define EMBER_SERIAL0_MODE EMBER_SERIAL_FIFO

#ifdef EMBER_SERIAL0_RX_QUEUE_SIZE
#undef EMBER_SERIAL0_RX_QUEUE_SIZE
#endif
#define EMBER_SERIAL0_RX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL0_TX_QUEUE_SIZE
#undef EMBER_SERIAL0_TX_QUEUE_SIZE
#endif
#define EMBER_SERIAL0_TX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL0_BLOCKING
#undef EMBER_SERIAL0_BLOCKING
#endif
#define EMBER_SERIAL0_BLOCKING

#ifdef EMBER_SERIAL1_MODE
#undef EMBER_SERIAL1_MODE
#endif
#define EMBER_SERIAL1_MODE EMBER_SERIAL_FIFO

#ifdef EMBER_SERIAL1_RX_QUEUE_SIZE
#undef EMBER_SERIAL1_RX_QUEUE_SIZE
#endif
#define EMBER_SERIAL1_RX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL1_TX_QUEUE_SIZE
#undef EMBER_SERIAL1_TX_QUEUE_SIZE
#endif
#define EMBER_SERIAL1_TX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL1_BLOCKING
#undef EMBER_SERIAL1_BLOCKING
#endif
#define EMBER_SERIAL1_BLOCKING

#ifdef EMBER_AF_SERIAL_PORT_INIT
#undef EMBER_AF_SERIAL_PORT_INIT
#endif
#define EMBER_AF_SERIAL_PORT_INIT() \
  do { \
    emberSerialInit(0, BAUD_115200, PARITY_NONE, 1); \
    emberSerialInit(1, BAUD_115200, PARITY_NONE, 1); \
  } while (0)



#endif // __APP_LM_PIR_MGM_ZNET510_H__
