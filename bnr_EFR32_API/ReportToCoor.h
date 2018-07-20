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
 * Last Changed:     Date: Jul 4, 2016 10:18:33 AM
 *
 ****************************************************************************/
#ifndef REPORTTOCOOR_H_
#define REPORTTOCOOR_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#define SUCCESS	0x00


typedef struct {
	uint16_t 	destination;
	uint8_t 	destinationEndpoint;
	uint8_t 	sourceEndpoint;
	uint16_t 	clusterId;
	uint8_t* 	valueBuffer;
	uint16_t 	valueBufferLen;
} ReadAtrRspFrame;

typedef struct {
	uint16_t	attributeId;
	uint8_t 	status;
	uint8_t 	dataType;
	uint8_t*	attributeBuffer;
	uint16_t	attributeBufferLen;
} ReadAtrRspAttributeRecord;

typedef struct {
	uint16_t destination;
	uint8_t destinationEndpoint;
	uint8_t sourceEndpoint;
	uint16_t clusterId;
	uint16_t attributeId;
	uint8_t attributeDataType;
	uint8_t* attributeValueBuffer;
	uint16_t attributeValueBufferLen;
} ReportAttributeFrame;

typedef struct {
	uint16_t attributeId;
	uint8_t attributeDataType;
	uint8_t* attributeBuffer;
} ReportAttributeData;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/
extern uint8_t* atrValRepBuf;
extern ReadAtrRspFrame atrValFrame;
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
boolean ReadAttributeSuccessResponseCommand(uint16_t desAddr, uint8_t desEnp, uint8_t srcEnp, uint16_t clusId, uint8_t* buf, uint16_t bufLen);

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
boolean ReportAttributeCommand(uint16_t desAddr, uint8_t desEnp, uint8_t srcEnp, uint16_t clusId, uint8_t* buf, uint16_t bufLen);
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
        EmberAfAttributeId attributeId, int8u* value, int8u dataType);


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
void SendOnOffViaBindingTable(int8u* value);



#endif /* REPORTTOCOOR_H_ */
