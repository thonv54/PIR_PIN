/****************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 * Author: phuclm
 *
 * Last Changed By:  Author: phuclm
 * Revision:         Revision: 1.0.0.1
 * Last Changed:     Date: Jun 3, 2016 4:22:01 PM
 *
 ****************************************************************************/
//#define TEST_PIR_DISTANCE
//#define TEST_PRECISION_OF_SENSOR
/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <math.h>

#include "app/framework/include/af.h"
#include "em_emu.h"

#include "macros.h"
#include "common.h"

#include "si7013.h"
#include "opt3001.h"
#include "bnr_hw_ADC.h"
#include "lm_pir_sensor.h"

#include "InitSensors.h"
#include "bnr_zb_form_and_join.h"
#include "LocalIndicate.h"
#include "ReportToCoor.h"
#include "stdbool.h"

#include "IAS_Serve.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#ifndef BTN_PRSED
#define BTN_PRSED                       (1)
#endif
#ifndef RELEASED
#define RELEASED                        (0)
#endif
#define HAS_PULSE                       (1)
#define NO_PULSE                        (0)

/* Define PIR pulse */
#define LOW_RISE                        (0)
#define HIGH_FALL                       (1)

/* Define TRUE PIR pulse width */
#define TRUE_PIR_PULSE_WIDTH_MS         100
#define MOTION_DETECTION_TIMEOUT_MS     5000
#define TRUE_MOTION_PIR_NUM_OF_PULSE    2

/* Send thresholds */
#ifndef SEND_THRESHOLD_LUX
#define SEND_THRESHOLD_LUX              5
#endif
#ifndef SEND_THRESHOLD_HUMI
#define SEND_THRESHOLD_HUMI             1
#endif
#ifndef SEND_THRESHOLD_TEMP
#define SEND_THRESHOLD_TEMP             1
#endif
#ifndef SEND_THRESHOLD_BAT
#define SEND_THRESHOLD_BAT              1
#endif
#ifndef SENSORS_COVER_FACTOR
#define SENSORS_COVER_FACTOR            1
#endif

#ifndef MAX_NO_CHANGE
#define MAX_NO_CHANGE                   30
#endif

#ifndef TIME_WAIT_PIR_STABLE
#define TIME_WAIT_PIR_STABLE            5000
#endif

#ifndef WAIT_FOR_CC2530_GET_INFO
#define WAIT_FOR_CC2530_GET_INFO        60000
#endif

#ifndef TIME_BETWEEN_TWO_LUX_MEASURE
#define TIME_BETWEEN_TWO_LUX_MEASURE    60000
#endif
#ifndef TIME_CHECK_SENSORS_CYCLE
#define TIME_CHECK_SENSORS_CYCLE        60000
#endif

#ifndef TIME_REJOIN_SCAN
#define TIME_REJOIN_SCAN                30
#endif
#ifndef TIME_FORCE_REJOIN_MS
#define TIME_FORCE_REJOIN_MS            30000
#endif

/* Define state of Device, check ZoneStatus attribute bit map for more detail */
#define PIR_NO_MOTION                   (0x0000)
#ifdef FOR_ZIPATO_HC
#define PIR_MOTION_DETECTED             (0x0003) // alarmed
#else
#define PIR_MOTION_DETECTED             (0x0001) // alarmed
#endif
#define PIR_NOT_STABLE                  (0x0080) // AC fault
#define PIR_CHECKING                    (0xffff)

/* Define endpoints mask */
#define LUX_ENDPOINT                    (2)
#define TEMP_ENDPOINT                   (3)
#define HUMI_ENDPOINT                   (4)
#define BATTERY_ENDPOINT                (5)

#ifndef FOR_ZIPATO_HC
#define HC_ENDPOINT                     (0x01)
#else
#define HC_ENDPOINT                     (0x0C)
#endif

#define ZDO_PROFILE_ID                  (0x0000)
/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
static boolean lockDevice = false;

static boolean pirStateWasChanged = false;
static uint16_t currentPirState = PIR_NOT_STABLE;
static uint16_t pirStateToSend = PIR_NOT_STABLE;

