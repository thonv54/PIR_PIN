/******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: localIndicate.c is a part of project LM_PIR
 *
 * @Author: phuclm
 * @Date: 
 *
 * Last Modified by: 	phuclm 
 * Last Modified time: 	2016 Nov 15, 2016 4:44:33 PM 
 *
 ******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "LM_PIR_mgm_znet510_board.h"
#include "em_gpio.h"
#include "em_cmu.h"

#include "InitSensors.h"
#include "lm_pir_sensor.h"
#include "bnr_zb_form_and_join.h"
#include "LocalIndicate.h"
#include "stdbool.h"
#include "plugin-soc/idle-sleep/idle-sleep.h"
#include "LM_PIR_mgm_znet510.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
// Button local
#define RESET_PORT					BSP_BUTTON1_PORT
#define RESET_PIN					BSP_BUTTON1_PIN
#define FUNC_BUTTON_PORT			BSP_BUTTON0_PORT
#define FUNC_BUTTON_PIN				BSP_BUTTON0_PIN

#define HOLD_1_SECOND				(1)
#define HOLD_3_SECOND				(3)
#define HOLD_5_SECOND				(5)
#define HOLD_7_SECOND				(7)
#define HOLD_8_SECOND				(8)
#define PRESS_FAST_2_TIME			(10)
#define PRESS_FAST_3_TIME			(11)

#define TIME_LED_BLINK_FAST			(50)
#define TIME_LED_BLINK_MEDIUM		(100)
#define TIME_LED_BLINK_SLOW			(500)

#define TIME_BLINK_LED_WHEN_MOTIONING		2000

#define BTN_HOLD_TIME_TO_JOIN_NETWORK		10		// 1 second
#define BTN_HOLD_TIME_TO_RESET_DEVICE		30		// 5 seconds
#define BTN_HOLD_TIME_RESET_DEVICE_TIMEOUT	50		// 80 seconds
#define BTN_HOLD_TIME_TO_BIND_DEVICE		80		// 80 seconds
#define BTN_HOLD_TIME_TIMEOUT				200		// 10 seconds

#define WAIT_FOR_NO_MOTION_PULSE			5000

#define CONSECUTIVE_MOTION_TO_REJOIN		7

#define PIR_DELAY_30_SECOND			30
#define PIR_DELAY_120_SECOND		120
#define PIR_DELAY_300_SECOND		300

typedef struct {
	GPIO_Port_TypeDef port;
	unsigned int pin;
} tLedArray;

const tLedArray ledArray[NUM_OF_LED_INDICATOR] = LED_INDICATOR_ARRAY_INIT;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
static uint8_t ledReplayTime = 0;
static uint16_t buttonHoldTime;
static uint8_t buttonState;
static uint8_t numFastPress = 0;

static uint8_t numOfToggleFindNetworkLed = 0;
boolean toggleColor = true;

static boolean configConfirmTime = false;

boolean stillHasMotion = false;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
EmberEventControl LocalIndicateEventControl;
EmberEventControl CalButtonPressTimeEventControl;
EmberEventControl ButtonPressComboEventControl;

uint8_t deviceState;
boolean batteryIsLow = false;
uint8_t funcButtonState;

uint32_t motionDetectedTime;

uint16_t pirDelayTime = PIR_DELAY_30_SECOND;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

void ButtonPressComboEventFunction(void);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

void DeInitLocalIndicate(void) {
	for (uint8_t i = 0; i < NUM_OF_LED_INDICATOR; i++) {
		GPIO_PinModeSet(ledArray[i].port, ledArray[i].pin, gpioModeDisabled,
				LED_OFF);
	}
}

/** @brief
 *
 */
void initLocalIndicate(void) {
#ifdef LED_INDICATE
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	for (uint8_t i = 0; i < NUM_OF_LED_INDICATOR; i++) {
		GPIO_PinModeSet(ledArray[i].port, ledArray[i].pin, gpioModePushPull,
				LED_OFF);
	}

	ledReplayTime = 0;

	deviceState = DEVICE_POWER_UP;
	emberEventControlSetDelayMS(LocalIndicateEventControl, 300);
#endif
#ifdef  BUZZ_INDICATE
#endif
}

/** @brief
 *
 */
void toggleLed(uint8_t led, uint8_t num, uint16_t time) {

}

/** @brief task:
 * 	count num of fast press: fast press + fast release
 *
 */
