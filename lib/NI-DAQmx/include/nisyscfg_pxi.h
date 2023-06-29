/*============================================================================*/
/*             National Instruments / System Configuration API                */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2010-2019.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:   nisyscfg_pxi.h                                                    */
/* Purpose: Include file for PXI specific attributes to be used with the      */
/*          nisyscfg interface defined in nisyscfg.h                          */
/*                                                                            */
/*============================================================================*/

#ifndef ___nisyscfg_pxi_h___
#define ___nisyscfg_pxi_h___

#include <nisyscfg.h>

/* Declares the possible values of NISysCfgPxiPropertyClk10Source */
typedef enum
{
   NISysCfgPxiClk10SrcUnknown             = -1, /* Not applicable, or not software-readable */
   NISysCfgPxiClk10SrcInternal            = 0,  /* Internal Oscillator */
   NISysCfgPxiClk10SrcBuiltInConnector    = 1,  /* Supplied by the dedicated 10 MHz REF IN connector (e.g. SMA or SMB) */
   NISysCfgPxiClk10SrcTimingModule        = 2,  /* System Timing Module */
   NISysCfgPxiClk10SrcTrig10MHzPort0      = 3,  /* TRIG / 10 MHz Port 0 / REF IN */
} NISysCfgPxiClock10Sources;

/* Declares the possible values of NISysCfgPxiPropertyExternalClockOutputSource */
typedef enum
{
   NISysCfgPxiExternalClockOutputSourceUnknown            = -1, /* Not applicable, or not software-readable */
   NISysCfgPxiExternalClockOutputSourcePXI_CLK10          = 0,  /* PXI_CLK10 from the chassis backplane */
   NISysCfgPxiExternalClockOutputSourceInternalOscillator = 1,  /* Internal Oscillator */
} NISysCfgPxiExternalClockOutputSource;

/* Declares the possible values of NISysCfgPxiPropertyInternalOscillator */
typedef enum
{
   NISysCfgPxiInternalOscillatorUnsupported = -1, /* Not software-readable */
   NISysCfgPxiInternalOscillatorVCXO        = 0,  /* Voltage-controlled Oscillator */
   NISysCfgPxiInternalOscillatorOCXO        = 1,  /* Oven-controlled Oscillator */
} NISysCfgPxiInternalOscillator;

/* Declares the possible values of NISysCfgPxiPropertyFanMode */
typedef enum
{
   NISysCfgPxiFanModesAuto       = 1, /* Default operating mode */
   NISysCfgPxiFanModesSafeManual = 2, /* Allows caller to manipulate the fan
                                         speed within safe boundaries by
                                         setting NISysCfgPxiPropertyFanUserRpm */
   NISysCfgPxiFanModesHigh       = 4, /* Fans run at the maximum speed for the
                                         current cooling profile */
} NISysCfgPxiFanModes;

/* Declares the possible values of NISysCfgPxiPropertyCoolingProfiles */
typedef enum
{
   NISysCfgPxiCoolingProfile38W  = 1, /* Default operating mode */
   NISysCfgPxiCoolingProfile58W  = 2, /* More aggressive cooling profile for
                                         cooling modules requiring 58W or
                                         less of cooling */
   NISysCfgPxiCoolingProfile82W  = 4, /* More aggressive cooling profile for
                                         cooling modules requiring 82W or
                                         less of cooling */
} NISysCfgPxiCoolingProfiles;

/* Declares the possible values of NISysCfgPxiIndexedPropertyPowerSupplyState */
typedef enum
{
   NISysCfgPxiPowerSupplyStateOff                                  = 0,
   NISysCfgPxiPowerSupplyStateOn                                   = 1,
   NISysCfgPxiPowerSupplyStateFaulted                              = 2,
   NISysCfgPxiPowerSupplyStateFault_OutputVoltageOvervoltage12V    = 16,
   NISysCfgPxiPowerSupplyStateFault_OutputVoltageUndervoltage12V   = 17,
   NISysCfgPxiPowerSupplyStateFault_OutputVoltageOvervoltage5VAux  = 18,
   NISysCfgPxiPowerSupplyStateFault_OutputVoltageUndervoltage5VAux = 19,
   NISysCfgPxiPowerSupplyStateFault_OutputCurrentOvercurrent12V    = 20,
   NISysCfgPxiPowerSupplyStateFault_OutputCurrentOvercurrent5VAux  = 21,
   NISysCfgPxiPowerSupplyStateFault_InputVoltageOvervoltage        = 22,
   NISysCfgPxiPowerSupplyStateFault_InputVoltageUndervoltage       = 23,
   NISysCfgPxiPowerSupplyStateFault_LowerAmbientTemperature        = 24,
   NISysCfgPxiPowerSupplyStateFault_UpperAmbientTemperature        = 25,
   NISysCfgPxiPowerSupplyStateFault_LowerInternalTemperature       = 26,
   NISysCfgPxiPowerSupplyStateFault_UpperInternalTemperature       = 27,
   NISysCfgPxiPowerSupplyStateFault_Fan                            = 28,
   NISysCfgPxiPowerSupplyStateAlert_OutputVoltageOvervoltage12V    = 48,
   NISysCfgPxiPowerSupplyStateAlert_OutputVoltageUndervoltage12V   = 49,
   NISysCfgPxiPowerSupplyStateAlert_OutputVoltageOvervoltage5VAux  = 50,
   NISysCfgPxiPowerSupplyStateAlert_OutputVoltageUndervoltage5VAux = 51,
   NISysCfgPxiPowerSupplyStateAlert_OutputCurrentOvercurrent12V    = 52,
   NISysCfgPxiPowerSupplyStateAlert_OutputCurrentOvercurrent5VAux  = 53,
   NISysCfgPxiPowerSupplyStateAlert_OutputCurrentSharing           = 54,
   NISysCfgPxiPowerSupplyStateAlert_InputVoltageOvervoltage        = 55,
   NISysCfgPxiPowerSupplyStateAlert_InputVoltageUndervoltage       = 56,
   NISysCfgPxiPowerSupplyStateAlert_LowerAmbientTemperature        = 57,
   NISysCfgPxiPowerSupplyStateAlert_UpperAmbientTemperature        = 58,
   NISysCfgPxiPowerSupplyStateAlert_LowerInternalTemperature       = 59,
   NISysCfgPxiPowerSupplyStateAlert_UpperInternalTemperature       = 60,
   NISysCfgPxiPowerSupplyStateAlert_Fan                            = 61,
} NISysCfgPxiPowerSupplyState;