static uint8_t noChangeLuxCount = 0;
static uint8_t noChangeHumiCount = 0;
static uint8_t noChangeTempCount = 0;
static uint8_t noChangeBatCount = 0;
static uint8_t noChangePirCount = 0;

static boolean forceReport = false;
static boolean nwkUp = false;
static boolean serveZDPMessages = false;
static uint32_t timePointNetworkUp = 0;

static boolean checkingNwkStatus = true;

static boolean buttonHold1Second = false;

uint16_t currentLux;
uint16_t currentHumidity;
uint16_t avgHumi;
uint16_t currentTemperture;
uint16_t avgTemp;
uint8_t currentBattery;
uint8_t avgBat;

uint32_t timePointGetLuxValue = 0;
boolean needCurrentLuxValue = true;
boolean mustMeasureOtherSensor = true;

#ifdef FOR_ZIPATO_HC
static IASZone_t motionSensorIas;
#endif

EmberEventControl lumiHalInitEventControl;
EmberEventControl CheckPirStateEventControl;
EmberEventControl PerformSensorsMeasurementEventControl;
EmberEventControl DeviceAnnounceEventControl;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
// Functions prototype
void lumiHalInitEventFunction(void);
void CheckPirStateEventFunction(void);
void PerformSensorsMeasurementEventFunction(void);
void DeviceAnnounceEventFunction(void);

EmberStatus WriteServerAttribute();
void ReportToCoordinator(void);
void ValidateSensorsValue(void);
uint16_t CalAvg(uint16_t arr[], uint8_t n);


// Functions define
/**
 * @func    None
 * @brief   None
 * @param   None
 * @retval  None
 */
uint16_t CalAvg(uint16_t arr[], uint8_t n) {
    uint16_t retVal = 0;
    for (uint8_t i = 0; i < n; i++) {
        retVal += arr[i];
    }

    return (retVal / n);
}

/**
 * @func    None
 * @brief   None
 * @param   None
 * @retval  None
 */
void ValidateSensorsValue(void) {
    static uint8_t index = 0;
    static uint16_t bat[5] = {100, 100, 100, 100, 100};
    static uint16_t humi[5] = {75, 75, 75, 75, 75};
    static uint16_t temp[5] = {25, 25, 25, 25, 25};

    bat[index] = (uint16_t)currentBattery;
    humi[index] = currentHumidity;
    temp[index] = currentTemperture;

    avgBat = (uint8_t)CalAvg(bat, 5);
    avgHumi = CalAvg(humi, 5);
    avgTemp = CalAvg(temp, 5);

    index += 1;
    if (index > 4) {
        index = 0;
    }

    if (avgBat < MIN_BAT) {
        avgBat = MIN_BAT;
    }
    if (avgBat > MAX_BAT) {
        avgBat = MAX_BAT;
    }

    if (avgHumi < MIN_HUMI) {
        avgHumi = MIN_HUMI;
    }
    if (avgHumi > MAX_HUMI) {
        avgHumi = MAX_HUMI;
    }

    if (currentLux < MIN_LUX) {
        currentLux = MIN_LUX;
    }
    if (currentLux > MAX_LUX) {
        currentLux = MAX_LUX;
    }

    if (avgTemp < MIN_TEMP) {
        avgTemp = MIN_TEMP;
    }
    if (avgTemp > MAX_TEMP) {
        avgTemp = MAX_TEMP;
    }
}

/**
 * @func    None
 * @status  building...
 * @brief   None
 * @param   None
 * @retval  None
 */
EmberStatus WriteServerAttribute() {
    EmberStatus status = EMBER_SUCCESS;

    status |= emberAfWriteServerAttribute(PIR_ENDPOINT,
            ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,
            (uint8_t*) &pirStateToSend, ZCL_BITMAP16_ATTRIBUTE_TYPE);

    status |= emberAfWriteServerAttribute(LUX_ENDPOINT,
            ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
            ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID, (uint8_t*) &currentLux,
            ZCL_INT16U_ATTRIBUTE_TYPE);

    status |= emberAfWriteServerAttribute(TEMP_ENDPOINT,
            ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
            ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID, (uint8_t*) &avgTemp,
            ZCL_INT16S_ATTRIBUTE_TYPE);

    status |= emberAfWriteServerAttribute(HUMI_ENDPOINT,
            ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID,
            ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID,
            (uint8_t*) &avgHumi, ZCL_INT16U_ATTRIBUTE_TYPE);

    status |= emberAfWriteServerAttribute(BATTERY_ENDPOINT,
            ZCL_POWER_CONFIG_CLUSTER_ID,
            ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID, &avgBat,
            ZCL_INT8U_ATTRIBUTE_TYPE);

    return status;
}

