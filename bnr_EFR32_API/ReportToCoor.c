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
 * Last Changed:     Date: Jul 4, 2016 10:18:48 AM
 *
 ****************************************************************************/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "app/framework/include/af.h"
#include "ReportToCoor.h"

#include "common.h"
#include "LocalIndicate.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/
uint8_t* atrValRepBuf;
ReadAtrRspFrame atrValFrame;
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
boolean ReadAttributeSuccessResponseCommand(uint16_t desAddr, uint8_t desEnp, uint8_t srcEnp, uint16_t clusId, uint8_t* buf, uint16_t bufLen)
{
#ifndef FOR_ZIPATO_HC
#ifndef DONT_BOOST_RF
    emberSetRadioPower(17);
#endif
    /* Fill to send buffer */
    emberAfFillCommandGlobalServerToClientReadAttributesResponse(clusId, buf, bufLen);
    emberAfSetCommandEndpoints(srcEnp, desEnp);

    /* Send */
    if (emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, desAddr) == EMBER_SUCCESS) {
        return true;
    } else {
        return false;
    }
#else
    memcpy(buf + 2, buf + 3, bufLen - 3);
    return ReportAttributeCommand(desAddr, desEnp, srcEnp, clusId, buf, bufLen - 1);
#endif
}

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
boolean ReportAttributeCommand(uint16_t desAddr, uint8_t desEnp, uint8_t srcEnp, uint16_t clusId, uint8_t* buf, uint16_t bufLen)
{
#ifndef DONT_BOOST_RF
    emberSetRadioPower(17);
#endif

    /* Fill to send buffer */
    // 3 = [attributeId:2]+[attributeType:1]
    emberAfFillCommandGlobalServerToClientReportAttributes(clusId, buf, bufLen);
    emberAfSetCommandEndpoints(srcEnp, desEnp);

    /* Send */
    if (emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, desAddr) == EMBER_SUCCESS)
        return true;

    return false;
}

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
void SendViaBindingTable(int8u endpoint, EmberAfClusterId clusterId,
        EmberAfAttributeId attributeId, int8u* value, int8u dataType) {

    int8u i;
    EmberBindingTableEntry result;
    EmberStatus bindingTableStatus;
    int16u NormalTransitionTime = 0xFFFF;

    for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
        bindingTableStatus = emberGetBinding(i, &result);
        if (bindingTableStatus == EMBER_SUCCESS) {
            if (result.type > EMBER_MULTICAST_BINDING) {
                result.type = 4;  // last entry in the string list above
            }
            if (result.type != EMBER_UNUSED_BINDING) {
                if (result.local == endpoint) {
                    if ((result.clusterId == clusterId) && (result.clusterId == ZCL_ON_OFF_CLUSTER_ID)) {
                        if (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID) {
                            if (value[0] == 1) {
                                emberAfFillCommandOnOffClusterOn();
                            }
                            else {
                                emberAfFillCommandOnOffClusterOff();
                            }
                        }
                    }
                    else if (clusterId == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
                        if (attributeId == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
                            emberAfFillCommandLevelControlClusterMoveToLevel(
                                    value[0], NormalTransitionTime);
                        }
                    }
                    emberAfSendCommandUnicast(EMBER_OUTGOING_VIA_BINDING, i); // Send Bind
                }
            }
        }
    }
}

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
void SendOnOffViaBindingTable(int8u *value) {
	SendViaBindingTable(SwitchEndpoint, ZCL_ON_OFF_CLUSTER_ID, ZCL_ON_OFF_ATTRIBUTE_ID, value, ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}







