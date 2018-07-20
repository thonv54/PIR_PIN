/***************************************************************************/
#include <stddef.h>
#include "si7013.h"
#include "i2cspm.h"

#include "stddef.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define SI7013_READ_TEMP       0xE0  /* Read previous T data from RH measurement
                                      * command*/

#define SI7013_READ_RH         0xE5  /* Perform RH (and T) measurement. */

#define SI7013_READ_RH_NH      0xF5  /* Perform RH (and T) measurement in no hold mode. */

#define SI7013_READ_VIN        0xEE  /* Perform thermistor measurement. */

#define SI7013_READ_ID1_1      0xFA
#define SI7013_READ_ID1_2      0x0F
#define SI7013_READ_ID2_1      0xFc
#define SI7013_READ_ID2_2      0xc9

#define SI7013_READ_FWREV_1    0x84
#define SI7013_READ_FWREV_2    0xB8

#define SI7020_WRITE_USER1_CMD	0xE6
#define SI7020_READ_USER1_CMD	0xE7

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/


/**************************************************************************/
static int32_t Si7013_Measure(I2C_TypeDef *i2c, uint8_t addr, uint32_t *data,
                              uint8_t command)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = command;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
    *data = 0;
    return((int) ret);
  }

  *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

  return((int) 2);
}
/**************************************************************************/
static int32_t Si7013_StartNoHoldMeasure(I2C_TypeDef *i2c, uint8_t addr, uint8_t command)

{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select command to issue */
  i2c_write_data[0] = command;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
    return((int) ret);
  }

  return((int) 0);
}
/**************************************************************************/
int32_t Si7013_GetFirmwareRevision(I2C_TypeDef *i2c, uint8_t addr, uint8_t *fwRev)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_write_data[2];
  uint8_t                    i2c_read_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = SI7013_READ_FWREV_1;
  i2c_write_data[1] = SI7013_READ_FWREV_2;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 1;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
    *fwRev = 0;
    return (uint32_t) ret;
  }
  *fwRev = i2c_read_data[0];

  return (uint32_t) i2cTransferDone;
}

/**************************************************************************/
int32_t Si7013_StartNoHoldMeasureRHAndTemp(I2C_TypeDef *i2c, uint8_t addr)
{
  int ret = Si7013_StartNoHoldMeasure(i2c, addr, SI7013_READ_RH_NH);

  return ret;
}

/**************************************************************************/
static int32_t Si7013_ReadNoHoldData(I2C_TypeDef *i2c, uint8_t addr, uint32_t *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_READ;
  /* Select command to issue */
  seq.buf[0].data = i2c_read_data;
  seq.buf[0].len  = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
    *data = 0;
    return((int) ret);
  }

  *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

  return((int) 2);
}
/**************************************************************************/
int32_t Si7013_ReadNoHoldRHAndTemp(I2C_TypeDef *i2c, uint8_t addr, uint32_t *rhData,
                                   int32_t *tData)
{
  int ret = Si7013_ReadNoHoldData(i2c, addr, rhData);

  if (ret == 2)
  {
    /* convert to milli-percent */
    *rhData = (((*rhData) * 15625L) >> 13) - 6000;
  }
  else
  {
    return -1;
  }

  ret = Si7013_Measure(i2c, addr, (uint32_t *) tData, SI7013_READ_TEMP);

  if (ret == 2)
  {
    *tData = (((*tData) * 21965L) >> 13) - 46850; /* convert to milli-degC */
  }
  else
  {
    return -1;
  }

  return 0;
}

/**************************************************************************/
int32_t Si7013_MeasureRHAndTemp(I2C_TypeDef *i2c, uint8_t addr, uint32_t *rhData,
                                int32_t *tData)
{
  int ret = Si7013_Measure(i2c, addr, rhData, SI7013_READ_RH);

  if (ret == 2)
  {
    /* convert to milli-percent */
    *rhData = (((*rhData) * 15625L) >> 13) - 6000;
  }
  else
  {
    return -1;
  }

  ret = Si7013_Measure(i2c, addr, (uint32_t *) tData, SI7013_READ_TEMP);

  if (ret == 2)
  {
    *tData = (((*tData) * 21965L) >> 13) - 46850; /* convert to milli-degC */
  }
  else
  {
    return -1;
  }

  return 0;
}

static int32_t Si7013_WriteUserReg2(I2C_TypeDef *i2c, uint8_t addr, int8_t data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[2];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select command to issue */
  i2c_write_data[0] = 0x50;
  i2c_write_data[1] = data;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
    return((int) ret);
  }

  return((int) 0);
}
/**************************************************************************/
int32_t Si7013_MeasureV(I2C_TypeDef *i2c, uint8_t addr, int32_t *vData)
{
  int ret;
  Si7013_WriteUserReg2(i2c, addr, 0x0e);
  ret = Si7013_Measure(i2c, addr, (uint32_t *) vData, SI7013_READ_VIN);
  Si7013_WriteUserReg2(i2c, addr, 0x09);
  if (ret == 2)
  {
    /* convert  */
  }
  else
  {
    return -1;
  }
  return 0;
}


/**************************************************************************/
bool Si7013_Detect(I2C_TypeDef *i2c, uint8_t addr, uint8_t *deviceId)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[8];
  uint8_t                    i2c_write_data[2];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = SI7013_READ_ID2_1;
  i2c_write_data[1] = SI7013_READ_ID2_2;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 8;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone)
  {
    return false;
  }
  if (NULL != deviceId)
  {
    *deviceId = i2c_read_data[0];
  }
  return true;
}

/**************************************************************************/
int32_t Si7013_WriteUserReg1(I2C_TypeDef *i2c, uint8_t addr, int8_t data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[2];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select command to issue */
  i2c_write_data[0] = SI7020_WRITE_USER1_CMD;
  i2c_write_data[1] = data;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
    return((int) ret);
  }

  return((int) 0);
}

/**************************************************************************/
int32_t Si7013_ReadUserReg1(I2C_TypeDef *i2c, uint8_t addr, uint16_t *userReg)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = SI7020_READ_USER1_CMD;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone)
  {
	  return((int) ret);
  }

  *userReg = (((uint16_t) i2c_read_data[0] << 8) + (uint16_t)(i2c_read_data[1]));

  return((int) 0);
}