/** @brief
 *
 */
void ReportToCoordinator(void) {
    uint16_t clusId;
    uint8_t buf[25];
    uint16_t bufLen;

    static uint16_t sentLux = 0;
    static uint16_t sentHumi = 0;
    static uint16_t sentTemp = 0;
    static uint8_t sentBat = 0;

    static boolean needConfirmPirState = false;

    if (pirStateWasChanged) {

        noChangePirCount = 0;

        pirStateWasChanged = false;
#ifndef FOR_ZIPATO_HC
        clusId = ZCL_IAS_ZONE_CLUSTER_ID;
        buf[0] = uint16GetLSB(ZCL_ZONE_STATUS_ATTRIBUTE_ID);
        buf[1] = uint16GetMSB(ZCL_ZONE_STATUS_ATTRIBUTE_ID);
        buf[2] = SUCCESS;
        buf[3] = ZCL_BITMAP16_ATTRIBUTE_TYPE;
        buf[4] = uint16GetLSB(pirStateToSend);
        buf[5] = uint16GetMSB(pirStateToSend);
        bufLen = 6;
        ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, PIR_ENDPOINT, clusId, buf, bufLen);


        if(pirStateToSend == PIR_MOTION_DETECTED){
        	int8u OnOffValue = 1;
        	SendOnOffViaBindingTable((int8u*) &OnOffValue);
        }
        if(pirStateToSend == PIR_NO_MOTION){
        	int8u OnOffValue = 0;
        	SendOnOffViaBindingTable((int8u*) &OnOffValue);
        }
#else
        iasServe_SendStatusChangeNotify(pirStateToSend, &motionSensorIas);
#endif

        clusId = ZCL_ILLUM_MEASUREMENT_CLUSTER_ID;
        buf[0] = uint16GetLSB(ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID);
        buf[1] = uint16GetMSB(ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID);
        buf[2] = SUCCESS;
        buf[3] = ZCL_INT16U_ATTRIBUTE_TYPE;
#ifndef FOR_ZIPATO_HC
        buf[4] = uint16GetLSB(currentLux);
        buf[5] = uint16GetMSB(currentLux);
#else
        buf[4] = uint16GetLSB(10000 * (uint16_t)log10((double_t)currentLux) + 1);
        buf[5] = uint16GetMSB(10000 * (uint16_t)log10((double_t)currentLux) + 1);
#endif
        bufLen = 6;
        if (abs(currentLux - sentLux) > SEND_THRESHOLD_LUX) {
            ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, LUX_ENDPOINT, clusId, buf, bufLen);
            sentLux = currentLux;
        }

        if (emberNetworkState() == EMBER_JOINED_NETWORK)
        {
            needConfirmPirState = true;
        }

        deviceState = DEVICE_SPECIFIC_STATE;
        emberEventControlSetDelayMS(LocalIndicateEventControl, 300);

    } else {

        clusId = ZCL_ILLUM_MEASUREMENT_CLUSTER_ID;
        buf[0] = uint16GetLSB(ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID);
        buf[1] = uint16GetMSB(ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID);
        buf[2] = SUCCESS;
        buf[3] = ZCL_INT16U_ATTRIBUTE_TYPE;
#ifndef FOR_ZIPATO_HC
        buf[4] = uint16GetLSB(currentLux);
        buf[5] = uint16GetMSB(currentLux);
#else
        buf[4] = uint16GetLSB(10000 * (uint16_t)log10((double_t)currentLux) + 1);
        buf[5] = uint16GetMSB(10000 * (uint16_t)log10((double_t)currentLux) + 1);
#endif
        bufLen = 6;
        if ((abs(currentLux - sentLux) > SEND_THRESHOLD_LUX) || (noChangeLuxCount > MAX_NO_CHANGE) || forceReport) {
            noChangeLuxCount = 0;
            ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, LUX_ENDPOINT, clusId, buf, bufLen);
            sentLux = currentLux;
        } else {
            noChangeLuxCount += 1;
        }

        clusId = ZCL_TEMP_MEASUREMENT_CLUSTER_ID;
        buf[0] = uint16GetLSB(ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID);
        buf[1] = uint16GetMSB(ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID);
        buf[2] = SUCCESS;
        buf[3] = ZCL_INT16S_ATTRIBUTE_TYPE;
