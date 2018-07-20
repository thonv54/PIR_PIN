/******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: InitSensors.c is a part of project LM_DOORv2
 *
 * @Author: phuclm
 * @Date: 
 *
 * Last Modified by: 	phuclm 
 * Last Modified time: 	2016 Nov 25, 2016 1:53:04 PM 
 *
 ******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"

#include "em_i2c.h"
#include "i2cspm.h"
#include "i2cspmconfig.h"
#include "si7013.h"
#include "opt3001.h"

#include "LocalIndicate.h"
#include "InitSensors.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
boolean sensorInitDone = false;
uint8_t sensorInitFailTime;

EmberEventControl InitHardwareEventControl;
EmberEventControl GetSensorsValueEventControl;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
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
void InitHardwareEventFunction(void)
{
	emberEventControlSetInactive(InitHardwareEventControl);

	sensorInitDone = sensor_Init();

	if (!sensorInitDone)
	{
		sensorInitFailTime += 1;
		if (sensorInitFailTime > SENSOR_ERROR_TIMEOUT)
		{
			deviceState = DEVICE_IDLE;
			emberEventControlSetActive(LocalIndicateEventControl);
		}
		else
		{
			emberEventControlSetDelayMS(InitHardwareEventControl, WAIT_SENSOR_STABLE_TIME);
		}
	}
	else
	{
		sensorInitFailTime = 0;
//
//		Si7013_ReadUserReg1(I2C0, SI7020_ADDR, &usrRegSi7020);
//
//		if (usrRegSi7020 != SI7020_MEASURE_RH_8_T_12)
//		{
//			Si7013_WriteUserReg1(I2C0, SI7020_ADDR, SI7020_MEASURE_RH_8_T_12);
//		}

//		deviceState = DEVICE_IDLE;
//		emberEventControlSetActive(LocalIndicateEventControl);
	}
}
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/** @brief
 *
 */
boolean sensor_Init(void)
{
    boolean retVal;
    uint8_t devIdSi7020 = 0xff;
    uint16_t usrRegSi7020 = 0xffff;

    I2CSPM_Init_TypeDef i2cspmInit = I2CSPM_INIT_LM_SI7020;
    I2CSPM_Init(&i2cspmInit);

    if (Si7013_Detect(I2C0, SI7020_ADDR, &devIdSi7020))
        retVal = (devIdSi7020 == SI7020_DEVICE_ID);

    if (retVal)
    {
        Si7013_ReadUserReg1(I2C0, SI7020_ADDR, &usrRegSi7020);

        if (usrRegSi7020 != SI7020_MEASURE_RH_8_T_12)
            Si7013_WriteUserReg1(I2C0, SI7020_ADDR, SI7020_MEASURE_RH_8_T_12);
    }

    if (retVal)
        retVal &= (Opt3001_GetDevIdVal() == OPT3001_DEV_ID);

    return retVal;

}

void DeInitSensor(void){
	// clear GPIO_PIN:
    I2CSPM_Init_TypeDef i2cspmInit = I2CSPM_INIT_LM_SI7020;
	GPIO_PinModeSet(i2cspmInit.sclPort, i2cspmInit.sclPin, gpioModeWiredAndPullUp, 1);
	GPIO_PinModeSet(i2cspmInit.sdaPort, i2cspmInit.sdaPin, gpioModeWiredAndPullUp, 1);
}
