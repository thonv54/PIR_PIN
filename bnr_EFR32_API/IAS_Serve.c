/*
 * ServeIAS.c
 *
 *  Created on: Aug 7, 2017
 *      Author: phuclm
 */
#include "IAS_Serve.h"

#include "app/framework/include/af.h"

/** @brief
 *
 */
void iasServe_Init(IASZone_t* obj)
{
    // reset values
    obj->zoneId = INVALID_ZONE_ID;
    for (uint8_t i = 0; i < 8; i++)
        obj->cieIeeeAddr[i] = 0xff;

    // read nvm values
    emberAfReadServerAttribute(obj->localEnp, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID, obj->cieIeeeAddr, 8);
    emberAfReadServerAttribute(obj->localEnp, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_ID_ATTRIBUTE_ID, &obj->zoneId, 1);

    // write nvm values
    emberAfWriteServerAttribute(obj->localEnp, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_TYPE_ATTRIBUTE_ID, &obj->zoneType, ZCL_ENUM16_ATTRIBUTE_TYPE);
}

/** @brief
 *
 */
void iasServe_ReceiveCieAddrCallback(uint8_t* _CIEIeee, IASZone_t* obj)
{
    // save CIE addr
    memcpy(obj->cieIeeeAddr, _CIEIeee, 8);
    emberAfWriteServerAttribute(obj->localEnp, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID, obj->cieIeeeAddr, ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE);

    // req nwkAddr of CIE
    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t temp;

        temp = obj->cieIeeeAddr[i];
        obj->cieIeeeAddr[i] = obj->cieIeeeAddr[7 - i];
        obj->cieIeeeAddr[7 - i] = temp;
    }
    emberAfFindNodeId(obj->cieIeeeAddr, obj->cieNwkAddrResponseCallback);
}

/** @brief
 *
 */
void iasServe_SendEnrollRequest(IASZone_t* obj)
{
    // send enroll request
    emberAfFillCommandIasZoneClusterZoneEnrollRequest(obj->zoneType, 0);
    emberAfSetCommandEndpoints(obj->localEnp, obj->cieEnp);
    emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, obj->cieNwkAddr);
}

/** @brief
 *
 *
 */
void iasServe_ReceiveEnrollResponseCallback(uint8_t code, uint8_t _zoneId, IASZone_t* obj)
{
    if (code == IAS_ENROLL_SUCCESS)
    {
        obj->zoneId = _zoneId;

        emberAfWriteServerAttribute(obj->localEnp, \
                                    ZCL_IAS_ZONE_CLUSTER_ID, \
                                    ZCL_ZONE_ID_ATTRIBUTE_ID, \
                                    &_zoneId, \
                                    ZCL_INT8U_ATTRIBUTE_TYPE);
    }
}

/** @brief
 *
 */
void iasServe_SendStatusChangeNotify(uint16_t _zoneStatus, IASZone_t* obj)
{
//    if (obj->zoneStatus != _zoneStatus && obj->zoneId != INVALID_ZONE_ID)
//    if (obj->zoneId != INVALID_ZONE_ID)
    {
        obj->zoneStatus = _zoneStatus;

        emberAfFillCommandIasZoneClusterZoneStatusChangeNotification(_zoneStatus, 0x00, obj->zoneId, 0) ;
        emberAfSetCommandEndpoints(obj->localEnp, obj->cieEnp);
        if (obj->cieNwkAddr >= (INVALID_CIE_NWK_ADDR - 2))
            obj->cieNwkAddr = 0x0000;
        emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, obj->cieNwkAddr);
    }
}