#ifndef FOR_ZIPATO_HC
        buf[4] = uint16GetLSB(avgTemp);
        buf[5] = uint16GetMSB(avgTemp);
#else
        buf[4] = uint16GetLSB(avgTemp * 100);
        buf[5] = uint16GetMSB(avgTemp * 100);
#endif
        bufLen = 6;
        if ((abs(avgTemp - sentTemp) > SEND_THRESHOLD_TEMP) || (noChangeTempCount > MAX_NO_CHANGE) || forceReport) {
            noChangeTempCount = 0;
            ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, TEMP_ENDPOINT, clusId, buf, bufLen);
            sentTemp = avgTemp;
        } else {
            noChangeTempCount += 1;
        }

        clusId = ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID;
        buf[0] = uint16GetLSB(ZCL_RELATIVE_HUMIDITY_ATTRIBUTE_ID);
        buf[1] = uint16GetMSB(ZCL_RELATIVE_HUMIDITY_ATTRIBUTE_ID);
        buf[2] = SUCCESS;
        buf[3] = ZCL_INT16U_ATTRIBUTE_TYPE;
#ifndef FOR_ZIPATO_HC
        buf[4] = uint16GetLSB(avgHumi);
        buf[5] = uint16GetMSB(avgHumi);
#else
        buf[4] = uint16GetLSB(avgHumi * 100);
        buf[5] = uint16GetMSB(avgHumi * 100);
#endif
        bufLen = 6;
        if ((abs(avgHumi - sentHumi) > SEND_THRESHOLD_HUMI) || (noChangeHumiCount > MAX_NO_CHANGE) || forceReport) {
            noChangeHumiCount = 0;
            ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, HUMI_ENDPOINT, clusId, buf, bufLen);
            sentHumi = avgHumi;
        } else {
            noChangeHumiCount += 1;
        }

        clusId = ZCL_POWER_CONFIG_CLUSTER_ID;
        buf[0] = uint16GetLSB(ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID);
        buf[1] = uint16GetMSB(ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID);
        buf[2] = SUCCESS;
        buf[3] = ZCL_INT8U_ATTRIBUTE_TYPE;
        buf[4] = avgBat;
        bufLen = 5;
        if ((abs(avgBat - sentBat) > SEND_THRESHOLD_BAT) || (noChangeBatCount > MAX_NO_CHANGE) || forceReport) {
            noChangeBatCount = 0;
            ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, BATTERY_ENDPOINT, clusId, buf, bufLen);
            sentBat = avgBat;
        } else {
            noChangeBatCount += 1;
        }

        // Pir
        if ((noChangePirCount > MAX_NO_CHANGE) || (forceReport)) {

            noChangePirCount = 0;
#ifndef FOR_ZIPATO_HC
            clusId = ZCL_IAS_ZONE_CLUSTER_ID;
            buf[0] = uint16GetLSB(ZCL_ZONE_STATUS_ATTRIBUTE_ID);
            buf[1] = uint16GetMSB(ZCL_ZONE_STATUS_ATTRIBUTE_ID);
            buf[2] = SUCCESS;
            buf[3] = ZCL_BITMAP16_ATTRIBUTE_TYPE;
            buf[4] = uint16GetLSB(pirStateToSend);
            buf[5] = uint16GetMSB(pirStateToSend);
            bufLen = 6;
            ReadAttributeSuccessResponseCommand(0x0000, HC_ENDPOINT, PIR_ENDPOINT, clusId, buf, bufLen);
#else
            iasServe_SendStatusChangeNotify(pirStateToSend, &motionSensorIas);
#endif
        } else {
            noChangePirCount += 1;
        }
    }

    if (forceReport) {
        /* Force report when:
         *  Button pressed
         *  Network up
         * when button pressed interrupt
         * was disable, then must enable PIR interrupt after force report
         * */
        if (deviceState != DEVICE_WAS_STABLE)
            enablePirSensor(TRUE);

        forceReport = false;
    }

    if (needConfirmPirState)
    {
//      needConfirmPirState = false;
//
//      forceReport = true;
//
//      mustMeasureOtherSensor = true;
//      emberEventControlSetDelayMS(PerformSensorsMeasurementEventControl, 5000);
    }

