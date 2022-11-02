/// See LICENSE.md at root of repository for copyright information
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


/**
 * @copyright copyright description
 * @author Suraj Chakravarthi Raja
 * @brief quickDAQ global header.
 * 
 * This file includes all of the macros and function declarations that are used in the quickDAQ library.
 * It also includes the descriptions of each function, input parameters and their outputs; along with expected useages.
 * 
 */

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

/**
 * @brief Possible status modes of the quickDAQ library.
 * 
 * Used to set the status of the quickDAQ library in quickDAQSetStatus().
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

/**
 * @brief Error status codes of the quickDAQ library.
 * 
 * Used in setting error status in quickDAQSetError().
 */
typedef enum _quickDAQErrorCodes {
	/** Library has encountered an unknown error*/
	ERROR_UNKNOWN = -99,
	/** Library is not ready to run. Configure library, sample clock and pin mode first!*/
	ERROR_NOTREADY = -7,
	/** A feature or functionality that is unsupported by quickDAQ requested.*/
	ERROR_UNSUPPORTED = -6,
	/**	An invalid or unsupported I/O type has been selected.*/
	ERROR_INVIO = -5,
	/** NI-DAQmx has generated an error. Need to check 'NIDAQmxErrorCode' for details.*/
	ERROR_NIDAQMX = -4,
	/** List of NI-DAQmx devices detected by quickDAQ library has changed.*/
	ERROR_DEVCHANGE = -3,
	/** No NI-DAQmx devices detected by quickDAQ library.*/
	ERROR_NODEVICES = -2,
	/** Pin and task configuration may be altered only in the preconfigure state.*/
	ERROR_NOTCONFIG = -1,
	/** No error has occured.*/
	ERROR_NONE = 0,
}quickDAQErrorCodes;


/**
 * @brief Defines the types of I/O modes suported by this library.
 */
typedef enum _IOmodes {
	// Invalid I/O mode
		/** Pin I/O mode: INVALID IO mode*/
	INVALID_IO = 32767,

	// pin I/O modes
		/** Pin I/O mode: ANALOG IN*/
		ANALOG_IN = 0,
		/** Pin I/O mode: DIGITAL IN*/
		DIGITAL_IN = 2,
		/** Pin I/O mode: ANALOG OUT*/
		ANALOG_OUT = 1,
		/** Pin I/O mode: DIGITAL OUT*/
		DIGITAL_OUT = 3,

		// counter I/O modes
			/** Counter I/O mode: COUNTER IN - reads angular position*/
			CTR_ANGLE_IN = 4,
			/** Counter I/O mode: COUNTER OUT - writes ticks*/
			CTR_TICK_OUT = 5
}IOmodes;

/**
 * @brief All possible I/O directions
 * 
 * Input, Output, and INOUT (Input / Output) defined for easy programming.
 */

typedef enum _IO_Direction {
	INPUT	= 0,
	OUTPUT	= 1,
	INOUT	= 2
}IO_Direction;


/**
* @brief Supported sampling modes
* 
* Currently linked to NI DAQ MX.
* @todo Make generic enough to be used with the alternative myoSyn DAQ solution that is coming in.
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

/**
* @brief Supported trigger modes
* Definition linked to NI DAQ MX.
* @todo - Same as samplingModes enum, make this enum generic enough to be used with an alternative system.
*/
typedef enum _trigger_modes {
	RISING	= DAQmx_Val_Rising, ///< Triggers on rising edge (0->1)
	FALLING = DAQmx_Val_Falling ///< Triggers on falling edge (1->0)
}triggerModes;

/**
* @brief Defines the details of each NI-DAQmx task
* @note Only to be used with NI-DAQmx backend.
*/
typedef struct _NItask {
	TaskHandle	taskHandler;
	IOmodes		taskType;
	unsigned	pinCount;
	void*		dataBuffer;
} NItask;

