/****************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: bnr_zb_form_and_join.c is a part of project LM_RGB_api572
 *
 * Author: phuclm
 *
 * Last Changed By:  Author: phuclm 
 * Revision:         Revision: 1.0.0.1 
 * Last Changed:     Date: 2016 Sep 6, 2016 11:08:22 AM 
 *
 ****************************************************************************/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "app/framework/include/af.h"
#include "app/util/common/form-and-join.h"
#include "app/framework/plugin/ezmode-commissioning/ez-mode.h"
#include "app/framework/plugin/reporting/reporting.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "bnr_zb_form_and_join.h"
#include "LocalIndicate.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#ifndef FIND_NETWORK_TIME
#define FIND_NETWORK_TIME			30
#endif

#ifndef ATTEMPT_JOIN_DURATION_MS
#define ATTEMPT_JOIN_DURATION_MS    1000
#endif
/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/
EmberEventControl FindAndJoinNetworkEventControl;
EmberEventControl ResetDeviceEventControl;

uint16_t clusterReportIds[15];
uint8_t numOfClusterReport;

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/****************************************************************************/
/*                            EXPORTED FUNCTIONS                            */
/****************************************************************************/

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
void FindAndJoinNetworkEventFunction(void) {
	static uint8_t numAttemptFind = 0;

	emberEventControlSetInactive(FindAndJoinNetworkEventControl);

#ifndef DONT_BOOST_RF
	emberSetRadioPower(17);
#endif
	EmberNetworkStatus state = emberAfNetworkState();

	if (state != EMBER_JOINED_NETWORK)
	{
		numAttemptFind += 1;
		deviceState = DEVICE_FINDING_NETWORK;

		if (numAttemptFind == 1)
		{
			emberEventControlSetActive(LocalIndicateEventControl);
		}

		if (state == EMBER_NO_NETWORK)
		{
			if (!emberFormAndJoinIsScanning())
			{
				emberAfStartSearchForJoinableNetwork();
			}
		}

		if (numAttemptFind < FIND_NETWORK_TIME)
		{
			emberEventControlSetDelayMS(FindAndJoinNetworkEventControl, ATTEMPT_JOIN_DURATION_MS);
		}
		else
		{
			numAttemptFind = 0;
			deviceState = DEVICE_FIND_FAIL;
			emberEventControlSetActive(LocalIndicateEventControl);
		}
	}
	else
	{
		numAttemptFind = 0;
#ifdef JOIN_SUCESS_CALLBACK
		joinSucessCallback();
#endif
	}

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
void ResetDeviceEventFunction(void) {
	static uint8_t waitTimeCount = 0;

	emberEventControlSetInactive(ResetDeviceEventControl);

	EmberNetworkStatus status = emberAfNetworkState();

	waitTimeCount += 1;
	if (waitTimeCount > 21) { // ~ 2 second
		waitTimeCount = 0;

		deviceState = DEVICE_RESET;
		emberEventControlSetDelayMS(LocalIndicateEventControl, 500);
	}

	if (status == EMBER_LEAVING_NETWORK) {
		emberEventControlSetDelayMS(ResetDeviceEventControl, 100);
	} else if (status == EMBER_NO_NETWORK) {
		waitTimeCount = 0;

		deviceState = DEVICE_RESET;
		emberEventControlSetDelayMS(LocalIndicateEventControl, 500);
	} else {
		// send leave response to Coordinator
		uint8_t contents[2];
		contents[1] = 0x00;
		(void) emberSendZigDevRequest(0x0000, LEAVE_RESPONSE, EMBER_AF_DEFAULT_APS_OPTIONS, contents, sizeof(contents));

		// ...then leave network
		emberLeaveNetwork();

		// check when device leave network
		emberEventControlSetDelayMS(ResetDeviceEventControl, 100);
	}
}
