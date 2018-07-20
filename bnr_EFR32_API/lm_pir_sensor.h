/******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: lm_pir_sensor.h is a part of project SS_LM_PIR
 *
 * @Author: phuclm
 * @Date: 
 *
 * Last Modified by: 	phuclm 
 * Last Modified time: 	2017 Feb 4, 2017 11:32:39 AM 
 *
 ******************************************************************************/

#ifndef LM_PIR_SENSOR_H_
#define LM_PIR_SENSOR_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"

#include "em_gpio.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define PIR_PORT						BSP_BUTTON0_PORT
#define PIR_PIN							BSP_BUTTON0_PIN

#define PIR_ENDPOINT					(1)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
extern GPIO_Port_TypeDef pirLocPort;
extern unsigned int pirLocPin;

extern boolean pirIsEnable;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/** @brief
 *
 */
void initPirSensor(GPIO_Port_TypeDef port, unsigned int pin, boolean en);

/** @brief
 *
 */
void enablePirSensor(boolean en);

#endif /* LM_PIR_SENSOR_H_ */