#ifdef TEST_PRECISION_OF_SENSOR
    sentLux = 0xffff;
#endif
}

/** @brief
 *
 */
#ifdef FOR_ZIPATO_HC
void cieNwkAddrResponseCallback(const EmberAfServiceDiscoveryResult* result)
{
    motionSensorIas.cieNwkAddr = result->matchAddress;

    iasServe_SendEnrollRequest(&motionSensorIas);
}
#endif

/**
 * @func    None
 * @status  building...
 * @brief   None
 * @param   None
 * @retval  None
 */
void joinSucessCallback(void) {

}

/**
 * @func    None
 * @status  building...
 * @brief   None
 * @param   None
 * @retval  None
 */
void lumiHalInitEventFunction(void)
{
#ifdef FOR_ZIPATO_HC
    motionSensorIas.cieEnp = 0x0C;
    motionSensorIas.localEnp = 0x01;
    motionSensorIas.zoneType = IAS_ZONE_TYPE_CONTACT;
    motionSensorIas.zoneStatus = 0xffff;
    motionSensorIas.cieNwkAddr = INVALID_CIE_NWK_ADDR;
    motionSensorIas.cieNwkAddrResponseCallback = cieNwkAddrResponseCallback;
    iasServe_Init(&motionSensorIas);
#endif

    emberEventControlSetInactive(lumiHalInitEventControl);

	initPirSensor(PIR_PORT, PIR_PIN, FALSE);

    initLocalIndicate();

    currentPirState = PIR_NOT_STABLE;

    serveZDPMessages = false;

    if (emberAfReadServerAttribute(PIR_ENDPOINT, ZCL_IAS_WD_CLUSTER_ID,
                                ZCL_MAX_DURATION_ATTRIBUTE_ID,
                                (uint8_t*) &pirDelayTime,
                                sizeof(pirDelayTime)) != EMBER_ZCL_STATUS_SUCCESS)
    {
        emberAfReadServerAttribute(PIR_ENDPOINT, ZCL_IAS_WD_CLUSTER_ID,
                                ZCL_MAX_DURATION_ATTRIBUTE_ID,
                                (uint8_t*) &pirDelayTime,
                                sizeof(pirDelayTime));
    }

    emberEventControlSetDelayMS(GetSensorsValueEventControl, TIME_WAIT_PIR_STABLE);
}

/**
 * @func    None
 * @status  building...
 * @brief   None
 * @param   None
 * @retval  None
 */
void emberAfMainInitCallback(void)
{
//  LumiHalInit();
    emberEventControlSetActive(lumiHalInitEventControl);

    checkingNwkStatus = true;

    nwkUp = false;
}

/** @brief
 *
 */
void inactiveAllRunningEvent(void) {
    emberEventControlSetInactive(CheckPirStateEventControl);
    emberEventControlSetInactive(DeviceAnnounceEventControl);
    emberEventControlSetInactive(FindAndJoinNetworkEventControl);
    emberEventControlSetInactive(GetSensorsValueEventControl);
    emberEventControlSetInactive(InitHardwareEventControl);
    emberEventControlSetInactive(LocalIndicateEventControl);
    emberEventControlSetInactive(PerformSensorsMeasurementEventControl);
}

/** @brief
 *
 */