void emberAfHalButtonIsrCallback(int8u button, int8u state) {
	static uint32_t buttonRelaseTime;

	if (button == RESET_PIN) {

		configButtonPressedCallback(state);

		if (state == BTN_PRSED) {

			enablePirSensor(FALSE);

			uint32_t releaseTimeOfButton = abs(
					halCommonGetInt32uMillisecondTick() - buttonRelaseTime);

			// prevent too fast or slow duplicate press button
			if ((releaseTimeOfButton > 349) || (releaseTimeOfButton < 50)) {
				numFastPress = 0;
			}

			buttonState = START_HOLD;
			buttonHoldTime = 0;

			emberEventControlSetInactive(LocalIndicateEventControl);

			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);

			emberEventControlSetActive(CalButtonPressTimeEventControl);

		} else if (state == BTN_RLSED) {

			buttonRelaseTime = halCommonGetInt32uMillisecondTick();

			emberEventControlSetInactive(LocalIndicateEventControl);
			emberEventControlSetInactive(CalButtonPressTimeEventControl);

			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);

			numOfToggleFindNetworkLed = 0;

			if (buttonHoldTime < 250) {
				numFastPress += 1;
			} else { // buttonHoldTime > 250
				numFastPress = 0;
				buttonState = BUTTON_IDLE;
			}

			if (numFastPress == 2) {
				buttonState = PRESS_FAST_2_TIME;
			}
			if (numFastPress >= 3) {
				numFastPress = 0;
				buttonState = PRESS_FAST_3_TIME;
			}
			switch (buttonState) {
			case HOLD_1_SECOND:
				buttonHold1SecondCallback();
//				emberEventControlSetActive(FindAndJoinNetworkEventControl);
				break;
			case HOLD_5_SECOND:
				inactiveAllRunningEvent();
				emberEventControlSetActive(ResetDeviceEventControl);
				break;
			case PRESS_FAST_2_TIME:
				emberEventControlSetInactive(ButtonPressComboEventControl);
				emberEventControlSetDelayMS(ButtonPressComboEventControl, 1000);
				switch (pirDelayTime) {
				case PIR_DELAY_30_SECOND:
					LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
					LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);
					break;

				case PIR_DELAY_120_SECOND:
					LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
					LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);
					break;

				case PIR_DELAY_300_SECOND:
					LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
					LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
					break;

				default:
					LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
					LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
					break;
				}

				deviceState = DEVICE_IDLE;
				emberEventControlSetDelayMS(LocalIndicateEventControl, 3000);
				break;
			case PRESS_FAST_3_TIME:
				emberEventControlSetInactive(ButtonPressComboEventControl);
				emberEventControlSetDelayMS(ButtonPressComboEventControl, 1000);

				LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);
				LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
				deviceState = WAIT_BIND;
				emberEventControlSetDelayMS(LocalIndicateEventControl, 10000);
				break;

			default:

				if (!pirIsEnable) {
					enablePirSensor(TRUE);
				}

				deviceState = DEVICE_IDLE;
				ledReplayTime = 0;
				emberEventControlSetActive(LocalIndicateEventControl);
				break;
			}
		}
	} else if (button == FUNC_BUTTON_PIN) {
#ifdef FUNC_BUTTON_PRESSED_CALLBACK
		funcButtonPressedCallback(state);
#endif
	}
}

/**
 * @func   	None
 * @brief  	None
 * @param  	None
 * @retval 	None
 */
void CalButtonPressTimeEventFunction(void) {
	static boolean toggleLed = false;

	emberEventControlSetInactive(CalButtonPressTimeEventControl);

	buttonHoldTime += 1;
//	if (buttonHoldTime > BTN_HOLD_TIME_TO_BIND_DEVICE)
//		{
//			buttonState = HOLD_8_SECOND;
//			toggleLed = false;
//			numOfToggleFindNetworkLed = 0;
//			LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
//			LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);
//
//			if (buttonHoldTime > BTN_HOLD_TIME_TIMEOUT)
//			{
//				LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
//				LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
//			}
//		}
	if (buttonHoldTime > BTN_HOLD_TIME_TO_RESET_DEVICE) {
		buttonState = HOLD_5_SECOND;
		toggleLed = false;
		numOfToggleFindNetworkLed = 0;
		LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
		LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);

		if (buttonHoldTime > BTN_HOLD_TIME_TIMEOUT) {
			LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
		}
	} else {
		if (buttonHoldTime > BTN_HOLD_TIME_TO_JOIN_NETWORK) {
			buttonState = HOLD_1_SECOND;
			if (numOfToggleFindNetworkLed < 4) {
				toggleLed = true;
			}
		}
		if (toggleLed) {
			if (numOfToggleFindNetworkLed < 4) {
				toggleLed = false;
				numOfToggleFindNetworkLed += 1;
				LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
				LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
			}
		}
		emberEventControlSetDelayMS(CalButtonPressTimeEventControl,
				TIME_LED_BLINK_MEDIUM);
	}
}
/**
 * @func   	<functionName>
 *
 * @status	building...
 *
 * @brief
 *
 * @param  	int x :
 *		   	int y :
 *
 * @retval 	None
 */