/**
* @brief Defines details on a device pin/channel.
*/
typedef struct _pinInfo {
	bool				isPinValid;
	IOmodes				pinIOMode;
	unsigned int		pinID;
	NItask				*pinTask;
}pinInfo;

/**
 * @brief Defined details of each device enumerated.
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
	unsigned int		AIcnt;         //< Number of analog inputs
	pinInfo				*AIpins;       //< Gives details of each analog input pin. 
	NItask				*AItask;       //< NI-DAQmx task associated with each analog input pin.
	//unsigned			AItaskDataLen;
	//bool				AItaskEnable;
	
	unsigned int		AOcnt;         ///< Number of analog outputs
	pinInfo				*AOpins;       ///< Gives details of each analog output pin. 
	NItask				*AOtask;       ///< NI-DAQmx task associated with each analog output pin.
	//unsigned			AOtaskDataLen;
	//bool				AOtaskEnable;
	
	unsigned int		DIcnt;         ///< Number of digital inputs
	pinInfo				*DIpins;       ///< Gives details of each digital input pin.
	NItask				*DItask;       ///< NI-DAQmx task associated with each digital input pin.
	//unsigned			DItaskDataLen;
	//bool				DItaskEnable;
	
	unsigned int		DOcnt;         ///< Number of digital outputs
	pinInfo				*DOpins;       ///< Gives details of each digital output pin.
	NItask				*DOtask;       ///< NI-DAQmx task associated with each digital output pin.
	//unsigned			DOtaskDataLen;
	//bool				DOtaskEnable;
	
	unsigned int		CIcnt;         ///< Number of counter inputs - used for reading angular position
	pinInfo				*CIpins;       ///< Gives details of each counter input pin.
	NItask				**CItask;      ///< NI-DAQmx task associated with each digital output pin.
	//unsigned			*CItaskDataLen;
	//bool				*CItaskEnable;
	
	unsigned int		COcnt;         ///< Number of counter outputs - used for writing ticks @todo what are ticks
	pinInfo				*COpins;       ///< Gives details of each counter output pin.
	NItask				**COtask;      ///< NI-DAQmx task associated with each digital output pin.
	//unsigned			*COtaskDataLen;
	//bool				*COtaskEnable;
}deviceInfo;

/**
 * @brief Structure to hold default values for NI-DAQmx.
 * 
 * These values are defined in quickDAQ.c
 * @see quickDAQ.c - DAQmxDefaults
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

	// Pulse Channel properties, most likely
	int32	plsIdleState; // Speficies resting state of output terminal, can either be set to DAQmx_Val_High or DAQmx_Val_Low
	int32	plsInitDel;
	int32	plsLoTick;
	int32	plsHiTick;
} NIdefaults;

//------------------------------
// quickDAQ Global Declarations
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
/**
 * @brief Checks returned error code for failure. Will terminate if this condition is met (error code > 0)
 * 
 * @param errCode - Error code returned by various DAQmx functions
 * 
 * @example @code DAQmxErrChk(DAQmxStartTask(taskHandle)) @endcode - Will attempt to start handle, and die if it is unable to do so.
 */
void DAQmxErrChk(int32 errCode);

/**
 * @brief Converts a device number (devNum) into a string stored that is pointed to in strBuf.
 * 
 * This does the following operation:
 * Concatenates PXI1Slot and the devNum to create a unique string per device.
 * For example, if your devNum = 4, then the string output will be: PXI1Slot4
 * 
 * @param strBuf pointer to the buffer that will be used to store the unique string identifier.
 * @param devNum device number as understood by NI-DAQmx
 * @return char* - strBuf, returns the same pointer that was passed in. Done for API flexibility.
 */
inline char* dev2string(char* strBuf, unsigned int devNum);
/**
 * @brief Outputs a string based on the device number, IO mode, and pin number.
 * 
 * Performs the following concatenation:
 * PXI1Slot+devNum+'@\'+pinType+pinNum
 *
 * For example, devNum = 4, pinType = "AI" (Analog Input), pinNum = 6 will give us:
 * PXI1Slot4\AI6
 * 
 * @param strbuf Pointer to buffer that will hold the unique string identifier/
 * @param devNum Device number as understood by NI-DAQmx.
 * @param ioMode @see IOmodes, assigned IO mode for this device / pin combo
 * @param pinNum pin number.
 * @return char* returns strbuf, which is the same pointer that was passed in. Done for API flexibility.
 */
