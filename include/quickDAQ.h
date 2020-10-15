#pragma once
#pragma once
#ifndef QUICKDAQ_H
#define QUICKDAQ_H

#include <ansi_c.h>
#include <cLinkedList.h>
#include <NIDAQmx.h>
#include <macrodef.h>
#include <msunistd.h>
#include <stdafx.h>
#include <targetver.h>

//-----------------------------
// quickDAQ Macro Declarations
//-----------------------------

// Macros defined to handle any errors that may be thrown
#define quickDAQErrChk(functionCall) if( functionCall < 0) goto quickDAQErr; else
#define NIDAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//NI-DAQmx max string size
#define DAQMX_MAX_STR_LEN			255

//DAQmx device constants
#define DAQMX_MAX_DEV_CNT			14
#define DAQMX_MAX_DEV_PREFIX_LEN	14
#define DAQMX_DEF_DEV_PREFIX		"PXI1Slot"
#define DAQMX_MAX_DEV_STR_LEN		DAQMX_MAX_DEV_PREFIX_LEN + 2 + 1

//DAQmx pin constants
#define DAQMX_MAX_PIN_CNT			32
#define DAQMX_MAX_PIN_STR_LEN		16 + 1

//-----------------------
// quickDAQ TypeDef List
//-----------------------

/*!
* Enumerates the list of possible status modes of the quickDAQ library as set in 'quickDAQStatus'.
*/
typedef enum _quickDAQStatusModes {
	/*! Indicates that quickDAQ has entered an unknown mode*/
	STATUS_UNKNOWN = -99,
	/*! Indicates that quickDAQ is nascent and uninitialized.*/
	STATUS_NASCENT = -2,
	/*! Indicates that quickDAQ has been initialized and ready to configure with active I/O.*/
	STATUS_INIT = -1,
	/*! Indicates that the quickDAQ is ready to run. I/O, sync, clock, and trigger resources reserved.*/
	STATUS_READY = 0,
	/*! Indicated that quickDAQ is running and data is being collected now.*/
	STATUS_RUNNING = 1,
	/*!	Indicates that quickDAQ has been stopped and resources freed.*/
	STATUS_SHUTDOWN = 2
}quickDAQStatusModes;

/*!
 * Enumerates the list of error codes of the quickDAQ library as set in 'quickDAQError'.
 */
typedef enum _quickDAQErrorCodes {
	/*! Library has encountered an unknown error*/
	ERROR_UNKNOWN = -99,
	/*! Library is not ready to run. Configure library, sample clock and pin mode first!*/
	ERROR_NOTREADY = -7,
	/*! A feature or functionality that is unsupported by quickDAQ requested.*/
	ERROR_UNSUPPORTED = -6,
	/*!	An invalid or unsupported I/O type has been selected.*/
	ERROR_INVIO = -5,
	/*! NI-DAQmx has generated an error. Need to check 'NIDAQmxErrorCode' for details.*/
	ERROR_NIDAQMX = -4,
	/*! List of NI-DAQmx devices detected by quickDAQ library has changed.*/
	ERROR_DEVCHANGE = -3,
	/*! No NI-DAQmx devices detected by quickDAQ library.*/
	ERROR_NODEVICES = -2,
	/*! Pin and task configuration may be altered only in the preconfigure state.*/
	ERROR_NOTCONFIG = -1,
	/*! No error has occured.*/
	ERROR_NONE = 0,
}quickDAQErrorCodes;


/*!
 * Defines the types of I/O modes suported by this library.
 */
typedef enum _IOmodes {
	// Invalid I/O mode
		/*! Pin I/O mode: INVALID IO mode*/
	INVALID_IO = 32767,

	// pin I/O modes
		/*! Pin I/O mode: ANALOG IN*/
		ANALOG_IN = 0,
		/*! Pin I/O mode: DIGITAL IN*/
		DIGITAL_IN = 1,
		/*! Pin I/O mode: ANALOG OUT*/
		ANALOG_OUT = 2,
		/*! Pin I/O mode: DIGITAL OUT*/
		DIGITAL_OUT = 3,

		// counter I/O modes
			/*! Counter I/O mode: COUNTER IN - reads angular position*/
			CTR_ANGLE_IN = 4,
			/*! Counter I/O mode: COUNTER OUT - writes ticks*/
			CTR_TICK_OUT = 5
}IOmodes;

/*!
 * Possible I/O directions - Input and Output, defined for easy programming.
 */
typedef enum _IO_Direction {
	INPUT = 0,
	OUTPUT = 1,
	INOUT = 2
}IO_Direction;