void LocalIndicateEventFunction(void) {
	static uint8_t countToRejoin = 0;
	static uint8_t numOfAttemptRejoin = 0;

	static boolean shiftedForGetSensor = false;

	EmberNetworkStatus nwkState;

	emberEventControlSetInactive(LocalIndicateEventControl);

	nwkState = emberNetworkState();

#ifdef LED_INDICATE
	switch (deviceState) {
	case DEVICE_POWER_UP:
		switch (ledReplayTime) {
		case 0:
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
			LedControlTurnOn(LED_BLUE_PORT, LED_BLUE_PIN);
			break;
		case 1:
			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);
			break;
		default:
			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
			break;
		}

		ledReplayTime += 1;

		enablePirSensor(FALSE);

		if (ledReplayTime > 2) {
			ledReplayTime = 0;

			// after toggle led, init sensors
			if (!sensorInitDone) {
				sensorInitFailTime = 0;
				emberEventControlSetActive(InitHardwareEventControl);
			}

		} else {
			emberEventControlSetDelayMS(LocalIndicateEventControl,
					TIME_LED_BLINK_SLOW);
		}
		break;

	case DEVICE_INIT_SENSOR_FAIL:
		LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
		LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
		ledReplayTime += 1;
		if (ledReplayTime > 1) {
			deviceState = DEVICE_IDLE;
			ledReplayTime = 0;
		}
		emberEventControlSetDelayMS(LocalIndicateEventControl, 3000);
		break;

	case DEVICE_WAS_STABLE:
		if (nwkState == EMBER_JOINED_NETWORK) {
			LedControlToggle(LED_RED_PORT, LED_RED_PIN);
			LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
		} else if (nwkState == EMBER_NO_NETWORK) {
			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlToggle(LED_RED_PORT, LED_RED_PIN);
		} else {
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
			LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
		}

		ledReplayTime += 1;
		if (ledReplayTime > 5) {
			deviceState = DEVICE_IDLE;
			ledReplayTime = 0;

//			if (nwkState != EMBER_JOINED_NETWORK_NO_PARENT) {
			enablePirSensor(TRUE);
//			}

		}
		emberEventControlSetDelayMS(LocalIndicateEventControl,
				TIME_LED_BLINK_SLOW);
		break;

	case DEVICE_FINDING_NETWORK:
		LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
		LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
		ledReplayTime = 0;
		break;

	case DEVICE_JOIN_SUCCESS:
		LedControlToggle(LED_RED_PORT, LED_RED_PIN);
		LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
		ledReplayTime += 1;
		if (ledReplayTime > 5) {
			ledReplayTime = 0;
			deviceState = DEVICE_IDLE;

			enablePirSensor(TRUE);
		}
		emberEventControlSetDelayMS(LocalIndicateEventControl,
				TIME_LED_BLINK_SLOW);
		break;

	case DEVICE_FIND_FAIL:
		LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
		LedControlToggle(LED_RED_PORT, LED_RED_PIN);
		ledReplayTime += 1;
		if (ledReplayTime > 5) {
			ledReplayTime = 0;
			deviceState = DEVICE_IDLE;

			enablePirSensor(TRUE);
		}
		emberEventControlSetDelayMS(LocalIndicateEventControl,
				TIME_LED_BLINK_SLOW);
		break;

	case DEVICE_IDLE:

		buttonState = BUTTON_IDLE;

		if (configConfirmTime) {
			configConfirmTime = false;
			enablePirSensor(TRUE);
		}

		if (batteryIsLow) {
			deviceState = DEVICE_BAT_LOW;
			emberEventControlSetActive(LocalIndicateEventControl);
		} else {
#ifdef SLEEPY_DEVICE
			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
#else
			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlTurnOn(LED_RED_PORT, LED_RED_PIN);
#endif
			if (stillHasMotion) {
				ledReplayTime = 0;

				if (nwkState == EMBER_JOINED_NETWORK) {
					if (!shiftedForGetSensor) {
						if (emberEventControlGetRemainingMS(
								GetSensorsValueEventControl)
								<= (TIME_BLINK_LED_WHEN_MOTIONING + 500)) {
							shiftedForGetSensor = true;
							emberEventControlSetDelayMS(
									GetSensorsValueEventControl,
									TIME_BLINK_LED_WHEN_MOTIONING + 500);
						}
					} else {
						shiftedForGetSensor = false;
					}
				}
				deviceState = DEVICE_MOTION_DETECTED;
				emberEventControlSetDelayMS(LocalIndicateEventControl,
						TIME_BLINK_LED_WHEN_MOTIONING);
			}
		}
		break;

	case DEVICE_SPECIFIC_STATE:
		if (!sensorInitDone) {
			LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
			LedControlToggle(LED_RED_PORT, LED_RED_PIN);
			emberEventControlSetDelayMS(LocalIndicateEventControl,
					TIME_LED_BLINK_FAST);
		} else {
			if (nwkState == EMBER_JOINED_NETWORK) {
				LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
				LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
				emberEventControlSetDelayMS(LocalIndicateEventControl,
						TIME_LED_BLINK_FAST);
			} else if (nwkState == EMBER_NO_NETWORK) {
				LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
				LedControlToggle(LED_RED_PORT, LED_RED_PIN);
				emberEventControlSetDelayMS(LocalIndicateEventControl,
						TIME_LED_BLINK_SLOW);
			} else {
				LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
				LedControlToggle(LED_RED_PORT, LED_RED_PIN);
				emberEventControlSetDelayMS(LocalIndicateEventControl,
						TIME_LED_BLINK_FAST);
			}
		}

		ledReplayTime += 1;
		if (ledReplayTime > 3) {
			deviceState = DEVICE_IDLE;
			ledReplayTime = 0;

			enablePirSensor(TRUE);
		}
		break;

	case DEVICE_MOTION_DETECTED:
		if (!sensorInitDone) {
			LedControlToggle(LED_RED_PORT, LED_RED_PIN);
			LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
		} else {
			if (nwkState == EMBER_JOINED_NETWORK) {
				LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
				LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
			} else {
				LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
				LedControlToggle(LED_RED_PORT, LED_RED_PIN);
			}
		}

//		stillHasMotion = false;
		if (abs(
				halCommonGetInt32uMillisecondTick()
						- motionDetectedTime) > WAIT_FOR_NO_MOTION_PULSE) {
			stillHasMotion = false;

			countToRejoin = 0;
		} else {
			// if motion consecutive appear up to 30 times, perform a rejoin
			if (nwkState == EMBER_JOINED_NETWORK_NO_PARENT) {
				if (numOfAttemptRejoin > 2) {
					halReboot();
				}

				countToRejoin += 1;
				if (countToRejoin > CONSECUTIVE_MOTION_TO_REJOIN) {
					numOfAttemptRejoin += 1;

					countToRejoin = 0;

					if (!emberStackIsPerformingRejoin()) {
//						emberFindAndRejoinNetwork(TRUE , 0);
						emberPollForData();
					}

				}
			} else {
				countToRejoin = 0;

				numOfAttemptRejoin = 0;
			}
		}

		deviceState = DEVICE_IDLE;
		if (nwkState == EMBER_NO_NETWORK) {
			emberEventControlSetDelayMS(LocalIndicateEventControl,
					TIME_LED_BLINK_SLOW);
		} else {
			emberEventControlSetDelayMS(LocalIndicateEventControl,
					TIME_LED_BLINK_FAST);
		}
		break;

	case DEVICE_BAT_LOW:
		LedControlToggle(LED_RED_PORT, LED_RED_PIN);
		LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
		ledReplayTime += 1;
		if (ledReplayTime > 9) {
			ledReplayTime = 0;

			LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
			LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);

			deviceState = DEVICE_BAT_LOW;
			emberEventControlSetDelayMS(LocalIndicateEventControl, 600000);
		} else {
			emberEventControlSetDelayMS(LocalIndicateEventControl,
					TIME_LED_BLINK_FAST);
		}
		break;

	case DEVICE_RESET:
		LedControlToggle(LED_RED_PORT, LED_RED_PIN);
		LedControlToggle(LED_BLUE_PORT, LED_BLUE_PIN);
		ledReplayTime += 1;
		if (ledReplayTime > 5) {
			ledReplayTime = 0;

			halReboot();
		} else {
			emberEventControlSetDelayMS(LocalIndicateEventControl,
					TIME_LED_BLINK_SLOW);
		break;
		}
	case WAIT_BIND:
		emberAfForceEndDeviceToStayAwake(false);
		LedControlTurnOff(LED_RED_PORT, LED_RED_PIN);
		LedControlTurnOff(LED_BLUE_PORT, LED_BLUE_PIN);
		deviceState = DEVICE_IDLE;
		emberEventControlSetActive(LocalIndicateEventControl);
		break;
	default:
		deviceState = DEVICE_IDLE;
		emberEventControlSetActive(LocalIndicateEventControl);
		break;
	}