char* pin2string(char* strbuf, unsigned int devNum, IOmodes ioMode, unsigned int pinNum);

/**
 * @brief Sets the global error variable quickDAQErrorCode, and can print a status message to STDERR
 * 
 * @param newError - Error code that needs to be handled. @see quickDAQErrorCodes enum
 * @param printFlag controls whether or not the error code is printed to STDERR
 * @return int Value of newError (which mirrors the global quickDAQErrorCode)
 */
int quickDAQSetError(quickDAQErrorCodes newError, bool printFlag);

/**
 * @brief Gets the value of global variable quickDAQErrorCode.
 * 
 * @return int the value of quickDAQErrorCode.
 */
inline int quickDAQGetError();

/**
 * @brief Sets the global status variable quickDAQStatus, using the code in newStatus.
 * Able to print status to STDERR.
 * 
 * @param newStatus Status code of the current state. @see quickDAQStatusModes
 * @param printFlag controls whether or not the current state is printed to STDERR
 * @return int Value of newStatus (which mirrors the global quickDAQStatus)
 */
int quickDAQSetStatus(quickDAQStatusModes newStatus, bool printFlag);

// library initialization functions
/**
 * @brief Sets the device previx string
 * 
 * @param newPrefix pointer to a buffer that holds the new prefix string we would like to use.
 * @return char* The same pointer that we passed in, used for API flexibility.
 */
char* setDAQmxDevPrefix(char* newPrefix);

/**
 * @brief Gets a list of all the NI Devices connected to the system that
 * are visible to NI-DAQmx.
 */
void enumerateNIDevices();

/**
 * @brief Gets the number of channels that are available for a given device & IO mode.
 *
 * Returns the number of physical channels of a particular I/O type available in a specified device.
 * A printFlag also allows users to optionally have the function print the list of available channels.
 *
 * \param myDev Device ID numver of the NI-DAQmx device as specified in QuickDAQ
 * \param IOtype The supported 'IOmode' I/O types for which number of channels must be enumerated.
 * \param printFlag Set to 1 to print (to STDERR) the list of channels of the specified I/O types available with the device.
 * \return Returns the number of physical channels of the specified I/O type that is available in the device.
 */
unsigned int enumerateNIDevChannels(unsigned int myDev, IOmodes IOtype, unsigned int printFlag);
/**
 * @brief Detects, enumerates and prints all the terminals on a specified DAQmx device.
 *
 * @param deviceNumber 
 * @return unsigned int Returns the total number of terminals on the device.
 */
unsigned int enumerateNIDevTerminals(unsigned int deviceNumber);

/**
 * @brief Initializes all the NI Tasks.
 * 
 * Initializes tasks to either a NULL reference or to an empty buffer that is typecast to
 * the counter I/O task type.
 */
void initDevTaskFlags();

/**
 * @brief Initialization routine for the quickDAQ Library.
 * 
 * Need to call this before anything else!!
 */
void quickDAQinit();

// configuration functions
/**
 * @brief Sets the system to trigger on the rising edge.
 * 
 * Sets the global variable DAQmxTriggerEdge to DAQmx_Val_Rising.
 */
void setActiveEdgeRising();

/**
 * @brief Sets the system to trigger on the rising edge.
 * 
 * Sets the global variable DAQmxTriggerEdge to DAQmx_Val_Falling.
 */
void setActiveEdgeFalling();

