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
 * Last Changed:     Date: Jun 24, 2016 12:45:31 AM 
 *
 ****************************************************************************/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "em_device.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"

#include "bnr_hw_gpio.h"
#include "bnr_hw_ADC.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
static volatile boolean adcConversionComplete = FALSE;
static volatile uint32_t ADCResult;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
static uint32_t
lumiAdcGetPosSelLoc(
		LumiGpioLocation gpioLoc
);

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
void ADC0_IRQHandler(void) {
	uint32_t flags;

	/* Clear interrupt flags */
	flags = ADC_IntGet(ADC0 );
	ADC_IntClear(ADC0, flags);

	/** Must read data for clear Flag */
	ADCResult = ADC0->SINGLEDATA;

	adcConversionComplete = true;
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
static uint32_t
lumiAdcGetPosSelLoc(
		LumiGpioLocation gpioLoc
) {
	uint32_t adcPosSelLoc;
	switch (gpioLoc) {
	case PF0:
		adcPosSelLoc = adcPosSelAPORT1XCH16;
		break;
	case PF1:
		adcPosSelLoc = adcPosSelAPORT1YCH17;
		break;
	case PF2:
		adcPosSelLoc = adcPosSelAPORT1XCH18;
		break;
	case PF3:
		adcPosSelLoc = adcPosSelAPORT1YCH19;
		break;
	case PD13:
		adcPosSelLoc = adcPosSelAPORT3YCH5;
		break;
	case PD14:
		adcPosSelLoc = adcPosSelAPORT3XCH6;
		break;
	case PD15:
		adcPosSelLoc = adcPosSelAPORT3YCH7;
		break;
	case PA0:
		adcPosSelLoc = adcPosSelAPORT3XCH8;
		break;
	case PA1:
		adcPosSelLoc = adcPosSelAPORT3YCH9;
		break;
	case PB11:
		adcPosSelLoc = adcPosSelAPORT3YCH27;
		break;
	case PB12:
		adcPosSelLoc = adcPosSelAPORT3XCH28;
		break;
	case PB13:
		adcPosSelLoc = adcPosSelAPORT3YCH29;
		break;
	case PB14:
		adcPosSelLoc = adcPosSelAPORT3XCH30;
		break;
	case PB15:
		adcPosSelLoc = adcPosSelAPORT3YCH31;
		break;
	case PC10:
		adcPosSelLoc = adcPosSelAPORT1XCH10;
		break;
	case PC11:
		adcPosSelLoc = adcPosSelAPORT1YCH11;
		break;
	}

	return adcPosSelLoc;
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
void lumiADCInit(
		LumiGpioLocation gpioLoc,
		LumiAdcReference ref,
		LumiAdcResolution res,
		boolean measureBattery,
		boolean useInterrupt
) {
	/* Enable clock for ADC0 */
	CMU_ClockEnable(cmuClock_ADC0, true);

	/** ADC0 Init
	 * Shutdown after each conversion.
	 *  */
	ADC_Init_TypeDef ADC0_init = ADC_INIT_DEFAULT;

	ADC0_init.ovsRateSel = adcOvsRateSel2;
	ADC0_init.warmUpMode = adcWarmupNormal;
	ADC0_init.timebase = ADC_TimebaseCalc(0);
	ADC0_init.prescale = ADC_PrescaleCalc(7000000, 0);
	ADC0_init.tailgate = 0;
	ADC0_init.em2ClockConfig = adcEm2Disabled;

	ADC_Init(ADC0, &ADC0_init);
	// [ADC0_Init]$

	// $[ADC0_InputConfiguration]
	ADC_InitSingle_TypeDef ADC0_init_single = ADC_INITSINGLE_DEFAULT;

	/* PRS settings */
	ADC0_init_single.prsEnable = 0;
	ADC0_init_single.prsSel = adcPRSSELCh0;

	/* Input(s) */
	ADC0_init_single.diff = 0;
	/** If don't measure battery, enable gpio mode */
	if (!measureBattery) {
		GPIO_PinModeSet((GPIO_Port_TypeDef)lumiGpioGetPortLoc(gpioLoc), lumiGpioGetPinLoc(gpioLoc), gpioModeInput, 0);
		ADC0_init_single.posSel = (ADC_PosSel_TypeDef)lumiAdcGetPosSelLoc(gpioLoc);
	} else {
		ADC0_init_single.posSel = adcPosSelAVDD;
	}
	ADC0_init_single.negSel = adcNegSelVSS;

	/** Select Reference Voltage */
	switch (ref) {
	case ADC_REF_1V25:
		ADC0_init_single.reference = adcRef1V25;
		break;
	case ADC_REF_2V5:
		ADC0_init_single.reference = adcRef2V5;
		break;
	case ADC_REF_5V:
		ADC0_init_single.reference = adcRef5VDIFF;
		break;
	case ADC_REF_VDD:
		ADC0_init_single.reference = adcRefVDD;
		break;
	default:
		ADC0_init_single.reference = adcRef2V5;
		break;
	}

	/* Generic conversion settings */
	ADC0_init_single.acqTime = adcAcqTime1;

	/** Select resolution */
	switch (res) {
	case ADC_RES_6:
		ADC0_init_single.resolution = adcRes6Bit;
		break;
	case ADC_RES_8:
		ADC0_init_single.resolution = adcRes8Bit;
		break;
	case ADC_RES_12:
		ADC0_init_single.resolution = adcRes12Bit;
		break;
	default:
		ADC0_init_single.resolution = adcRes12Bit;
		break;
	}

	ADC0_init_single.leftAdjust = 0;
	ADC0_init_single.rep = 0;
	ADC0_init_single.singleDmaEm2Wu = 0;
	ADC0_init_single.fifoOverwrite = 0;

	ADC_InitSingle(ADC0, &ADC0_init_single);

	/* Enable interrupt if was select*/
	if (useInterrupt) {
		NVIC_ClearPendingIRQ(ADC0_IRQn);
		NVIC_EnableIRQ(ADC0_IRQn);
		ADC_IntEnable(ADC0, ADC_IF_SINGLE);
	} else {
		adcConversionComplete = false;
	}
}
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
uint32_t
lumiADCGetValue(
		LumiGpioLocation gpioLoc,
		LumiAdcReference ref,
		LumiAdcResolution res,
		boolean measureBattery,
		boolean useInterrupt
) {
	ADC_Reset(ADC0 );
	lumiADCInit(gpioLoc, ref, res, measureBattery, useInterrupt);

	if (useInterrupt) {
		adcConversionComplete = false;
		ADC_Start(ADC0, adcStartSingle);
		while (!adcConversionComplete) {
			EMU_EnterEM1();
		}
	} else {
		ADC_Start(ADC0, adcStartSingle);
		halCommonDelayMilliseconds(1);
		ADCResult = ADC0->SINGLEDATA;
	}

	ADC_Reset(ADC0 );

	return ADCResult;
}

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
void
lumiADCEnable(
		LumiGpioLocation gpioLoc,
		boolean enable
) {
	/* Ember API can't Deactive specific inputPin*/
	if (enable == FALSE) {
		ADC_Reset(ADC0);
	}
}

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
void
lumiADCConfigure(
		LumiGpioLocation gpioLoc,
		LumiAdcReference ref,
		LumiAdcResolution res,
		boolean measureBattery,
		boolean useInterrupt
) {
	lumiADCInit(gpioLoc, ref, res, measureBattery, useInterrupt);
}