#endif
}

/**
 * @func   	<functionName>
 *
 * @status	building...
 *
 * @brief  	This function do <functionName>: ...
 *
 * @param  	int x :
 *		   	int y :
 *
 * @retval 	None
 */
boolean emberAfStackStatusCallback(EmberStatus status) {
	if (status == EMBER_NETWORK_UP) {
#ifdef NETWORK_UP_CALLBACK
		networkUpCallback();
#endif
	} else {
#ifdef NETWORK_DOWN_CALLBACK
		networkDownCallback();
#endif
	}
//
//	deviceState = DEVICE_IDLE;
//	emberEventControlSetActive(LocalIndicateEventControl);
	return FALSE;
}

void ButtonPressComboEventFunction(void) {

	emberEventControlSetInactive(ButtonPressComboEventControl);
	switch (buttonState) {
	case PRESS_FAST_2_TIME:
		switch (pirDelayTime) {
		case PIR_DELAY_30_SECOND:
			pirDelayTime = PIR_DELAY_120_SECOND;
			break;
		case PIR_DELAY_120_SECOND:
			pirDelayTime = PIR_DELAY_300_SECOND;
			break;
		case PIR_DELAY_300_SECOND:
			pirDelayTime = PIR_DELAY_30_SECOND;
			break;
		default:
			pirDelayTime = PIR_DELAY_30_SECOND;
			break;
		}
		emberAfWriteServerAttribute(PIR_ENDPOINT, ZCL_IAS_WD_CLUSTER_ID,
		ZCL_MAX_DURATION_ATTRIBUTE_ID, (uint8_t*) &pirDelayTime,
				ZCL_INT16U_ATTRIBUTE_TYPE);
		configConfirmTime = true;
		break;
	case PRESS_FAST_3_TIME:
		emberAfSendEndDeviceBind(SwitchEndpoint);
		emberAfForceEndDeviceToStayAwake(true);
		break;
	default:
		break;
	}
}

