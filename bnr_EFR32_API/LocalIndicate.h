/******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: localIndicate.h is a part of project LM_PIR
 *
 * @Author: phuclm
 * @Date: 
 *
 * Last Modified by: 	phuclm 
 * Last Modified time: 	2016 Nov 15, 2016 4:44:22 PM 
 *
 ******************************************************************************/

#ifndef LOCALINDICATE_H_
#define LOCALINDICATE_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
/* Device states */
#define DEVICE_POWER_UP				(0)
#define DEVICE_IDLE					(1)
#define DEVICE_FINDING_NETWORK		(2)
#define DEVICE_FIND_FAIL			(3)
#define DEVICE_JOIN_SUCCESS			(4)
#define DEVICE_SPECIFIC_STATE		(5)
#define DEVICE_BAT_LOW				(6)
#define DEVICE_INIT_SENSOR_FAIL		(7)
#define DEVICE_SETUP_PIR_DELAY_TIME	(8)
#define DEVICE_WAS_STABLE			(9)
#define DEVICE_RESET				(10)
#define DEVICE_MOTION_DETECTED		(11)
#define WAIT_BIND					(12)

// Button states
#define BTN_PRSED                   (1)
#define BTN_RLSED                   (0)
#define BUTTON_IDLE					(2)
#define START_HOLD					(3)

// Define led indicate
#ifdef CATHODE_LED
#define LED_ON								(0)
#define LED_OFF								(1)
#define LedControlTurnOn(port, pin)			GPIO_PinOutClear(port, pin)
#define LedControlTurnOff(port, pin)		GPIO_PinOutSet(port, pin)
#elif defined(ANODE_LED)
#define LED_ON								(1)
#define LED_OFF								(0)
void LedControlTurnOn(GPIO_Port_TypeDef port,unsigned int pin);
void LedControlTurnOff(GPIO_Port_TypeDef port,unsigned int pin);
#endif
void LedControlToggle(GPIO_Port_TypeDef port,unsigned int pin);


//
#define SwitchEndpoint 6

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
extern EmberEventControl LocalIndicateEventControl;
extern uint8_t deviceState;

extern boolean batteryIsLow;

extern uint8_t funcButtonState;

extern uint16_t pirDelayTime;

extern boolean stillHasMotion;
extern uint32_t motionDetectedTime;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
void DeInitLocalIndicate(void);
void initLocalIndicate(void);
void toggleLed(uint8_t led, uint8_t num, uint16_t time);
void funcButtonPressedCallback(uint8_t funcButtonState);
void configButtonPressedCallback(uint8_t cfgBtnState);
void LocalIndicateEventFunction(void);
void CalButtonPressTimeEventFunction(void);
void networkUpCallback(void);
void networkDownCallback(void);
void buttonHold1SecondCallback(void);
void inactiveAllRunningEvent(void);

#endif /* LOCALINDICATE_H_ */
