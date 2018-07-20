/***************************************************************************/
#include <stddef.h>
#include "i2c-driver.h"
#include "i2cspm.h"

#include "opt3001.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define OPT3001_ADDR_GND        0x88
#define OPT3001_ADDR_VDD        0x8A
#define OPT3001_ADDR_SDA        0x8C
#define OPT3001_ADDR_SCL        0x8E

#define CONFIG_CMD_LSB			0x00
#define CONFIG_CMD_MSB			0xC2

#define RESULT_REG              0x00
#define CONFIG_REG              0x01
#define LOWLIM_REG              0x02
#define HIGHLIM_REG             0x03
#define MANUID_REG              0x7E
#define DEVID_REG               0x7F

#define CONFIG_RN_MASK          (0xF << 12) /* [15..12] Range Number */
#define CONFIG_RN_AUTO          (0xC << 12)

#define CONFIG_CT               BIT(11) /* [11] Conversion Time */

#define CONFIG_MC_MASK          (0x3 << 9) /* [10..9] Mode of Conversion */
#define CONFIG_MC_SHUTDOWN      (0 << 9)
#define CONFIG_MC_SINGLE        (0x1 << 9)
#define CONFIG_MC_CONTINUOS     (0x2 << 9)

#define CONFIG_OVF              BIT(8) /* [8] Overflow */
#define CONFIG_CRF              BIT(7) /* [7] Conversion Ready Field */
#define CONFIG_FH               BIT(6) /* [6] Flag High */
#define CONFIG_FL               BIT(5) /* [5] Flag Low */
#define CONFIG_L                BIT(4) /* [4] Latch */
#define CONFIG_POL              BIT(3) /* [3] Polarity */
#define CONFIG_ME               BIT(2) /* [2] Mask Exponent */

#define CONFIG_FC_MASK          (0x3 << 0)/* [1..0] Fault Count */

#define STATE_SLEEPING          0
#define STATE_ACTIVE            1
#define STATE_DATA_READY        2

#define GET_EXP(n)              ((n) >> 12)
#define GET_MATISSA(n)          ((n) & 0xFFFF)

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
uint8_t LSB;
uint8_t MSB;

/**************************************************************************/
int32_t Opt3001_GetRegVal(I2C_TypeDef *i2c, uint8_t command) {

	I2C_TransferSeq_TypeDef seq;
	I2C_TransferReturn_TypeDef ret;
	uint8_t i2c_read_data[2];
	uint8_t i2c_write_data[1];

	seq.addr = OPT3001_ADDR_GND;
	seq.flags = I2C_FLAG_WRITE_READ;
	/* Select command to issue */
	i2c_write_data[0] = command;
	seq.buf[0].data = i2c_write_data;
	seq.buf[0].len = 1;
	/* Select location/length of data to be read */
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len = 2;

	ret = I2CSPM_Transfer(i2c, &seq);

	if (ret != i2cTransferDone) {
		return ((int) ret);
	}

	MSB = i2c_read_data[0];
	LSB = i2c_read_data[1];

	return ((int) 2);
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
uint16_t Opt3001_GetLuxVal(void) {
	uint16_t rawData, tmpData, LSB_Size;
	uint16_t retVal = 0;

	/* Read Lux value */
	if (Opt3001_GetRegVal(I2C0, RESULT_REG) == 2)
	    rawData = ((uint16_t) (MSB) << 8) + (uint16_t) (LSB);
	else
	    rawData = 0;

	/* Formula  */
	LSB_Size = 1 << ((rawData & 0xf000) >> 12); // LSB_Size = 2 ^ exponent
	tmpData = (rawData & 0x0fff) * LSB_Size; // lux = R[11:0] * LSB_Size * 0.01

	if ((tmpData % 100) <= 5) {
		retVal = (tmpData / 10);
	} else {
		retVal = (tmpData / 10) + 1;
	}

//	/* Send configure data include start measure command to Opt3001 */
//	halI2cWriteBytes(OPT3001_ADDR_GND, CONFIG_CMD_DATA, 3);

	return retVal;
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
void Opt3001_MeasureLux(void) {
	const uint8_t CONFIG_CMD_DATA[3] = { CONFIG_REG, CONFIG_CMD_MSB,
			CONFIG_CMD_LSB };

	/* Send configure data include start measure command to Opt3001 */
	halI2cWriteBytes(OPT3001_ADDR_GND, CONFIG_CMD_DATA, 3);
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
uint16_t Opt3001_GetDevIdVal(void) {

	Opt3001_GetRegVal(I2C0, DEVID_REG);

	return (uint16_t) (MSB << 8) + (uint16_t) (LSB);
}
