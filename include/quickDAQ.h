#pragma once
#pragma once
#ifndef QUICKDAQ_H
#define QUICKDAQ_H

#ifdef __cplusplus 
extern "C" {
#endif

#include <ansi_c.h>
#include <cLinkedList.h>
#include <NIDAQmx.h>
#include <macrodef.h>
#include <msunistd.h>
#include <stdafx.h>
#include <targetver.h>
#include <stdbool.h>

//-----------------------------
// quickDAQ Macro Declarations
//-----------------------------

#define DAQmxBufSize						15000
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

//DAQmx default sample clock source
#define DAQMX_SAMPLE_CLK_SRC_FINITE		"OnboardClock"
#define DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED	"/PXI1Slot5/ai/SampleClock"

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
		DIGITAL_IN = 2,
		/*! Pin I/O mode: ANALOG OUT*/
		ANALOG_OUT = 1,
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
/*
typedef enum _IO_Direction {
	INPUT	= 0,
	OUTPUT	= 1,
	INOUT	= 2
}IO_Direction;
*/

/*!
* Supported sampling modes
*/
typedef enum _sampling_modes {
	/*! Sampling mode: FINITE - acquire a finite number of samples.*/
	FINITE		= DAQmx_Val_FiniteSamps,
	/*! Sampling mode: HW_CLOCKED - acquire hardware-timed samples, one at a time.*/
	HW_CLOCKED	= DAQmx_Val_HWTimedSinglePoint,
	/*! Sampling mode: CONTINUOUS - continuously acquire samples.*/
	CONTINUOUS	= DAQmx_Val_ContSamps,
	/*! Sampling mode: ON_DEMAND - acquire samples on demand.*/
	ON_DEMAND	= DAQmx_Val_OnDemand
}samplingModes;

/*!
* Supported trigger modes
*/
typedef enum _trigger_modes {
	RISING	= DAQmx_Val_Rising,
	FALLING = DAQmx_Val_Falling
}triggerModes;

/*!
* Defines the details of each NI-DAQmx task
*/
typedef struct _NItask {
	TaskHandle	taskHandler;
	IOmodes		taskType;
	unsigned	pinCount;
	void*		dataBuffer;
} NItask;

/*!
* Defines details on a device pin/channel.
*/
typedef struct _pinInfo {
	bool				isPinValid;
	IOmodes				pinIOMode;
	unsigned int		pinID;
	NItask				*pinTask;
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
	pinInfo				*AIpins;
	NItask				*AItask;
	//unsigned			AItaskDataLen;
	//bool				AItaskEnable;
	
	unsigned int		AOcnt;
	pinInfo				*AOpins;
	NItask				*AOtask;
	//unsigned			AOtaskDataLen;
	//bool				AOtaskEnable;
	
	unsigned int		DIcnt;
	pinInfo				*DIpins;
	NItask				*DItask;
	//unsigned			DItaskDataLen;
	//bool				DItaskEnable;
	
	unsigned int		DOcnt;
	pinInfo				*DOpins;
	NItask				*DOtask;
	//unsigned			DOtaskDataLen;
	//bool				DOtaskEnable;
	
	unsigned int		CIcnt;
	pinInfo				*CIpins;
	NItask				**CItask;
	//unsigned			*CItaskDataLen;
	//bool				*CItaskEnable;
	
	unsigned int		COcnt;
	pinInfo				*COpins;
	NItask				**COtask;
	//unsigned			*COtaskDataLen;
	//bool				*COtaskEnable;
}deviceInfo;

/*!
 * Some default values for NI-DAQmx.
 */
typedef struct _NIdefault_values {
	// Device prefix
	char devPrefix[DAQMX_MAX_DEV_STR_LEN];

	// Analog I/O settings
	int32	NImeasureUnits;
	int32	NIterminalConf;
	
	// Analog input settings
	float64 AImin;
	float64 AImax;

	// Analog output settings
	float64 AOmin;
	float64 AOmax;

	// Digital I/O settings
	int32	NIdigiLineGroup;

	// Counter I/O settings
	int32	NIctrDecodeMode;
	bool32  ZidxEnable;
	float64 ZidxValue;
	int32	ZidxPhase;
	int32	NIctrUnits;
	float64 angleInit;

	// Sampling/Timing properties
	int32	NItriggerEdge;
	float64	NIsamplingRate;
	int32	NIsamplesPerCh;
		// Don't use this setting for analog in. Instead, use -1 (auto)
	int32	NIAIsampsPerCh;
	int32	NIsamplingMode; //DAQmx_Val_HWTimedSinglePoint; //DAQmx_Val_FiniteSamps; // SCR
	char	NIclockSource[DAQMX_MAX_STR_LEN]; // /*DAQMX_SAMPLE_CLK_SRC_FINITE;*/ DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED;// (DAQmxSampleMode == DAQmx_Val_HWTimedSinglePoint) ? DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED : DAQMX_SAMPLE_CLK_SRC_FINITE;

	// Real-time operation output flags
	int32	isSampleLate;
	
	// Encoder properties
	uInt32	encoderPPR; // For CUI AMT103 encoder

	// I/O operation properties
	float64 IOtimeout;
	bool32	DigiAutoStart;
	bool32	AnalogAutoStart; // (NIsamplingMode == DAQmx_Val_HWTimedSinglePoint) ? FALSE : TRUE; //SCR FALSE for HW-timed
	bool32	dataLayout;
		// Don't use this layout for analog input
	bool32	AIdataLayout;
	
	// Miscellaneous stuff that I've lost track of - probably not used. Consider removal.
	int32	plsIdleState;
	int32	plsInitDel;
	int32	plsLoTick;
	int32	plsHiTick;
} NIdefaults;

//------------------------------
// quickDAQ Glabal Declarations
//------------------------------
extern quickDAQErrorCodes		quickDAQErrorCode;
extern int32					NIDAQmxErrorCode;
extern quickDAQStatusModes		quickDAQStatus;
extern bool32					lateSampleWarning;

// NI-DAQmx specific declarations
extern char						DAQmxDevPrefix[DAQMX_MAX_DEV_STR_LEN];
extern unsigned int				DAQmxEnumerated;
//extern long						DAQmxErrorCode;
extern const NIdefaults			DAQmxDefaults;
extern deviceInfo				*DAQmxDevList;
extern unsigned int				DAQmxDevCount;
extern unsigned int				DAQmxMaxCount;
extern int32					DAQmxTriggerEdge;
extern samplingModes			DAQmxSampleMode;
extern char						DAQmxSampleModeString[20];
extern float64					DAQmxSamplingRate;
extern uInt64					DAQmxNumDataPointsPerSample;
extern char						DAQmxClockSource[DAQMX_MAX_STR_LEN];
extern IOmodes					DAQmxClockSourceTask;
extern int						DAQmxClockSourceDev, DAQmxClockSourcePin;

// NI-DAQmx subsystem tasks

extern cLinkedList	*NItaskList;
//extern TaskHandle	*AItaskHandle, *AOtaskHandle, *DItaskHandle, *DOtaskHandle;
//extern unsigned		 AIpinCount, AOpinCount, DIpinCount, DOpinCount;
extern NItask		*AItask	   , *AOtask   , *DItask   , *DOtask;

extern cLinkedList	*CItaskList, *COtaskList;
//extern unsigned		 CIpinCount, COpinCount;


//--------------------------------
// quickDAQ Function Declarations
//--------------------------------
// support functions
void DAQmxErrChk(int32 errCode);
inline char* dev2string(char* strBuf, unsigned int devNum);
char* pin2string(char* strbuf, unsigned int devNum, IOmodes ioMode, unsigned int pinNum);
inline int quickDAQSetError(quickDAQErrorCodes newError, bool printFlag);
inline int quickDAQGetError();
inline int quickDAQSetStatus(quickDAQStatusModes newStatus, bool printFlag);
inline int quickDAQGetStatus();

// library initialization functions
inline char* setDAQmxDevPrefix(char* newPrefix);
void enumerateNIDevices();
unsigned int enumerateNIDevChannels(unsigned int myDev, IOmodes IOtype, unsigned int printFlag);
unsigned int enumerateNIDevTerminals(unsigned int deviceNumber);
void initDevTaskFlags();
void quickDAQinit();

// configuration functions
inline void setActiveEdgeRising();
inline void setActiveEdgeFalling();
void setSampleClockTiming(samplingModes sampleMode, float64 samplingRate, char* triggerSource, triggerModes triggerEdge, uInt64 numDataPointsPerSample, bool printFlag);
bool setClockSource(unsigned devNum, int pinNum, IOmodes ioMode);
void pinMode(unsigned int devNum, IOmodes ioMode, unsigned int pinNum);

// library run functions
void quickDAQstart();
void quickDAQstop();

// read/write functions
typedef struct { unsigned _; } NoArg; // use compound literal to form a dummy value for _Generic, only its type matters
#define NO_ARG ((const NoArg){0})
	// Function calls that write to/read either from external buffers or internal buffers
void readAnalog_extBuf(unsigned devNum, float64 *outputData);
void readAnalog_intBuf(unsigned devNum);
#define readAnalog_(args, a, b, ...)	\
  _Generic((b),							\
           NoArg:	readAnalog_intBuf,	\
           default: readAnalog_extBuf	\
          )args
// pass copy of args as the first argument
// add NO_ARG value, only its type matters
// add dummy `~` argument to ensure that `...` in `foo_` catches something
#define readAnalog(...) readAnalog_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~)
inline float64 getAnalogInPin(unsigned devNum, unsigned pinNum);

void writeAnalog_extBuf(unsigned devNum, float64 *inputData);
void writeAnalog_intBuf(unsigned devNum);
#define writeAnalog_(args, a, b, ...)	\
  _Generic((b),							\
           NoArg:	writeAnalog_intBuf,	\
           default: writeAnalog_extBuf	\
          )args
#define writeAnalog(...) writeAnalog_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~)
void setAnalogOutPin(unsigned devNum, unsigned pinNum, float64 pinValue);

void writeDigital_extBuf(unsigned devNum, uInt32 *inputData);
void writeDigital_intBuf(unsigned devNum);
#define writeDigitalPort_(args, a, b, ...)	\
  _Generic((b),							\
           NoArg:	writeDigital_intBuf,	\
           default: writeDigital_extBuf	\
          )args
#define writeDigital(...) writeDigitalPort_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~)
void setDigitalOutPort(unsigned devNum, unsigned portNum, uInt32 portValue);

void writeDigitalPin(unsigned devNum, unsigned portNum, unsigned pinNum, bool bitState);
void setDigitalOutPin(unsigned devNum, unsigned portNum, unsigned pinNum, bool bitState);

void readCounterAngle_extBuf(unsigned devNum, unsigned ctrNum, float64 *outputData);
void readCounterAngle_intBuf(unsigned devNum, unsigned ctrNum);
#define readCounterAngle_(args, a, b, c, ...)	\
  _Generic((c),							\
           NoArg:	writeDigitalPin_intBuf,	\
           default: writeDigitalPin_extBuf	\
          )args
#define readCounterAngle(...) readCounterAngle_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~)
float64 getCounterAngle(unsigned devNum, unsigned ctrNum);

void syncSampling();

// shutdown routines
int quickDAQTerminate();

#ifdef __cplusplus 
}
#endif 

#endif // !QUICKDAQ_H