void configButtonPressedCallback(uint8_t cfgBtnState) {
    if (cfgBtnState == BTN_PRSED) {
        enablePirSensor(FALSE);

        stillHasMotion = false;

        emberEventControlSetInactive(CheckPirStateEventControl);
    }
}

/** @brief
 *
 */
void buttonHold1SecondCallback(void)
{
    buttonHold1Second = true;

    needCurrentLuxValue = true;
    mustMeasureOtherSensor = true;
    emberEventControlSetDelayMS(PerformSensorsMeasurementEventControl, 300);
}

/** @brief
 *
 */
void PerformSensorsMeasurementEventFunction(void)
{
    static boolean measuredOpt3001 = false;
    uint32_t rh;
    int32_t t;

    uint32_t temp;

    EmberNetworkStatus state;

    emberEventControlSetInactive(PerformSensorsMeasurementEventControl);

    state = emberNetworkState();

    if (!sensorInitDone)
        sensorInitDone = sensor_Init();

    if (!measuredOpt3001)
    {
        Opt3001_MeasureLux();
        measuredOpt3001 = true;

        if (mustMeasureOtherSensor) {
            /* Battery calculation method:
                100% is [2.5-3]V
                ADC value [0-255] with reference is 5V
                therefore ADC value 128 is 2.5V, 153 is 3V
             */
            temp = lumiADCGetValue(PD13, ADC_REF_5V, ADC_RES_8, TRUE, TRUE);
            if (temp >= 128)
                currentBattery = (uint8_t) ((temp - 128) * 4);
            else
                currentBattery = 0;

            if (currentBattery <= 10) {
                deviceState = DEVICE_BAT_LOW;
                batteryIsLow = true;
            } else {
                batteryIsLow = false;
            }

            if (Si7013_MeasureRHAndTemp(I2C0, SI7020_ADDR, &rh, &t) == 0) // 0 mean success
            {
                currentTemperture = (uint16_t) (t / 1000);
                currentHumidity = (uint16_t) (rh / 1000);
            }
            else
            {
                currentHumidity = 0;
                currentTemperture = 0;
            }
        }

        if (needCurrentLuxValue) {
            emberEventControlSetDelayMS(PerformSensorsMeasurementEventControl, 200);
        } else {
            ValidateSensorsValue();

            WriteServerAttribute();

            ReportToCoordinator();
        }

    } else { // measuredOpt3001
        measuredOpt3001 = false;
        currentLux = Opt3001_GetLuxVal();
        timePointGetLuxValue = halCommonGetInt32uMillisecondTick();
#ifdef SENSORS_COVER_FACTOR
        currentLux = currentLux * SENSORS_COVER_FACTOR;
#endif
        ValidateSensorsValue();

        WriteServerAttribute();

        if (buttonHold1Second)
        {
            buttonHold1Second = false;

            if (state == EMBER_NO_NETWORK)
            {
                enablePirSensor(FALSE);
                emberEventControlSetActive(FindAndJoinNetworkEventControl);
            }
            else if (state == EMBER_JOINED_NETWORK)
            {
                forceReport = true;
                ReportToCoordinator();
            }
            else if (state == EMBER_JOINED_NETWORK_NO_PARENT)
            {
                enablePirSensor(TRUE);

                if (!emberStackIsPerformingRejoin())
                    emberPollForData();
            }
        }
        else
        {
            ReportToCoordinator();
        }
    }
}

/**
 * @func    None
 * @status  building...
 * @brief   None
 * @param   None
 * @retval  None
 */
void funcButtonPressedCallback(uint8_t funcButtonState)
{
    enablePirSensor(FALSE);

    currentPirState = PIR_MOTION_DETECTED;

    emberEventControlSetActive(CheckPirStateEventControl);
}

/** @brief
 *
 */
