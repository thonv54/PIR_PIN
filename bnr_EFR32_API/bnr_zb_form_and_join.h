/*******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: bnr_zb_form_and_join.h is a part of project LM_RGB_api572
 *
 * Author: phuclm
 *
 * Last Changed By:  Author: phuclm 
 * Revision:         Revision: 1.0.0.1 
 * Last Changed:     Date: 2016 Sep 6, 2016 11:08:00 AM 
 *
 ******************************************************************************/

#ifndef BNR_ZB_FORM_AND_JOIN_H_
#define BNR_ZB_FORM_AND_JOIN_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
extern EmberEventControl FindAndJoinNetworkEventControl;
extern EmberEventControl ResetDeviceEventControl;
extern uint16_t clusterReportIds[15];
extern uint8_t numOfClusterReport;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void FindAndJoinNetworkEventFunction(void);
void ResetDeviceEventFunction(void);
void joinSucessCallback(void);

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

#endif /* BNR_ZB_FORM_AND_JOIN_H_ */
