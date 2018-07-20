/******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: lm_pir_sensor.c is a part of project SS_LM_PIR
 *
 * @Author: phuclm
 * @Date: 
 *
 * Last Modified by: 	phuclm 
 * Last Modified time: 	2017 Feb 4, 2017 11:32:52 AM 
 *
 ******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "lm_pir_sensor.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
GPIO_Port_TypeDef pirLocPort;
unsigned int pirLocPin;

boolean pirIsEnable = false;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/** @brief
 *
 */
void initPirSensor(GPIO_Port_TypeDef port, unsigned int pin, boolean en) {
	pirLocPort = port;
	pirLocPin = pin;

	enablePirSensor(en);
}

/** @brief
 *
 */
void enablePirSensor(boolean en)
{
//	GPIO_IntConfig(pirLocPort, pirLocPin, true, false, en);
	/* Clear any pending interrupt */
	GPIO->IFC = 1 << pirLocPin;
	/* Finally enable/disable interrupt */
	BUS_RegBitWrite(&(GPIO->IEN), pirLocPin, en);

	pirIsEnable = en;
}