/* Declares the possible values of NISysCfgPxiInhibitModes */
typedef enum
{
  NISysCfgPxiInhibitDefault = 1, /* Chassis power controlled by the power button and OS */
  NISysCfgPxiInhibitManual  = 2, /* Chassis power controlled by the Remote Inhibit signal */
} NISysCfgPxiInhibitModes;

/* Declares the possible values of NISysCfgPxiCalExtActions */
typedef enum
{
   NISysCfgPxiCalExtCancel    = 0,
   NISysCfgPxiCalExtOcxoStart = 1,
   NISysCfgPxiCalExtCommit    = 2,
} NISysCfgPxiCalExtActions;

typedef enum
{
   /* Chassis attributes */
   NISysCfgPxiPropertyPxiChassisNumber               = 184565760,  /* unsigned int */

   /* Clock attributes */
   NISysCfgPxiPropertyClk10Source                    = 184635392,  /* NISysCfgPxiClock10Sources */
   /* Sets the source for external CLK10 Reference Clock outputs on the
      chassis. The default is PXI_CLK10 from the backplane, which itself may
      be sourced from various inputs. To achieve minimal phase offset of the
      PXI_CLK10 reference clock between multiple chassis, set this attribute
      to NISysCfgPxiExternalClockOutputSourceInternalOscillator, then split
      the signal from the 10 MHz REF OUT connector using matched-length
      cabling to the 10 Mhz REF IN connector on each chassis requiring minimal
      phase offset. For best results, use the same model of chassis. */
   NISysCfgPxiPropertyExternalClockOutputSource      = 184639488,  /* NISysCfgPxiExternalClockOutputSource */
   NISysCfgPxiPropertyInternalOscillator             = 184643584,  /* NISysCfgPxiInternalOscillator */

   /* Fan control attributes */
   NISysCfgPxiPropertyFanMode                        = 185597952,  /* NISysCfgPxiFanModes */
   NISysCfgPxiPropertyFanUserRpm                     = 185602048,  /* unsigned int */
   NISysCfgPxiPropertySupportedFanModes              = 185606144,  /* bitmask of NISysCfgPxiFanModes */
   NISysCfgPxiPropertyFanManualRpmLowerBound         = 185634816,  /* unsigned int */
   NISysCfgPxiPropertyFanManualRpmUpperBound         = 185638912,  /* unsigned int */
   NISysCfgPxiPropertyCoolingProfile                 = 185610240,  /* NISysCfgPxiCoolingProfiles */
   NISysCfgPxiPropertySupportedCoolingProfiles       = 185614336,  /* bitmask of NISysCfgPxiCoolingProfiles */

   /* Power supply attributes */
   NISysCfgPxiPropertyPowerSupplyBayCount            = 186777600,  /* int */
   NISysCfgPxiPropertyPowerSuppliesRedundant         = 186798080,  /* unsigned int */
   NISysCfgPxiPropertyInhibitMode                    = 186806272,  /* NISysCfgPxiInhibitModes */
   NISysCfgPxiPropertySupportedInhibitModes          = 186810368,  /* bitmask of NISysCfgPxiInhibitModes */

   /* Calibration attributes */
   NISysCfgPxiPropertyCalExtAction                   = 186908672,  /* NISysCfgPxiCalExtActions */
   NISysCfgPxiPropertyCalExtDacValue                 = 186925056,  /* unsigned int */

} NISysCfgPxiProperty;

typedef enum
{
   /* Power supply index attributes */
   NISysCfgPxiIndexedPropertyPowerSupplyName               = 186781696,  /* char * */
   NISysCfgPxiIndexedPropertyPowerSupplyState              = 186789888,  /* NISysCfgPxiPowerSupplyState */
   NISysCfgPxiIndexedPropertyPowerSupplyPower              = 186793984,  /* unsigned int (Watts) */
   NISysCfgPxiIndexedPropertyPowerSupplyPowerReading       = 186822656,  /* double (Watts) */
   NISysCfgPxiIndexedPropertyPowerSupplyIntakeTemp         = 186802176,  /* double (degrees Celsius) */
   NISysCfgPxiIndexedPropertyPowerSupplyPowerLineFrequency = 186785792,  /* unsigned int (Hertz) */

} NISysCfgPxiIndexedProperty;

#endif
