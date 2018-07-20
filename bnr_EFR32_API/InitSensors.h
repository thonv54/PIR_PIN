/******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: InitSensors.h is a part of project LM_DOORv2
 *
 * @Author: phuclm
 * @Date: 
 *
 * Last Modified by: 	phuclm 
 * Last Modified time: 	2016 Nov 25, 2016 1:52:56 PM 
 *
 ******************************************************************************/

#ifndef INITSENSORS_H_
#define INITSENSORS_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define SENSOR_ERROR_TIMEOUT			3
#define WAIT_SENSOR_STABLE_TIME			200

/* Default value */
#ifndef MIN_LUX
#define MIN_LUX						0
#endif
#ifndef MAX_LUX
#define MAX_LUX						83865
#endif
#ifndef MIN_TEMP
#define MIN_TEMP					1
#endif
#ifndef MAX_TEMP
#define MAX_TEMP					80
#endif
#ifndef MIN_HUMI
#define MIN_HUMI					2
#endif
#ifndef MAX_HUMI
#define MAX_HUMI					100
#endif
#ifndef MIN_BAT
#define MIN_BAT						1
#endif
#ifndef MAX_BAT
#define MAX_BAT						100
#endif
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
extern boolean sensorInitDone;
extern uint8_t sensorInitFailTime;

extern EmberEventControl InitHardwareEventControl;
extern EmberEventControl GetSensorsValueEventControl;
/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
/** @brief
 *
 */
void GetSensorsValueEventFunction(void);

boolean sensor_Init(void);
void DeInitSensor(void);

#endif /* INITSENSORS_H_ */