/**
 * @brief Set the Sample Clock Timing mode.
 * 
 * Sets the sample clock timing mode for the system, with the parameters specified.
 * 
 * @param sampleMode @see samplingModes, sets the sampling mode.
 * @param samplingRate sets the sampling rate in Hertz (Hz).
 * @param triggerSource pointer to a buffer which contains a string, naming the trigger source. @see quickDAQ_example.cpp
 * @param triggerEdge @see triggerModes The trigger mode, rising edge or falling edge
 * @param numDataPointsPerSample The number of samples to acquire or generate for each channel in the task if sampleMode is FINITE. If sampleMode is CONTINUOUS, NI-DAQmx uses this value to determine the buffer size.
 * @param printFlag controls if we print the error status codes to STDERR
 */
void setSampleClockTiming(samplingModes sampleMode, float64 samplingRate, char* triggerSource, triggerModes triggerEdge, uInt64 numDataPointsPerSample, bool printFlag);

/**
 * @brief Set the Clock Source as a unique combination of deviceNum+pinNum+ioMode.
 * 
 * By default, quickDAQ will set the clock source to what we have defined in the global variable DAQmxClockSource on startup.
 * Sets the global variable DAQmxClockSource to the unique string that we are using as the clock input signal.
 * 
 * @param devNum Device number as known by NI-DAQmx
 * @param pinNum The pin number on the device specified.
 * @param ioMode The IO mode of that particular pin, @see IOmodes
 * @return true Successfully set the clock source to desired.
 * @return false Failed to setup the new clock source.
 */
bool setClockSource(unsigned devNum, int pinNum, IOmodes ioMode);

/**
 * @brief Arduino compatible definition for pin mode - specifies IO mode for a particular pin.
 * 
 * @param devNum Device number as known by NI-DAQmx
 * @param ioMode The IO mode we wish to set
 * @param pinNum The pin we wish to use for the desired IO mode.
 */
void pinMode(unsigned int devNum, IOmodes ioMode, unsigned int pinNum);

// library run functions

/**
 * @brief Starts the NI-DAQmx task handler and kicks off data acquisition.
 * 
 * Reads into the linked list of tasks in NItaskList and performs the specified task initialization.
 * This will depend on how you have configured the system and tasks. @see quickDAQ_example.cpp
 */
void quickDAQstart();

/**
 * @brief Performs the necessary tasks to end the NI-DAQmx task handler safely.
 * 
 */
void quickDAQstop();

// read/write functions
typedef struct { unsigned _; } NoArg; // use compound literal to form a dummy value for _Generic, only its type matters
#define NO_ARG ((const NoArg){0})

// Function calls that write to/read either from external buffers or internal buffers
/**
 * @brief This function will read analog data from the specified device number.
 * 
 * This function will store data into the specified buffer, given by the pointer.
 * 
 * @param devNum - Device number we wish to read from. 
 * @param outputData - pointer to buffer for output
 */
void readAnalog_extBuf(unsigned devNum, float64 *outputData);
/**
 * @brief This function reads analog data and stores it into an internal buffer.
 * 
 * @param devNum Device number we wish to read from.
 */
void readAnalog_intBuf(unsigned devNum);
#define readAnalog_(args, a, b, ...)	\
  _Generic((b),							\
           NoArg:	readAnalog_intBuf,	\
           default: readAnalog_extBuf	\
          )args
// pass copy of args as the first argument
// add NO_ARG value, only its type matters
// add dummy `~` argument to ensure that `...` in `foo_` catches something
/**
 * @fn readAnalog(deviceNumber) - reads from device number and stores result into internal buffer.
 * @fn readAnalog(deviceNumber, &(float64 *)buf) - reads from device number and stores result into external buffer at buf.
 */
#define readAnalog(...) readAnalog_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~) ///< Reads from device number (analog) and stores result into internal buffer if only devNum is specified or external buffer if devNum and a pointer to float64 is provided.
/**
 * @brief Gets the float64 measurement from the specified input.
 * 
 * @param devNum device to read from
 * @param pinNum pin to read from on the device
 * @return float64 value that we just read.
 */
float64 getAnalogInPin(unsigned devNum, unsigned pinNum);

