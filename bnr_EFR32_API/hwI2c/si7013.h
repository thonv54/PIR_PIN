/***************************************************************************/
#ifndef __SI7013_H
#define __SI7013_H

#include "em_device.h"
#include <stdbool.h>

/***************************************************************************/
/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define SI7013_ADDR      0x82

#define SI7020_ADDR      0x80

#define SI7020_MEASURE_RH_8_T_12	0x3B

#define SI7013_DEVICE_ID 0x0D

#define SI7020_DEVICE_ID 0x14

#define SI7021_DEVICE_ID 0x21

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

int32_t Si7013_MeasureRHAndTemp(I2C_TypeDef *i2c, uint8_t addr,
                                 uint32_t *rhData, int32_t *tData);

int32_t Si7013_GetFirmwareRevision(I2C_TypeDef *i2c, uint8_t addr, uint8_t *fwRev);

bool Si7013_Detect(I2C_TypeDef *i2c, uint8_t addr, uint8_t *deviceId);
int32_t Si7013_ReadNoHoldRHAndTemp(I2C_TypeDef *i2c, uint8_t addr, uint32_t *rhData,
                                int32_t *tData);
int32_t Si7013_StartNoHoldMeasureRHAndTemp(I2C_TypeDef *i2c, uint8_t addr);
int32_t Si7013_MeasureV(I2C_TypeDef *i2c, uint8_t addr, int32_t *vData);

int32_t Si7013_ReadUserReg1(I2C_TypeDef *i2c, uint8_t addr, uint16_t *userReg);
int32_t Si7013_WriteUserReg1(I2C_TypeDef *i2c, uint8_t addr, int8_t data);
#ifdef __cplusplus
}
#endif

#endif /* __SI7013_H */
