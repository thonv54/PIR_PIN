/****************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: Include file for application
 *
 * Author: phuclm
 *
 * Last Changed By:  Author: phuclm 
 * Revision:         Revision: 1.0.0.1 
 * Last Changed:     Date: Jun 24, 2016 12:45:21 AM 
 *
 ****************************************************************************/
#ifndef LUMIADC_H_
#define LUMIADC_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "app/framework/include/af.h"

#include "bnr_hw_gpio.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
typedef uint8_t LumiAdcReference;
typedef uint8_t LumiAdcResolution;

#define USE_INTERRUPT	1

#define ADC_REF_1V25	0
#define	ADC_REF_2V5		1
#define ADC_REF_VDD		2
#define ADC_REF_5V		3

#define ADC_RES_6		0
#define ADC_RES_8		1
#define ADC_RES_12		2



/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/****************************************************************************/
/*                            EXPORTED FUNCTIONS                            */
/****************************************************************************/

/*******************************************************************************
 * @func   funcA
 *
 * @status
 *
 * @brief  funcA do something
 *
 * @param  int x :
 int y :
 *
 * @retval None
 */
void lumiADCInit(
		LumiGpioLocation gpioLoc,
		LumiAdcReference ref,
		LumiAdcResolution res,
		boolean measureBattery,
		boolean useInterrupt
);
/**
 * @func   funcA
 *
 * @status
 *
 * @brief  funcA do something
 *
 * @param  int x :
 int y :
 *
 * @retval None
 */
uint32_t
lumiADCGetValue(
		LumiGpioLocation gpioLoc,
		LumiAdcReference ref,
		LumiAdcResolution res,
		boolean measureBattery,
		boolean useInterrupt
);

/**
 * @func   funcA
 *
 * @status
 *
 * @brief  funcA do something
 *
 * @param  int x :
 int y :
 *
 * @retval None
 */
void
lumiADCEnable(
		LumiGpioLocation gpioLoc,
		boolean enable
);

/**
 * @func   funcA
 *
 * @status
 *
 * @brief  funcA do something
 *
 * @param  int x :
 int y :
 *
 * @retval None
 */
void
lumiADCConfigure(
		LumiGpioLocation gpioLoc,
		LumiAdcReference ref,
		LumiAdcResolution res,
		boolean measureBattery,
		boolean useInterrupt
);

#endif /* LUMIADC_H_ */