void writeAnalog_extBuf(unsigned devNum, float64 *inputData);
void writeAnalog_intBuf(unsigned devNum);
#define writeAnalog_(args, a, b, ...)	\
  _Generic((b),							\
           NoArg:	writeAnalog_intBuf,	\
           default: writeAnalog_extBuf	\
          )args
#define writeAnalog(...) writeAnalog_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~) ///< Writes to device number (analog) from internal buffer if only devNum is specified or from external buffer if devNum and a pointer to float64 is provided.

/**
 * @brief Sets an analog out pin to be a specified number.
 * 
 * @param devNum Device number to use.
 * @param pinNum pin to use
 * @param pinValue the value in volts that the pin will take on.
 */
void setAnalogOutPin(unsigned devNum, unsigned pinNum, float64 pinValue);

void writeDigital_extBuf(unsigned devNum, uInt32 *inputData);
void writeDigital_intBuf(unsigned devNum);
#define writeDigitalPort_(args, a, b, ...)	\
  _Generic((b),							\
           NoArg:	writeDigital_intBuf,	\
           default: writeDigital_extBuf	\
          )args
#define writeDigital(...) writeDigitalPort_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~) ///< Writes to device number (digital) from internal buffer if only devNum is specified or from external buffer if devNum and a pointer to float64 is provided.
/**
 * @brief Sets the number that the digital port will transmit. 
 * 
 * 
 * @todo What kind of transmission is this?
 * 
 * 
 * @param devNum Device number we wish to transmit over
 * @param portNum Port number we wish to transmit from
 * @param portValue The value we want to send.
 */
void setDigitalOutPort(unsigned devNum, unsigned portNum, uInt32 portValue);

/**
 * @brief Sets a pin to be a 1 or a 0 based on bitState.
 * 
 * @param devNum Device number we wish to use.
 * @param portNum port number we wish to use on the device.
 * @param pinNum Pin number on the device we wish to set to a specific state.
 * @param bitState The state we want to set our pin to.
 */
void writeDigitalPin(unsigned devNum, unsigned portNum, unsigned pinNum, bool bitState);

/**
 * @brief Sets a pin to be a 1 or a 0 based on bitState.
 * 
 * @param devNum Device number we wish to use.
 * @param portNum port number we wish to use on the device.
 * @param pinNum Pin number on the device we wish to set to a specific state.
 * @param bitState The state we want to set our pin to.
 */
void setDigitalOutPin(unsigned devNum, unsigned portNum, unsigned pinNum, bool bitState);

void readCounterAngle_extBuf(unsigned devNum, unsigned ctrNum, float64 *outputData);
void readCounterAngle_intBuf(unsigned devNum, unsigned ctrNum);
#define readCounterAngle_(args, a, b, c, ...)	\
  _Generic((c),							\
           NoArg:	readCounterAngle_intBuf,	\
           default: readCounterAngle_extBuf	\
          )args
#define readCounterAngle(...) readCounterAngle_((__VA_ARGS__), __VA_ARGS__, NO_ARG, ~) ///< Reads counter angle from internal buffer if only devNum is specified or from external buffer if devNum and a pointer to float64 is provided.
/**
 * @brief Get the Counter Angle.
 * 
 * @param devNum device number we wish to read from
 * @param ctrNum counter number on the device
 * @return float64 current counter angle.
 */
float64 getCounterAngle(unsigned devNum, unsigned ctrNum);

/**
 * @brief The crux of the NI-DAQmx system. Ensures that all samples are performed simultaneously.
 * Call this before sampling / writing
 */
void syncSampling();

// shutdown routines
/**
 * @brief Shuts down the program
 * 
 * Frees the memory for the linked lists created for the task handler, as well as the memory
 * used to store information about the device in quickDAQ.
 * Also stops any running tasks that were submitted to NI-DAQmx.
 * 
 * @return int 0 if success, otherwise ?
 */
int quickDAQTerminate();

#ifdef __cplusplus 
}
#endif 

#endif // !QUICKDAQ_H