void emberAfPluginIdleSleepWakeUpCallback(uint32_t durationMs) {
	//turn on UArt
	emberSerialInit(APP_SERIAL, EMBER_AF_BAUD_RATE, PARITY_NONE, 1);

	//turn on PIR

//	enablePirSensor(1);
	//turn on ADC

	//turn on Led
//	initLocalIndicate();
	//turn on I2C
	(void) sensor_Init();

}

bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs) {

	//turn off UArt
	(void) COM_DeInit(COM_USART0);
	GPIO_PinModeSet(BSP_USART0_CTS_PORT,
	BSP_USART0_CTS_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(BSP_USART0_RTS_PORT,
	BSP_USART0_RTS_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(BSP_USART0_RX_PORT,
	BSP_USART0_RX_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(BSP_USART0_TX_PORT,
	BSP_USART0_TX_PIN, gpioModeDisabled, 0);
	//turn off PIR
//	enablePirSensor(0);
	//turn off ADC
	//turn off Led
//	DeInitLocalIndicate();
	//turn off I2C
	DeInitSensor();
	return true;
}




void LedControlTurnOn(GPIO_Port_TypeDef port,unsigned int pin){
	GPIO_PinOutSet( port, pin);
}

void LedControlTurnOff(GPIO_Port_TypeDef port,unsigned int pin){
	GPIO_PinOutClear( port, pin);
}

void LedControlToggle(GPIO_Port_TypeDef port,unsigned int pin){
	GPIO_PinOutToggle(port, pin);
}