void CheckPirStateEventFunction(void) {
    static boolean newStateDetected = false;
    static boolean isChecking = false;

    EmberNetworkStatus state = emberNetworkState();

    emberEventControlSetInactive(CheckPirStateEventControl);

    if (currentPirState == PIR_MOTION_DETECTED)
    {
        motionDetectedTime = halCommonGetInt32uMillisecondTick();

        if (!stillHasMotion)
        {
            stillHasMotion = true;

            deviceState = DEVICE_MOTION_DETECTED;
            emberEventControlSetActive(LocalIndicateEventControl);
        }

        if (!isChecking)
        {
            pirStateToSend = PIR_MOTION_DETECTED;
            newStateDetected = true;

            isChecking = true;
        }

        currentPirState = PIR_NO_MOTION;

        emberEventControlSetDelayMS(CheckPirStateEventControl, pirDelayTime * 1000);
    }
    else if (currentPirState == PIR_NO_MOTION)
    {

        pirStateToSend = PIR_NO_MOTION;
        newStateDetected = true;

        isChecking = false;
    }

    if (newStateDetected) {

        newStateDetected = false;

        if (pirStateToSend == PIR_MOTION_DETECTED) {
            stillHasMotion = true;
        } else if (pirStateToSend == PIR_NO_MOTION) {
            stillHasMotion = false;
        }

        // Disable PIR interrupt before report to Coordinator
        enablePirSensor(FALSE);

        // only get lux value if it not measure before 5 seconds
        if ( abs(halCommonGetInt16uMillisecondTick() - timePointGetLuxValue) > TIME_BETWEEN_TWO_LUX_MEASURE)
        {
            needCurrentLuxValue = true;
        }
        else
        {
            needCurrentLuxValue = false;
        }

        pirStateWasChanged = true;

        mustMeasureOtherSensor = false;
        emberEventControlSetActive(PerformSensorsMeasurementEventControl);
    }
    else
    {
        // if don't need report to coordinator, enable interrupt now
        enablePirSensor(TRUE);
    }
}

/** @brief If device joined a network, every 30 seconds, get sensors value,
 *  if has difference send new value to Coordinator. If device (sleepy end device)
 *  lost it's parent, perform Rejoin each 30 minutes. If device not joined a
 *  network, after first of this event, enable GPIO pin
 */
void GetSensorsValueEventFunction(void)
{
    static uint8_t numOfAttemptRejoin = 0;
    EmberNetworkStatus state;

    emberEventControlSetInactive(GetSensorsValueEventControl);

    state = emberNetworkState();

    if (checkingNwkStatus)
    {
        checkingNwkStatus = false;

        deviceState = DEVICE_WAS_STABLE;
        emberEventControlSetActive(LocalIndicateEventControl);
    }

    switch (state)
    {
    case EMBER_JOINED_NETWORK:
        needCurrentLuxValue = true;
        mustMeasureOtherSensor = true;

        emberEventControlSetActive(PerformSensorsMeasurementEventControl);

        if (nwkUp)
        {
            nwkUp = false;

            emberEventControlSetDelayMS(GetSensorsValueEventControl, TIME_WAIT_PIR_STABLE);
        }
        else
        {
            emberEventControlSetDelayMS(GetSensorsValueEventControl, TIME_CHECK_SENSORS_CYCLE);
        }
        break;

    case EMBER_JOINED_NETWORK_NO_PARENT:
        numOfAttemptRejoin += 1;

        if (numOfAttemptRejoin > 2)
        {
            halReboot();
        }
        else
        {
            if (!emberStackIsPerformingRejoin())
                emberPollForData();
        }

        emberEventControlSetDelayMinutes(GetSensorsValueEventControl, TIME_REJOIN_SCAN);
        break;
    }

    if (currentPirState == PIR_NOT_STABLE)
    {
        currentPirState = PIR_NO_MOTION;
        pirStateToSend = PIR_NO_MOTION;
    //  enablePirSensor(TRUE);
    }
}

/**
 * @func    None
 * @brief   None
 * @param   None
 * @retval  None
 */
