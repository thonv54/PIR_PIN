/*
 * ServeIAS.h
 *
 *  Created on: Aug 7, 2017
 *      Author: phuclm
 */

#ifndef BNR_EFR32_API_IAS_SERVE_H_
#define BNR_EFR32_API_IAS_SERVE_H_

#include "app/framework/include/af.h"

#define IAS_ZONE_TYPE_CONTACT   (0x0015)
#define IAS_ZONE_TYPE_MOTION    (0x000d)

#define INVALID_CIE_NWK_ADDR    (0xffff)
#define INVALID_ZONE_ID         (0xff)
#define IAS_ENROLL_SUCCESS      (0x00)

typedef void (*cieNwkAddrResponseCallback_t)(const EmberAfServiceDiscoveryResult* result);

typedef struct {
    uint8_t localEnp;
    uint8_t zoneType;
    uint8_t zoneId;
    uint16_t zoneStatus;

    uint8_t cieEnp;
    uint16_t cieNwkAddr;
    uint8_t cieIeeeAddr[8];

    cieNwkAddrResponseCallback_t cieNwkAddrResponseCallback;
} IASZone_t;

void iasServe_Init(IASZone_t* obj);
void iasServe_ReceiveCieAddrCallback(uint8_t* _CIEIeee, IASZone_t* obj);
void iasServe_SendEnrollRequest(IASZone_t* obj);
void iasServe_ReceiveEnrollResponseCallback(uint8_t code, uint8_t _zoneId, IASZone_t* obj);
void iasServe_SendStatusChangeNotify(uint16_t _zoneStatus, IASZone_t* obj);

#endif /* BNR_EFR32_API_IAS_SERVE_H_ */