/*!
* Defines details on a device pin/channel.
*/
typedef struct _pinInfo {
	bool				isPinValid;
	unsigned int		pinNum;
	IOmodes				pinIOMode;
	IO_Direction		pinDir;
	TaskHandle			*pinTask;
}pinInfo;

/*!
 * Defined details of each device enumerated.
*/
typedef struct _deviceInfo {
	bool				isDevValid;
	unsigned int		devNum;
	unsigned long		devSerial;
	char				devName[20];
	char				devType[20];
	bool				isDevSimulated;
	long				devError;

	// Device I/O counts and their respective 'pinInfo'.
	unsigned int		AIcnt;
	pinInfo				AIpins[DAQMX_MAX_PIN_CNT];
	unsigned int		AOcnt;
	pinInfo				AOpins[DAQMX_MAX_PIN_CNT];
	unsigned int		DIcnt;
	pinInfo				DIpins[DAQMX_MAX_PIN_CNT];
	unsigned int		DOcnt;
	pinInfo				DOpins[DAQMX_MAX_PIN_CNT];
	unsigned int		CIcnt;
	pinInfo				CIpins[DAQMX_MAX_PIN_CNT];
	unsigned int		COcnt;
	pinInfo				COpins[DAQMX_MAX_PIN_CNT];
}deviceInfo;

/*!
 * Some default values for NI-DAQmx.
 */
typedef struct _NIdefaults {
	// Analog I/O settings
	int32	NImeasureUnits	= DAQmx_Val_Volts;
	int32	NIterminalConf	= DAQmx_Val_RSE;
	
	// Analog input settings
	float64 AImin			= -10;
	float64 AImax			= 10;

	// Analog output settings
	float64 AOmin			= -10;
	float64 AOmax			= 10;

	// Digital I/O settings
	int32	NIdigiLineGroup = DAQmx_Val_ChanForAllLines;

	// Counter I/O settings
	int32	NIctrDecodeMode = DAQmx_Val_X4;
	bool32  ZidxEnable		= 0;
	float64 ZidxValue		= 0.0;
	int32	ZidxPhase		= DAQmx_Val_AHighBHigh;
	int32	NIctrUnits		= DAQmx_Val_Degrees;
	float64 angleInit		= 0.0;

	// Sampling/Timing properties
	float64	NIsamplingRate = 1000.0;
	int32	NIsamplesPerCh = 1;
		// Don't use this setting for analog in. Instead, use -1 (auto)
	int32	NIsamplingMode = DAQmx_Val_FiniteSamps;

	// Real-time operation output flags
	int32	isSampleLate	= 0;
	
	// Encoder properties
	uInt32	encoderPPR		= 2048; // For CUI AMT103 encoder

	// I/O operation properties
	float64 IOtimeout		= 10.0;
	bool32	DigiAutoStart	= TRUE;
	bool32	AnalogAutoStart = FALSE;
	bool32	dataLayout		= DAQmx_Val_GroupByChannel;
		// Don't use this layout for analog input
	
	// Miscellaneous stuff that I've lost track of - probably not used. Consider removal.
	int32	plsIdleState	= DAQmx_Val_Low;
	int32	plsInitDel		= 0;
	int32	plsLoTick		= 1;
	int32	plsHiTick		= 1;
}NIdefaults;

//------------------------------
// quickDAQ Glabal Declarations
//------------------------------
extern quickDAQErrorCodes		quickDAQErrorCode;
extern long						NIDAQmxErrorCode;
extern quickDAQStatusModes		quickDAQStatus;

// NI-DAQmx specific declarations
extern char						DAQmxDevPrefix[DAQMX_MAX_DEV_STR_LEN];
extern unsigned int				DAQmxEnumerated;
extern long						DAQmxErrorCode;
extern NIdefaults				DAQmxDefaults;
extern deviceInfo				*DAQmxDevList;
extern unsigned int				DAQmxDevCount;
extern unsigned int				DAQmxMaxCount;

//--------------------------------
// quickDAQ Function Declarations
//--------------------------------
// support functions
inline char* dev2string(char* strBuf, unsigned int devNum);
char* pin2string(char* strbuf, unsigned int devNum, IOmodes ioMode, unsigned int pinNum);
inline int quickDAQSetError(quickDAQErrorCodes newError, bool printFlag);

// library initialization functions
inline char* setDAQmxDevPrefix(char* newPrefix);
void enumerateNIDevices();
unsigned int enumerateNIDevChannels(unsigned int myDev, IOmodes IOtype, unsigned int printFlag);
unsigned int enumerateNIDevTerminals(unsigned int deviceNumber);


// configuration functions

// library run functions

// shutdown routines
int quickDAQTerminate();

#endif // !QUICKDAQ_H