void networkUpCallback(void)
{
    if (!checkingNwkStatus)
    {
        deviceState = DEVICE_JOIN_SUCCESS;
        emberEventControlSetActive(LocalIndicateEventControl);
    }

    forceReport = true;

    nwkUp = true;

    pirStateWasChanged = false;

    if (emberAfReadServerAttribute(PIR_ENDPOINT, ZCL_IAS_WD_CLUSTER_ID,
            ZCL_MAX_DURATION_ATTRIBUTE_ID,
            (uint8_t*) &pirDelayTime,
            sizeof(pirDelayTime)) != EMBER_ZCL_STATUS_SUCCESS)
    {
        emberAfReadServerAttribute(PIR_ENDPOINT, ZCL_IAS_WD_CLUSTER_ID,
                ZCL_MAX_DURATION_ATTRIBUTE_ID,
                (uint8_t*) &pirDelayTime,
                sizeof(pirDelayTime));
    }

    timePointNetworkUp = halCommonGetInt32uMillisecondTick();
    serveZDPMessages = true;
    emberEventControlSetDelayMS(DeviceAnnounceEventControl, 500);
}

/** @brief
 *
 */
void DeviceAnnounceEventFunction(void)
{
    static uint8_t sentNum = 0;
    uint32_t currentTick = 0;

#ifndef DONT_BOOST_RF
    emberSetRadioPower(17);
#endif

    emberEventControlSetInactive(DeviceAnnounceEventControl);

    currentTick = halCommonGetInt32uMillisecondTick();

    if (serveZDPMessages)
    {
        serveZDPMessages = false;
        sentNum = 0;
    }

    emberPollForData();

    sentNum += 1;

    /* Poll 10 times after each time received a ZDP messages within
     * 2 minutes from networkUp */
    if ((sentNum < 10) && (abs(currentTick - timePointNetworkUp) < 120000))
        emberEventControlSetDelayMS(DeviceAnnounceEventControl, 1000);
    else
        emberEventControlSetDelayMS(GetSensorsValueEventControl, TIME_WAIT_PIR_STABLE);
}

/** @brief
 *
 */
void emberIncomingCommandHandler(EmberZigbeeCommandType commandType,
                                 EmberMessageBuffer commandBuffer,
                                 uint8_t indexOfCommand,
                                 void *data)
{
    if (commandType == EMBER_ZIGBEE_COMMAND_TYPE_ZDO)
        serveZDPMessages = true;
}

/** @brief
 *
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{

    /* Check if message is device lock */
    if (lockDevice && incomingMessage->apsFrame->clusterId != ZCL_LM_DEV_OPT_ID)
        return true; // discard all message

    /* if is ZDP message, run stateful principle */
    if (incomingMessage->apsFrame->profileId == EMBER_ZDO_PROFILE_ID)
        serveZDPMessages = true;

#ifdef FOR_ZIPATO_HC
    uint8_t cmdId;
    boolean isClusterSpecific;

    isClusterSpecific = (boolean)(*incomingMessage->message & 0x01);
    cmdId = *(incomingMessage->message + 2);

    if (incomingMessage->apsFrame->clusterId == ZCL_IAS_ZONE_CLUSTER_ID)
    {
        uint16_t attrId;

        attrId = ((uint16_t)*(incomingMessage->message + 4) << 8) + \
                (uint16_t)*(incomingMessage->message + 3);

        // cie write it's ieee address to zone device
        if ((cmdId == 0x02) && !isClusterSpecific && \
                (attrId == ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID))
        {
            uint8_t tempIeee[8];

            for (uint8_t i = 0; i < 8; i++)
                tempIeee[7 - i] = *(incomingMessage->message + i + 6);

            iasServe_ReceiveCieAddrCallback(tempIeee, &motionSensorIas);
        }

        // cie send enroll response
        if (cmdId == 00 && isClusterSpecific)
        {
            iasServe_ReceiveEnrollResponseCallback(*(incomingMessage->message + 3), \
                                                   *(incomingMessage->message + 4), \
                                                   &motionSensorIas);
        }

    }
#endif

    return false;
}

/** @brief
 *
 */
void networkDownCallback(void)
{
    nwkUp = false;
}

/** @brief
 *
 */
void emberAfLmDevOptServerAttributeChangedCallback(int8u endpoint, EmberAfAttributeId attributeId) {
    if (endpoint == 49) {
        emberAfReadServerAttribute(endpoint, ZCL_LM_DEV_OPT_ID,  0x0005, (uint8_t *)&lockDevice, sizeof(lockDevice));
    }
}


