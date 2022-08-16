#include "stdafx.h"
#include <stdio.h>
#include <cLinkedList.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <quickDAQ.h>
#include <macrodef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus 
extern "C" {
#endif

//------------------------------
// QuickDAQmx Glabal Definitions
//------------------------------
quickDAQErrorCodes			quickDAQErrorCode	= 0;
int32						NIDAQmxErrorCode	= 0;
quickDAQStatusModes			quickDAQStatus		= STATUS_NASCENT;
bool32						lateSampleWarning	= 0;

// NI-DAQmx specific declarations
char						DAQmxDevPrefix[DAQMX_MAX_DEV_STR_LEN] = DAQMX_DEF_DEV_PREFIX;
unsigned int				DAQmxEnumerated = 0;
long						DAQmxErrorCode = 0;

const NIdefaults			DAQmxDefaults = {
// Device prefix
	.devPrefix			= DAQMX_DEF_DEV_PREFIX,

// Analog I/O settings
	.NImeasureUnits		= DAQmx_Val_Volts,
	.NIterminalConf		= DAQmx_Val_RSE,

// Analog input settings
	.AImin				= -10,
	.AImax				= 10,

// Analog output settings
	.AOmin				= -10,
	.AOmax				= 10,

// Digital I/O settings
	.NIdigiLineGroup	= DAQmx_Val_ChanForAllLines,

// Counter I/O settings
	.NIctrDecodeMode	= DAQmx_Val_X4,
	.ZidxEnable			= 0,
	.ZidxValue			= 0.0,
	.ZidxPhase			= DAQmx_Val_AHighBHigh,
	.NIctrUnits			= DAQmx_Val_Degrees,
	.angleInit			= 0.0,

// Sampling/Timing properties
	.NItriggerEdge		= DAQmx_Val_Rising,
	.NIsamplingRate		= 1000.0,
	.NIsamplesPerCh		= 1,
// Don't use this setting for analog in. Instead, use -1 (auto)
	.NIAIsampsPerCh		= -1,
	.NIsamplingMode		= DAQmx_Val_HWTimedSinglePoint, //DAQmx_Val_FiniteSamps; 
	.NIclockSource		= /*DAQMX_SAMPLE_CLK_SRC_FINITE;*/ DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED,// (DAQmxSampleMode == DAQmx_Val_HWTimedSinglePoint) ? DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED : DAQMX_SAMPLE_CLK_SRC_FINITE;

// Real-time operation output flags
	.isSampleLate		= 0,

// Encoder properties
	.encoderPPR			= 2048, // For CUI AMT103 encoder

// I/O operation properties
	.IOtimeout			= 10.0,
	.DigiAutoStart		= TRUE,
	.AnalogAutoStart	= FALSE, // (NIsamplingMode == DAQmx_Val_HWTimedSinglePoint) ? FALSE : TRUE, //SCR FALSE for HW-timed
	.dataLayout			= DAQmx_Val_GroupByChannel, // Don't use this layout for analog input
	.AIdataLayout		= DAQmx_Val_GroupByScanNumber,
	
// Miscellaneous stuff that I've lost track of - probably not used. Consider removal.
	.plsIdleState		= DAQmx_Val_Low,
	.plsInitDel			= 0,
	.plsLoTick			= 1,
	.plsHiTick			= 1
};

deviceInfo					*DAQmxDevList = NULL;
unsigned int				DAQmxDevCount = 0;
unsigned int				DAQmxMaxCount = 0;
int32						DAQmxTriggerEdge					= DAQmx_Val_Rising; // DAQmxDefaults.NItriggerEdge;
samplingModes				DAQmxSampleMode						= (samplingModes) DAQmx_Val_HWTimedSinglePoint;// DAQmxDefaults.NIsamplingMode;
char						DAQmxSampleModeString[20]			= "";
float64						DAQmxSamplingRate					= 1000.0;// DAQmxDefaults.NIsamplingRate;
uInt64						DAQmxNumDataPointsPerSample			= 1;// DAQmxDefaults.NIsamplesPerCh;
char						DAQmxClockSource[DAQMX_MAX_STR_LEN] = /*DAQMX_SAMPLE_CLK_SRC_FINITE;*/DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED;//((DAQmxSampleMode == DAQmx_Val_HWTimedSinglePoint) ? DAQMX_SAMPLE_CLK_SRC_HW_CLOCKED : DAQMX_SAMPLE_CLK_SRC_FINITE);
IOmodes						DAQmxClockSourceTask = INVALID_IO;
int							DAQmxClockSourceDev = -1, DAQmxClockSourcePin = -1;

// NI-DAQmx subsystem tasks
cLinkedList *NItaskList = NULL;
// TaskHandle *AItaskHandle = NULL, *AOtaskHandle = NULL, *DItaskHandle = NULL, *DOtaskHandle = NULL;
cLinkedList *CItaskList = NULL, * COtaskList = NULL;
// unsigned	AIpinCount = 0, AOpinCount = 0, DIpinCount = 0, DOpinCount = 0, CIpinCount = 0, COpinCount = 0;

NItask *AItask = NULL, *AOtask = NULL, *DItask = NULL, *DOtask = NULL;

//-------------------------------
// quickDAQ Function Definitions
//-------------------------------
// support functions
void DAQmxErrChk(int32 errCode)
{
	char errBuff[2048];

	NIDAQmxErrorCode = errCode;

	if (DAQmxFailed(NIDAQmxErrorCode)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		fprintf(ERRSTREAM, "NI-DAQmx Error %ld: %s\n", (long)NIDAQmxErrorCode, errBuff);
		quickDAQTerminate();
		quickDAQSetStatus(STATUS_UNKNOWN, FALSE);
		quickDAQSetError(ERROR_NIDAQMX, TRUE);
		exit(NIDAQmxErrorCode);
	}
}

inline char* dev2string(char* strBuf, unsigned int devNum)
{
	snprintf(strBuf, DAQMX_MAX_DEV_STR_LEN, "%s%d", DAQmxDevPrefix, devNum);
	return strBuf;
}

char* pin2string(char* strbuf, unsigned int devNum, IOmodes ioMode, unsigned int pinNum)
{
	char pinType[DAQMX_MAX_PIN_STR_LEN];
	switch (ioMode)
	{
	case ANALOG_IN:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "ai");
		break;
	case ANALOG_OUT:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "ao");
		break;
	case DIGITAL_IN:
		//snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "port");
		//break;
	case DIGITAL_OUT:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "port");
		break;
	case CTR_ANGLE_IN:
		//snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "ctr");
		//break;
	case CTR_TICK_OUT:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "ctr");
		break;
	default:
		fprintf(ERRSTREAM, "QuickDAQ library: FATAL: Invalid I/O type requested.\n");
		break;
	}
	snprintf(strbuf, DAQMX_MAX_STR_LEN, "%s%d/%s%d", DAQmxDevPrefix, devNum, pinType, pinNum);
	return strbuf;
}

inline int quickDAQSetError(quickDAQErrorCodes newError, bool printFlag)
{
	switch (newError)
	{
	case ERROR_NOTREADY: 
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: Library is not ready to run. Configure library, sample clock and pin mode first!\n", (int)newError);
		break;
	case ERROR_UNSUPPORTED:
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: A feature or functionality that is unsupported by quickDAQ requested.\n", (int)newError);
		break;
	case ERROR_INVIO:
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: An invalid or unsupported I/O type has been selected.\n", (int)newError);
		break;
	case ERROR_NIDAQMX:
		if (printFlag != 0) {
			fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: NI-DAQmx has generated error code %ld.\n", (int)newError, NIDAQmxErrorCode);
			char NIerrorString[1000];
			DAQmxGetErrorString(NIDAQmxErrorCode, NIerrorString, sizeof(NIerrorString));
			fprintf(ERRSTREAM, "QuickDAQ library: NI-DAQmx Error %ld: %s\n", NIDAQmxErrorCode, NIerrorString);
		}
			break;
	case ERROR_DEVCHANGE:
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: List of NI-DAQmx devices detected by quickDAQ library has changed.\n", (int)newError);
		break;
	case ERROR_NODEVICES:
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: No NI-DAQmx devices detected by quickDAQ library.\n", (int)newError);
		break;
	case ERROR_NOTCONFIG:
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: Pin and task configuration may be altered only in the preconfigure state.\n", (int)newError);
		break;
	case ERROR_NONE:
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: No error has occured.\n", (int)newError);
		break;
	default:
		newError = ERROR_UNKNOWN;
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: Undefined error code detected!\n", (int)newError);
		break;
	}
	quickDAQErrorCode = newError;	
	return (int)quickDAQErrorCode;
}

inline int quickDAQSetStatus(quickDAQStatusModes newStatus, bool printFlag)
{
	switch (newStatus)
	{
	case STATUS_NASCENT:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS NASCENT (code %d): quickDAQ is nascent and uninitialized.\n", (int)newStatus);
		break;
	case STATUS_INIT:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS INITIALIZED (code %d): quickDAQ has been initialized and ready to configure with active I/O.\n", (int)newStatus);
		break;
	case STATUS_READY:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS READY (code %d): quickDAQ is ready to run. I/O, sync, clock, and trigger resources reserved.\n", (int)newStatus);
		break;
	case STATUS_RUNNING:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS RUNNING (code %d): quickDAQ is running and data is being collected now.\n", (int)newStatus);
		break;
	case STATUS_SHUTDOWN: 
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS SHUTDOWN (code %d): quickDAQ has been stopped and resources freed.\n", (int)newStatus);
		break;
	default:
		newStatus = STATUS_UNKNOWN;
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS UNKNOWN (code %d): quickDAQ in an unknown status mode.\n", (int)newStatus);
		break;
	}
	quickDAQStatus = newStatus;
	return (int)quickDAQStatus;
}

long quickDAQGetSamplingMode(char* sampleModeString)
{
	switch (DAQmxSampleMode)
	{
	case FINITE:
		sprintf_s(sampleModeString, sizeof(DAQmxSampleModeString), "Finite");
		break;
	case HW_CLOCKED:
		sprintf_s(sampleModeString, sizeof(DAQmxSampleModeString), "Hardware Timed");
		break;
	case CONTINUOUS:
		sprintf_s(sampleModeString, sizeof(DAQmxSampleModeString), "Continuous");
		break;
	case ON_DEMAND:
		sprintf_s(sampleModeString, sizeof(DAQmxSampleModeString), "On Demand");
		break;
	default:
		sprintf_s(sampleModeString, sizeof(DAQmxSampleModeString), "UNKNOWN");
		break;
	}
	
	return DAQmxSampleMode;
}

// initialization function definitions
inline char* setDAQmxDevPrefix(char* newPrefix)
{
	if (quickDAQStatus != STATUS_NASCENT || DAQmxEnumerated == 1) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Before setting new NI-DAQmx device prefix, library must be reset and devices should NOT be enumerated.\n");
		return NULL;
	}
	strcpy_s(DAQmxDevPrefix, DAQMX_MAX_DEV_STR_LEN, newPrefix);
	return DAQmxDevPrefix;
}

void enumerateNIDevices()
{
	int buffersize = 0;
	//Buffer size datatypes
	int devicetype_buffersize;
	//int devicesernum_buffersize;

	// Device info linked list
	cLinkedList		*newDevList;
	//cListElem		*newDevElem;
	deviceInfo		*newDev;
	
	if (DAQmxEnumerated == 1) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Reenumuerating NI-DAQmx I/O devices.\n");
		free(DAQmxDevList);
		DAQmxDevList = NULL;
	}
	else if (quickDAQStatus != STATUS_NASCENT) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Library was active and will be reset before enumuerating NI-DAQmx I/O devices.\n");
		quickDAQTerminate();
	}
	DAQmxEnumerated = 1;

	newDevList = (cLinkedList*)malloc(sizeof(cLinkedList));
	cListInit(newDevList);

	//Device Info variable initialization
	char* DAQmxDevEnum = NULL;
	char* DAQmxDevRoot = NULL;

	char *devName;
	//unsigned long devNum = 0;
	int is_simulated;
	
	//Get information about the device	
	buffersize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, (void *)DAQmxDevEnum);
	DAQmxDevEnum = (char*)malloc(buffersize);
	DAQmxDevRoot = DAQmxDevEnum;
	DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, DAQmxDevEnum, buffersize); //Get the string of DAQmxDevEnum in the computer
	
	for (devName = strtok_s(DAQmxDevEnum, ",", &DAQmxDevEnum), DAQmxDevCount = 0; 
			devName != NULL; 
				devName = strtok_s(NULL, ", ", &DAQmxDevEnum), DAQmxDevCount++) {
		// Find device info, create device info object and copy to device info object
		
			//Create new device object, mark it as valid and add to linked list
		newDev = (deviceInfo*) malloc(sizeof(deviceInfo));
		cListAppend(newDevList, (void*)newDev);
		newDev->isDevValid = TRUE;
		
			// Extract device number from device name, copy both into device object
		strcpy_s(newDev->devName, sizeof(newDev->devName), devName);
		newDev->devNum = strtol(&(devName[strlen(DAQmxDevPrefix)]), NULL, 10);

			// Extract device type and copy to dev obj
		devicetype_buffersize = DAQmxGetDeviceAttribute(devName, DAQmx_Dev_ProductType, NULL);
		DAQmxGetDeviceAttribute(devName, DAQmx_Dev_ProductType, newDev->devType, min(sizeof(newDev->devType),devicetype_buffersize) );

			// Extract device serial and copy to dev obj
		DAQmxGetDeviceAttribute(devName, DAQmx_Dev_SerialNum, &(newDev->devSerial), 1);

			// Get Is device simulated? for the device
		DAQmxGetDeviceAttribute(devName, DAQmx_Dev_IsSimulated, &is_simulated, 1);
		newDev->isDevSimulated = (bool)is_simulated;

		// Set highest device number
		DAQmxMaxCount = (newDev->devNum > DAQmxMaxCount) ? newDev->devNum : DAQmxMaxCount;

			// Enumerate and copy channel counts for each I/O typr into device object
		newDev->AIcnt = enumerateNIDevChannels(newDev->devNum, ANALOG_IN  , 0);
		newDev->AOcnt = enumerateNIDevChannels(newDev->devNum, ANALOG_OUT  , 0);
		newDev->DIcnt = enumerateNIDevChannels(newDev->devNum, DIGITAL_IN  , 0);
		newDev->DOcnt = enumerateNIDevChannels(newDev->devNum, DIGITAL_OUT , 0);
		newDev->CIcnt = enumerateNIDevChannels(newDev->devNum, CTR_ANGLE_IN, 0);
		newDev->COcnt = enumerateNIDevChannels(newDev->devNum, CTR_TICK_OUT, 0);
		}

	// Create array of NI devices for fast access and clear linked list memory at the same time
	DAQmxDevList = (deviceInfo*) malloc( sizeof(deviceInfo) * (DAQmxMaxCount+1) );
		// set default validity of all elements in the array to 0
	unsigned idx;
	for (idx = 0; idx <= DAQmxMaxCount; idx++) {
		DAQmxDevList[idx].isDevValid = FALSE;
	}

	cListElem *thisElem = cListFirstElem(newDevList);
	cListElem* nextElem = cListNextElem(newDevList, thisElem);
	while (thisElem != NULL) {
		// Copy device info data to device info array
		newDev = (deviceInfo*)thisElem->obj;
		DAQmxDevList[newDev->devNum] = *newDev;

		// Free memory
		free(newDev);
		cListUnlinkElem(newDevList, thisElem);
		thisElem = nextElem;
		nextElem = cListNextElem(newDevList, thisElem);
	}

	// Printing
	printf("\n");
	printf("*** NI-DAQmx DEVICE LIST ********************************************************************************************\n");
	printf("Device Number ||    Device Name || Device type || Device Serial # || Sim? || Pins: AI  | AO  | DI  | DO  | CIN | COUT\n");
	printf("*********************************************************************************************************************\n");

	for (idx = 0; idx < DAQmxMaxCount + 1; idx++) {
		newDev = &(DAQmxDevList[idx]);
		if (newDev->isDevValid == TRUE) {
			if (newDev->isDevSimulated == FALSE)
				printf("%13u || %14s || %11s || %15ld || Nope || ", newDev->devNum, newDev->devName, newDev->devType, newDev->devSerial);
			else
				printf("%13u || %14s || %11s || %15ld || Yeah || ", newDev->devNum, newDev->devName, newDev->devType, newDev->devSerial);
			
			printf("      %3d | %3d | %3d | %3d | %3d | %3d\n", newDev->AIcnt, newDev->AOcnt, newDev->DIcnt, newDev->DOcnt, newDev->CIcnt, newDev->COcnt);
		}
	}
	printf("*********************************************************************************************************************\n");
	printf("\n");
	
	printf("*** DEVICE ENUMERATION DIAGNOSTICS **********************************************************************************\n");
	printf("Number of device IDs: %u\n", DAQmxMaxCount + 1);
	printf("Device ID array - Start ID: 0 | End ID: %u\n", DAQmxMaxCount);
	printf("Number of valid devices: %lu\n", DAQmxDevCount);
	printf("*********************************************************************************************************************\n");
	printf("\n");
	
	// Local dynamic memory cleanup
	free(DAQmxDevRoot);
	free(newDevList);
}

/*!
 * \fn unsigned int enumerateNIDevChannels(unsigned int myDev, IOmode IOtype, unsigned int printFlag)
 * Returns the number of physical channels of a particular I/O type available in a specified device.
 * A printFlag also allows users to optionally have the function print the list of available channels.
 *
 * \param myDev Device ID numver of the NI-DAQmx device as specified in QuickDAQ
 * \param IOtype The supported 'IOmode' I/O types for which number of channels must be enumerated.
 * \param printFlag Set to 1 to print the list of channels of the specified I/O types available with the device.
 * \return Returns the number of physical channels of the specified I/O type that is available in the device.
 */
unsigned int enumerateNIDevChannels(unsigned int myDev, IOmodes IOtype, unsigned int printFlag)
{
	char data[DAQmxBufSize];
	char DevIDstring[DAQMX_MAX_DEV_STR_LEN];
	char* rem_data, * oneCh_data;

	dev2string(DevIDstring, myDev);

	switch (IOtype)
	{
	case ANALOG_IN:
		DAQmxGetDevAIPhysicalChans(DevIDstring, data, DAQmxBufSize);
		break;
	case ANALOG_OUT:
		DAQmxGetDevAOPhysicalChans(DevIDstring, data, DAQmxBufSize);
		break;
	case DIGITAL_IN:
		DAQmxGetDevDIPorts(DevIDstring, data, DAQmxBufSize);
		break;
	case DIGITAL_OUT:
		DAQmxGetDevDOPorts(DevIDstring, data, DAQmxBufSize);
		break;
	case CTR_ANGLE_IN:
		DAQmxGetDevCIPhysicalChans(DevIDstring, data, DAQmxBufSize);
		break;
	case CTR_TICK_OUT:
		DAQmxGetDevCOPhysicalChans(DevIDstring, data, DAQmxBufSize);
		break;
	default:
		quickDAQSetError(ERROR_INVIO, 1);
		quickDAQTerminate();
		break;
	}

	rem_data = data;
	int i = 0;
	for (oneCh_data = strtok_s(rem_data, ",", &rem_data); oneCh_data != NULL; oneCh_data = strtok_s(rem_data, ",", &rem_data), i++) {
		if (printFlag == 1) {
			fprintf(LOGSTREAM, "Terminal %d: %s\n", i + 1, oneCh_data);
		} // end printflag if block

		// Check and omit counting of frequency scalers
		if (IOtype == CTR_TICK_OUT && strstr(oneCh_data, "freqout") != NULL) {
			i--;
		}
	}// end channel counting for loop

	return i; // returns the number of termninals of a certain I/O type avaiable in a particular device.
}

/*!
 * \fn unsigned int enumerateNIDevTerminals(unsigned int deviceNumber)
 * Detects, enumerates and prints all the terminals on a specified DAQmx device.
 *
 * \return Returns the total number of terminals on the device.
 */
unsigned int enumerateNIDevTerminals(unsigned int deviceNumber)
{
	char myDev[1 + DAQMX_MAX_DEV_STR_LEN];
	char data[DAQmxBufSize];
	char* rem_data;
	char* oneCh_data;

	dev2string(myDev, deviceNumber);
	NIDAQmxErrorCode = DAQmxGetDevTerminals(myDev, data, DAQmxBufSize);
	int charLength = (int)strnlen_s(data, DAQmxBufSize);
	rem_data = data;
	unsigned int i = 0;

	printf("\n");
	printf("*** DEV%3d TERMINAL ENUMERATION DIAGNOSTICS *************************************************************************\n", deviceNumber);
	for (oneCh_data = strtok_s(rem_data, ",", &rem_data); oneCh_data != NULL; oneCh_data = strtok_s(rem_data, ",", &rem_data), i++) {
		fprintf(LOGSTREAM, "Terminal %d: %s\n", i + 1, oneCh_data);
	}

	fprintf(LOGSTREAM, "\n\n %s - %d Terminals (%d characters)\n\n", myDev, i, charLength);
	printf("*********************************************************************************************************************\n");
	printf("\n");
	
	return i;
}


void initDevTaskFlags()
{
	// Create global tasks list
	//AItaskHandle = (TaskHandle*) malloc(sizeof(TaskHandle));
	//AOtaskHandle = (TaskHandle*) malloc(sizeof(TaskHandle));
	//DItaskHandle = (TaskHandle*) malloc(sizeof(TaskHandle));
	//DOtaskHandle = (TaskHandle*) malloc(sizeof(TaskHandle));


	// create task handles for all NI DAQmx tasks within each valid device
	unsigned int i = 0, j = 0;
	if (quickDAQStatus != STATUS_NASCENT) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Library must be reset and enumerated before initializing NI-DAQmx task flags.\n");
		return;
	}
	if (DAQmxEnumerated != 1) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Enumerate NI-DAQmx devices before initializing NI-DAQmx task flags.\n");
		return;
	}

	for (i = 0; i <= DAQmxMaxCount; i++) {
		if ((DAQmxDevList[i]).isDevValid == TRUE) {
			// AI task
			(DAQmxDevList[i]).AItask = NULL;
			//DAQmxDevList[i].AItask = &AItaskHandle;
			//DAQmxErrChk(DAQmxCreateTask("", (DAQmxDevList[i]).AItask));
			//(DAQmxDevList[i]).AItaskDataLen = 0;
			//(DAQmxDevList[i]).AItaskEnable = FALSE;
			
			// AO task
			(DAQmxDevList[i]).AOtask = NULL;
			//DAQmxDevList[i].AOtask = &AOtaskHandle;
			//DAQmxErrChk(DAQmxCreateTask("", (DAQmxDevList[i]).AOtask));
			//(DAQmxDevList[i]).AOtaskDataLen = 0;
			//(DAQmxDevList[i]).AOtaskEnable = FALSE;
			
			// DI task
			(DAQmxDevList[i]).DItask = NULL;
			//DAQmxDevList[i].DItask = &DItaskHandle;
			//DAQmxErrChk(DAQmxCreateTask("", (DAQmxDevList[i]).DItask));
			//(DAQmxDevList[i]).DItaskDataLen = 0;
			//(DAQmxDevList[i]).DItaskEnable = FALSE;
			
			// DO task
			(DAQmxDevList[i]).DOtask = NULL;
			//DAQmxDevList[i].DOtask = &DOtaskHandle;
			//DAQmxErrChk(DAQmxCreateTask("", (DAQmxDevList[i]).DOtask));
			//(DAQmxDevList[i]).DOtaskDataLen = 0;
			//(DAQmxDevList[i]).DOtaskEnable = FALSE;
			
			// CI tasks
			(DAQmxDevList[i]).CItask = (NItask**)malloc(((DAQmxDevList[i]).CIcnt) * sizeof(NItask*));
			//(DAQmxDevList[i]).CItaskDataLen = (unsigned*)malloc(((DAQmxDevList[i]).CIcnt) * sizeof(unsigned));
			//(DAQmxDevList[i]).CItaskEnable = (bool*)malloc(((DAQmxDevList[i]).CIcnt) * sizeof(bool));
			for (j = 0; j < (DAQmxDevList[i]).CIcnt; j++) {
				(DAQmxDevList[i]).CItask[j] = NULL;
				//DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).CItask[j])));
				//(DAQmxDevList[i]).CItaskEnable[j] = FALSE;
			}

			// CO tasks
			(DAQmxDevList[i]).COtask = (NItask**)malloc(((DAQmxDevList[i]).COcnt) * sizeof(NItask*));
			//(DAQmxDevList[i]).COtaskDataLen = (unsigned*)malloc(((DAQmxDevList[i]).COcnt) * sizeof(unsigned));
			//(DAQmxDevList[i]).COtaskEnable = (bool*)malloc(((DAQmxDevList[i]).COcnt) * sizeof(bool));
			for (j = 0; j < (DAQmxDevList[i]).COcnt; j++) {
				(DAQmxDevList[i]).COtask[j] = NULL;
				//DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).COtask[j])));
				//(DAQmxDevList[i]).COtaskEnable[j] = FALSE;
			}
		}
	}


}

void quickDAQinit()
{
	char newPrefix[] = DAQMX_DEF_DEV_PREFIX;
	quickDAQSetStatus(STATUS_NASCENT, FALSE);
	
	DAQmxEnumerated = 0;
	setDAQmxDevPrefix(newPrefix);
	enumerateNIDevices();
	initDevTaskFlags();

	NItaskList = (cLinkedList*)malloc(sizeof(cLinkedList));
	cListInit(NItaskList);
	
	quickDAQSetStatus(STATUS_INIT, TRUE);
}

// configuration function definitions
inline void setActiveEdgeRising()
{
	if (quickDAQStatus <= STATUS_INIT && quickDAQStatus != STATUS_UNKNOWN)
		DAQmxTriggerEdge = DAQmx_Val_Rising;
}

inline void setActiveEdgeFalling()
{
	if (quickDAQStatus <= STATUS_INIT && quickDAQStatus != STATUS_UNKNOWN)
		DAQmxTriggerEdge = DAQmx_Val_Falling;
}

void setSampleClockTiming(samplingModes sampleMode, float64 samplingRate, char *triggerSource, triggerModes triggerEdge, uInt64 numDataPointsPerSample, bool printFlag)
{
	if (quickDAQStatus == STATUS_INIT) {
		DAQmxSampleMode = sampleMode;
		DAQmxSamplingRate = samplingRate;
		strcpy_s(DAQmxClockSource, DAQMX_MAX_STR_LEN, triggerSource);
		DAQmxTriggerEdge = triggerEdge;
		DAQmxNumDataPointsPerSample = numDataPointsPerSample;

		quickDAQGetSamplingMode(DAQmxSampleModeString);
		fprintf(ERRSTREAM, "\nSetting up DAQmx sample clock timing with sample mode %d (%s) at %0.2f Hz.\n", DAQmxSampleMode, DAQmxSampleModeString, DAQmxSamplingRate);
		fprintf(ERRSTREAM, "\tClock source is '%s'.\n", DAQmxClockSource);

		NItask* myTask = NULL;
		cListElem* myElem = NULL;
		unsigned isFirstTask = 1;

		for (myElem = cListFirstElem(NItaskList); myElem != NULL; myElem = cListNextElem(NItaskList, myElem)) {
			myTask = (NItask*)myElem->obj;
			if (myTask != DItask && myTask != DOtask) {
				if (isFirstTask == 1) {
					DAQmxErrChk(DAQmxCfgSampClkTiming(myTask->taskHandler, "", DAQmxSamplingRate,
						DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
					fprintf(ERRSTREAM, "First task: ");
					isFirstTask = 0;
				}
				else {
					DAQmxErrChk(DAQmxCfgSampClkTiming(myTask->taskHandler, DAQmxClockSource, DAQmxSamplingRate,
						DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
				}
			}
			if (sampleMode == HW_CLOCKED) 
				DAQmxSetRealTimeConvLateErrorsToWarnings(myTask->taskHandler, TRUE);
			fprintf(ERRSTREAM, "Sample clock source and timing have been set.\n\n");
		}

		if(!cListEmpty(NItaskList))
			quickDAQSetStatus(STATUS_READY, TRUE);

		/*
		unsigned int i, j, k, isSet;
		for (i = 0; i <= DAQmxMaxCount; i++) {
			deviceInfo* thisDev = &(DAQmxDevList[i]);
			if (thisDev->isDevValid == TRUE) {
				//AI task
				for (k = 0, isSet = 0; k < thisDev->AIcnt && isSet == 0; k++) {
					if (thisDev->AIpins[k].isPinValid == TRUE) {
						thisDev->AItaskEnable = TRUE; isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->AItask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
						if (sampleMode == HW_CLOCKED) DAQmxSetRealTimeConvLateErrorsToWarnings(thisDev->AItask, TRUE);
						quickDAQSetStatus(STATUS_READY, FALSE);
						if (printFlag) fprintf(ERRSTREAM, "\n\tDev %d : AI%d | CLK SRC: %s", i, k, DAQmxClockSource);
					}
				}

				//AO task
				for (k = 0, isSet = 0; k < thisDev->AOcnt && isSet == 0; k++) {
					if (thisDev->AOpins[k].isPinValid == TRUE) {
						thisDev->AOtaskEnable = TRUE; isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->AOtask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
						if (sampleMode == HW_CLOCKED) DAQmxSetRealTimeConvLateErrorsToWarnings(thisDev->AOtask, TRUE);
						quickDAQSetStatus(STATUS_READY, FALSE);
						if (printFlag) fprintf(ERRSTREAM, "\n\tDev %d : AO%d | CLK SRC: %s", i, k, DAQmxClockSource);
					}
				}

				//DI task
				for (k = 0, isSet = 0; k < thisDev->DIcnt && isSet == 0; k++) {
					if (thisDev->DIpins[k].isPinValid == TRUE) {
						thisDev->DItaskEnable = TRUE; isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->DItask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
						if (sampleMode == HW_CLOCKED) DAQmxSetRealTimeConvLateErrorsToWarnings(thisDev->DItask, TRUE);
						quickDAQSetStatus(STATUS_READY, FALSE);
						if (printFlag) fprintf(ERRSTREAM, "\n\tDev %d : DI%d | CLK SRC: %s", i, k, DAQmxClockSource);
					}
				}
				
				//DO task
				for (k = 0, isSet = 0; k < thisDev->DOcnt && isSet == 0; k++) {
					if (thisDev->DOpins[k].isPinValid == TRUE) {
						thisDev->DOtaskEnable = TRUE; isSet = 1;
						//DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->DOtask, DAQmxClockSource, DAQmxSamplingRate, DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
						// DIGITAL OUT ONLY SUPPORTS ON-DEMAND DATA COLLECTION.
						quickDAQSetStatus(STATUS_READY, FALSE);
						if (printFlag) fprintf(ERRSTREAM, "\n\tDev %d : DO%d | CLK SRC: %s", i, k, "ON DEMAND MODE");
					}
				}
				
				// CI tasks
				char CIclockSource[255] = "/PXI1Slot3/PFI9";
				for (j = 0; j < (DAQmxDevList[i]).CIcnt; j++) {
					if (thisDev->CIpins[j].isPinValid == TRUE) {
						if (sampleMode == DAQmx_Val_HWTimedSinglePoint)
							strcpy_s(CIclockSource, 255, triggerSource);
						thisDev->CItaskEnable[j] = TRUE;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->CItask[j], CIclockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample)); //SCR
						if (sampleMode == HW_CLOCKED) DAQmxSetRealTimeConvLateErrorsToWarnings(thisDev->CItask[j], TRUE);
						quickDAQSetStatus(STATUS_READY, FALSE);
						if (printFlag) fprintf(ERRSTREAM, "\n\tDev %d : CI%d | CLK SRC: %s", i, k, DAQmxClockSource);
					}
				}

				// CO tasks
				for (j = 0; j < (DAQmxDevList[i]).COcnt; j++) {
					if (thisDev->COpins[j].isPinValid == TRUE) {
						thisDev->COtaskEnable[j] = TRUE;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->COtask[j], DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
						if (sampleMode == HW_CLOCKED) DAQmxSetRealTimeConvLateErrorsToWarnings(thisDev->COtask[j], TRUE);
						quickDAQSetStatus(STATUS_READY, FALSE);
						if (printFlag) fprintf(ERRSTREAM, "\n\tDev %d : CO%d | CLK SRC: %s", i, k, DAQmxClockSource);
					}
				}
			}
		}
		*/
		
		if (printFlag) fprintf(ERRSTREAM, "\n");
		if (quickDAQStatus == STATUS_READY) quickDAQSetStatus(quickDAQStatus, TRUE);
	}
}

bool setClockSource(unsigned devNum, int pinNum, IOmodes ioMode)
{
	// setup clock source now
	bool isNewClockSet = FALSE;
	char localClock[256];
	char sourceDev[DAQMX_MAX_DEV_PREFIX_LEN];
	dev2string(sourceDev, devNum);
	if (ioMode == ANALOG_IN && DAQmxClockSourceTask > ANALOG_IN) {
		//strncat_s(localClock, sizeof(localClock), "/ai/SampleClock", sizeof(localClock) - strlen(localClock) - 1);
		//strncpy_s(DAQmxClockSource, sizeof(DAQmxClockSource), localClock, sizeof(localClock));
		sprintf_s(DAQmxClockSource, sizeof(DAQmxClockSource), "/%s/ai/SampleClock", sourceDev);
		DAQmxClockSourceTask = ANALOG_IN;
		DAQmxClockSourceDev = devNum;
		isNewClockSet = TRUE;
	}
	else if (ioMode == ANALOG_OUT && DAQmxClockSourceTask > ANALOG_OUT) {
		strncat_s(localClock, sizeof(localClock), "/ao/SampleClock", sizeof(localClock) - strlen(localClock) - 1);
		strncpy_s(DAQmxClockSource, sizeof(DAQmxClockSource), localClock, sizeof(localClock));
		DAQmxClockSourceTask = ANALOG_OUT;
		DAQmxClockSourceDev = devNum;
		isNewClockSet = TRUE;
	}
	else if (ioMode == DIGITAL_IN && DAQmxClockSourceTask > DIGITAL_IN) {
		strncat_s(localClock, sizeof(localClock), "/di/SampleClock", sizeof(localClock) - strlen(localClock) - 1);
		strncpy_s(DAQmxClockSource, sizeof(DAQmxClockSource), localClock, sizeof(localClock));
		DAQmxClockSourceTask = DIGITAL_IN;
		isNewClockSet = TRUE;
	}
	else if (ioMode == DIGITAL_OUT && DAQmxClockSourceTask > DIGITAL_OUT) {
		strncat_s(localClock, sizeof(localClock), "/do/SampleClock", sizeof(localClock) - strlen(localClock) - 1);
		strncpy_s(DAQmxClockSource, sizeof(DAQmxClockSource), localClock, sizeof(localClock));
		DAQmxClockSourceTask = DIGITAL_OUT;
		DAQmxClockSourceDev = devNum;
		isNewClockSet = TRUE;
	}
	else if (ioMode == CTR_ANGLE_IN && DAQmxClockSourceTask > CTR_ANGLE_IN) {
		strncat_s(localClock, sizeof(localClock), "OnboardClock", sizeof(localClock) - strlen(localClock) - 1);
		strncpy_s(DAQmxClockSource, sizeof(DAQmxClockSource), localClock, sizeof(localClock));
		DAQmxClockSourceTask = CTR_ANGLE_IN;
		DAQmxClockSourceDev = devNum;
		isNewClockSet = TRUE;
	}
	else if (ioMode == CTR_TICK_OUT && DAQmxClockSourceTask > CTR_TICK_OUT) {
		strncat_s(localClock, sizeof(localClock), "OnboardClock", sizeof(localClock) - strlen(localClock) - 1);
		strncpy_s(DAQmxClockSource, sizeof(DAQmxClockSource), localClock, sizeof(localClock));
		DAQmxClockSourceTask = CTR_TICK_OUT;
		DAQmxClockSourceDev = devNum;
		isNewClockSet = TRUE;
	}
	/*
	else {
		sprintf_s(DAQmxClockSource, sizeof(DAQmxClockSource), "OnboardClock");
		//DAQmxClockSourceTask = INVALID_IO;
		//DAQmxClockSourceDev = devNum;
	}
	*/
	if (isNewClockSet == TRUE) {
		fprintf(ERRSTREAM, "Set clk src.: %s\n", DAQmxClockSource);
		return TRUE;
	}
	return FALSE;
}

void pinModeErrHandler(unsigned int devNum, IOmodes ioMode, unsigned int pinNum) {
	fprintf(ERRSTREAM, "QuickDAQ library: FATAL: Invalid I/O requested: Dev %d | IO mode %d | pin %d.\n", devNum, ioMode, pinNum);
	quickDAQTerminate();
	quickDAQSetStatus(STATUS_SHUTDOWN, TRUE);
	quickDAQSetError(ERROR_INVIO, TRUE);
	exit(quickDAQErrorCode);
}

void pinMode(unsigned int devNum, IOmodes ioMode, unsigned int pinNum)
{
	if (quickDAQStatus == STATUS_INIT) {
		char pinName[DAQMX_MAX_STR_LEN];
		char pinModeStr[20];
		pin2string(pinName, devNum, ioMode, pinNum);
		deviceInfo* thisDev = &(DAQmxDevList[devNum]);
		//NItask* tempTaskObj = NULL;
		NItask* clkSourceTask = NULL;
		unsigned pinID = pinNum;

		if (thisDev->isDevValid != 0) {
			switch (ioMode)
			{
			case ANALOG_IN:
				if (thisDev->AIcnt != 0 && pinID >= 0 && pinID < thisDev->AIcnt) {
					thisDev->AIpins[pinID].isPinValid = TRUE;
					thisDev->AIpins[pinID].pinNum = pinNum;
					thisDev->AIpins[pinID].pinIOMode = ioMode;
					if (AItask == NULL) {
						AItask = (NItask*)malloc(sizeof(NItask));
						AItask->taskType = ANALOG_IN;
						AItask->pinCount = 0;
						AItask->dataBuffer = NULL;
						//AItaskHandle = (TaskHandle*)malloc(sizeof(TaskHandle));
						DAQmxErrChk(DAQmxCreateTask("", &(AItask->taskHandler)));
						//thisDev->AItaskEnable = TRUE;
						thisDev->AItask = AItask;
						clkSourceTask = thisDev->AItask;
						//cListAppend(NItaskList, (void*)sourceTask);
					}
					AItask->pinCount++;

					thisDev->AIpins[pinID].pinTask = thisDev->AItask;
					DAQmxErrChk(DAQmxCreateAIVoltageChan(thisDev->AItask->taskHandler, pinName            , ""                          , DAQmxDefaults.NIterminalConf,
														 DAQmxDefaults.AImin		 , DAQmxDefaults.AImax, DAQmxDefaults.NImeasureUnits, NULL));
					//thisDev->AItaskDataLen++;
					sprintf_s(pinModeStr, sizeof(pinModeStr), "ANALOG IN");
				}
				else {
					pinModeErrHandler(devNum, ioMode, pinNum);
				}
				break;

			case ANALOG_OUT:
				if (thisDev->AOcnt != 0 && pinID >= 0 && pinID < thisDev->AOcnt) {
					thisDev->AOpins[pinID].isPinValid = TRUE;
					thisDev->AOpins[pinID].pinNum = pinNum;
					thisDev->AOpins[pinID].pinIOMode = ioMode;
					if (AOtask == NULL) {
						AOtask = (NItask*)malloc(sizeof(NItask));
						AOtask->taskType = ANALOG_OUT;
						AOtask->pinCount = 0;
						AOtask->dataBuffer = NULL;
						//AOtaskHandle = (TaskHandle*)malloc(sizeof(TaskHandle));
						DAQmxErrChk(DAQmxCreateTask("", &(AOtask->taskHandler)));
						//thisDev->AOtaskEnable = TRUE;
						thisDev->AOtask = AOtask;
						clkSourceTask = thisDev->AOtask;
						//cListAppend(NItaskList, (void*)sourceTask);
					}
					AOtask->pinCount++;

					thisDev->AOpins[pinID].pinTask = thisDev->AOtask;
					DAQmxErrChk(DAQmxCreateAOVoltageChan(thisDev->AOtask->taskHandler    , pinName            , ""                          ,
														 DAQmxDefaults.AOmin			 , DAQmxDefaults.AOmax, DAQmxDefaults.NImeasureUnits, NULL));
					//thisDev->AOtaskDataLen++;
					sprintf_s(pinModeStr, sizeof(pinModeStr), "ANALOG OUT");
				}
				else {
					pinModeErrHandler(devNum, ioMode, pinNum);
				}
				break;

			case DIGITAL_IN:
				// SCR TODO
				break;

			case DIGITAL_OUT:
				if (thisDev->DOcnt != 0 && pinID >= 0 && pinID < thisDev->DOcnt) {
					thisDev->DOpins[pinID].isPinValid = TRUE;
					thisDev->DOpins[pinID].pinNum = pinNum;
					thisDev->DOpins[pinID].pinIOMode = ioMode;
					if (DOtask == NULL) {
						DOtask = (NItask*)malloc(sizeof(NItask));
						DOtask->taskType = DIGITAL_OUT;
						DOtask->pinCount = 0;
						DOtask->dataBuffer = NULL;
						//DOtaskHandle = (TaskHandle*)malloc(sizeof(TaskHandle));
						DAQmxErrChk(DAQmxCreateTask("", &(DOtask->taskHandler)));
						//thisDev->DOtaskEnable = TRUE;
						thisDev->DOtask = DOtask;
						clkSourceTask = thisDev->DOtask;
						//cListAppend(NItaskList, (void*)sourceTask);
					}
					DOtask->pinCount++;

					thisDev->DOpins[pinID].pinTask = thisDev->DOtask;
					DAQmxErrChk(DAQmxCreateDOChan(thisDev->DOtask->taskHandler, pinName, "", DAQmxDefaults.NIdigiLineGroup));
					//thisDev->DOtaskDataLen++;
					sprintf_s(pinModeStr, sizeof(pinModeStr), "DIGITAL OUT");
				} 
				else {
					pinModeErrHandler(devNum, ioMode, pinNum);
				}
				break;

			case CTR_ANGLE_IN:
				if (thisDev->CIcnt != 0 && pinID >= 0 && pinID < thisDev->CIcnt) {
					thisDev->CIpins[pinID].isPinValid = TRUE;
					thisDev->CIpins[pinID].pinNum = pinNum;
					thisDev->CIpins[pinID].pinIOMode = ioMode;
					if (CItaskList == NULL) {
						CItaskList = (cLinkedList*)malloc(sizeof(cLinkedList));
						cListInit(CItaskList);
					}
					clkSourceTask = (NItask*)malloc(sizeof(NItask));
					clkSourceTask->taskType = CTR_ANGLE_IN;
					clkSourceTask->pinCount = 1;
					clkSourceTask->dataBuffer = NULL;
					//clkSourceTask = (TaskHandle *)malloc(sizeof(TaskHandle));
					DAQmxErrChk(DAQmxCreateTask("", &(clkSourceTask->taskHandler)));
					//thisDev->CItaskEnable[pinID] = TRUE;
					thisDev->CItask[pinID] = clkSourceTask;
					clkSourceTask = thisDev->CItask[pinID];
					cListAppend(CItaskList, (void*)clkSourceTask);

					thisDev->CIpins[pinID].pinTask = thisDev->CItask[pinID];
					DAQmxErrChk(DAQmxCreateCIAngEncoderChan(thisDev->CItask[pinID]->taskHandler , pinName             , "", DAQmxDefaults.NIctrDecodeMode,
															DAQmxDefaults.ZidxEnable			, DAQmxDefaults.ZidxValue , DAQmxDefaults.ZidxPhase,
															DAQmxDefaults.NIctrUnits			, DAQmxDefaults.encoderPPR, DAQmxDefaults.angleInit, ""));
					//thisDev->CItaskDataLen[pinID]++;
					sprintf_s(pinModeStr, sizeof(pinModeStr), "COUNTER(ANGLE) IN");
				}
				else {
					pinModeErrHandler(devNum, ioMode, pinNum);
				}
				break;

			case CTR_TICK_OUT:
				// SCR TODO
				break;

			default:
				pinModeErrHandler(devNum, ioMode, pinNum);
				return;
				break;
			} // end IO mode switch block

			// Auto-set sample clock source using setClockSource function
			if (clkSourceTask != NULL) {
				if (setClockSource(devNum, pinID, ioMode) == TRUE) {
					cListPrepend(NItaskList, (void *)clkSourceTask);
				}
				else
					cListAppend(NItaskList, (void *)clkSourceTask);
			}

			fprintf(ERRSTREAM, "Set pin mode: %s [%s]\n", pinName, pinModeStr);
		} // end device validity check if block
	}
}

// library run function definitions
void quickDAQstart()
{

	const float64	zeroAnalog		= 0.000;
	const uInt32	zeroDigital_32b = 0x00000000;

	if (quickDAQStatus == STATUS_READY) {
		//quickDAQSetStatus(STATUS_RUNNING, TRUE);
		fprintf(ERRSTREAM, "Starting %d NI-DAQmx tasks...\n", cListLength(NItaskList));
		/*fprintf(ERRSTREAM, "\tDetected %d AI pins, %d AO pins, %d DI pins, %d DO pins, %d CI pins, %d CO pins.\n",
			AIpinCount, AOpinCount, DIpinCount, DOpinCount, CIpinCount, COpinCount);
		*/
		
		cListElem	*myElem = NULL;
		NItask		*myTask = NULL;
		unsigned	ii = 0;
		for (myElem = cListFirstElem(NItaskList); myElem != NULL; myElem = cListNextElem(NItaskList, myElem)) {
			myTask = (NItask*)myElem->obj;
			switch (myTask->taskType)
			{
			case ANALOG_IN:
				fprintf(ERRSTREAM, "Starting DAQmx 'ANALOG IN' task with %d active pins\n", myTask->pinCount);
				myTask->dataBuffer = (void*)malloc(myTask->pinCount * sizeof(float64));
				for (ii = 0; ii < myTask->pinCount; ii++) {
					((float64*)myTask->dataBuffer)[ii] = zeroAnalog;
				}
				break;
			case ANALOG_OUT:
				fprintf(ERRSTREAM, "Starting DAQmx 'ANALOG OUT' task with %d active pins\n", myTask->pinCount);
				myTask->dataBuffer = (void*)malloc(myTask->pinCount * sizeof(float64));
				for (ii = 0; ii < myTask->pinCount; ii++) {
					((float64*)myTask->dataBuffer)[ii] = zeroAnalog;
				}
				break;
			case DIGITAL_IN:
				fprintf(ERRSTREAM, "Starting DAQmx 'DIGITAL IN' task with %d active ports\n", myTask->pinCount);
				myTask->dataBuffer = (void*)malloc(myTask->pinCount * sizeof(uInt32));
				for (ii = 0; ii < myTask->pinCount; ii++) {
					((uInt32*)myTask->dataBuffer)[ii] = zeroDigital_32b;
				}
				break;
			case DIGITAL_OUT:
				fprintf(ERRSTREAM, "Starting DAQmx 'DIGITAL OUT' task with %d active ports\n", myTask->pinCount);
				myTask->dataBuffer = (void*)malloc(myTask->pinCount * sizeof(uInt32));
				for (ii = 0; ii < myTask->pinCount; ii++) {
					((uInt32*)myTask->dataBuffer)[ii] = zeroDigital_32b;
				}
				break;
			case CTR_ANGLE_IN:
				fprintf(ERRSTREAM, "Starting DAQmx 'COUNTER ANGLE IN' task with %d active counters\n", myTask->pinCount);
				myTask->dataBuffer = (void*)malloc(myTask->pinCount * sizeof(float64));
				for (ii = 0; ii < myTask->pinCount; ii++) {
					((float64*)myTask->dataBuffer)[ii] = zeroAnalog;
				}
				break;
			case CTR_TICK_OUT:
				fprintf(ERRSTREAM, "Starting DAQmx 'COUNTER TICK OUT' task with %d active counters\n", myTask->pinCount);
				myTask->dataBuffer = (void*)malloc(myTask->pinCount * sizeof(uInt32));
				for (ii = 0; ii < myTask->pinCount; ii++) {
					((uInt32*)myTask->dataBuffer)[ii] = zeroDigital_32b;
				}
				break;
			default:
				fprintf(ERRSTREAM, "quickDAQ: FATAL: Attempting to start a task of unknown I/O type.\n");
				quickDAQTerminate();
				quickDAQSetStatus(STATUS_UNKNOWN, FALSE);
				quickDAQSetError(ERROR_INVIO, TRUE);
				exit(quickDAQErrorCode);
				break;
			}
			DAQmxErrChk(DAQmxStartTask(myTask->taskHandler));
		}
		


		/*
		unsigned i = 0, j = 0;
		for (i = 0; i < DAQmxMaxCount; i++) {
			if (DAQmxDevList[i].isDevValid == TRUE) {
				deviceInfo* thisDev = &(DAQmxDevList[i]);
				if (thisDev->AItaskEnable == TRUE)
					DAQmxErrChk(DAQmxStartTask(thisDev->AItask));
				if (thisDev->AOtaskEnable == TRUE)
					DAQmxErrChk(DAQmxStartTask(thisDev->AOtask));
				if (thisDev->DItaskEnable == TRUE)
					DAQmxErrChk(DAQmxStartTask(thisDev->DItask));
				if (thisDev->DOtaskEnable == TRUE)
					DAQmxErrChk(DAQmxStartTask(thisDev->DOtask));
				for (j = 0; j < thisDev->CIcnt; j++) {
					if (thisDev->CItaskEnable[j] == TRUE) DAQmxErrChk(DAQmxStartTask(thisDev->CItask[j]));
				}
				for (j = 0; j < thisDev->COcnt; j++) {
					if (thisDev->COtaskEnable[j] == TRUE) DAQmxErrChk(DAQmxStartTask(thisDev->COtask[j]));
				}
			}
		}
		*/
	
		quickDAQSetStatus(STATUS_RUNNING, TRUE);
	}
}

void quickDAQstop()
{
	if (quickDAQStatus == STATUS_RUNNING) {
		
		cListElem *myElem = NULL;
		NItask* myTask = NULL;
		for (myElem = cListFirstElem(NItaskList); myElem != NULL; myElem = cListNextElem(NItaskList, myElem)) {
			myTask = (NItask*)myElem->obj;
			DAQmxErrChk(DAQmxStopTask(myTask->taskHandler));
			//fprintf(ERRSTREAM, "Stopped a DAQmx task\n");
			switch (myTask->taskType)
			{
			case ANALOG_IN:
				fprintf(ERRSTREAM, "Stopped DAQmx 'ANALOG IN' task with %d active pin(s)\n", myTask->pinCount);
				break;
			case ANALOG_OUT:
				fprintf(ERRSTREAM, "Stopped DAQmx 'ANALOG OUT' task with %d active pin(s)\n", myTask->pinCount);
				break;
			case DIGITAL_IN:
				fprintf(ERRSTREAM, "Starting DAQmx 'DIGITAL IN' task with %d active port(s)\n", myTask->pinCount);
				break;
			case DIGITAL_OUT:
				fprintf(ERRSTREAM, "Stopped DAQmx 'DIGITAL OUT' task with %d active port(s)\n", myTask->pinCount);
				break;
			case CTR_ANGLE_IN:
				fprintf(ERRSTREAM, "Stopped DAQmx 'COUNTER ANGLE IN' task with %d active counter(s)\n", myTask->pinCount);
				break;
			case CTR_TICK_OUT:
				fprintf(ERRSTREAM, "Stopped DAQmx 'COUNTER TICK OUT' task with %d active counter(s)\n", myTask->pinCount);
				break;
			default:
				fprintf(ERRSTREAM, "quickDAQ: FATAL: Attempted to stop a task of unknown I/O type.\n");
				quickDAQTerminate();
				quickDAQSetStatus(STATUS_UNKNOWN, FALSE);
				quickDAQSetError(ERROR_INVIO, TRUE);
				exit(quickDAQErrorCode);
				break;
			}
		}
		quickDAQSetStatus(STATUS_READY, TRUE);
		
		/*
		unsigned i = 0, j = 0;
		for (i = 0; i < DAQmxMaxCount; i++) {
			deviceInfo* thisDev = &(DAQmxDevList[i]);
			if (thisDev->AItaskEnable == TRUE) DAQmxErrChk(DAQmxStopTask(thisDev->AItask));
			if (thisDev->AOtaskEnable == TRUE) DAQmxErrChk(DAQmxStopTask(thisDev->AOtask));
			if (thisDev->DItaskEnable == TRUE) DAQmxErrChk(DAQmxStopTask(thisDev->DItask));
			if (thisDev->DOtaskEnable == TRUE) DAQmxErrChk(DAQmxStopTask(thisDev->DOtask));
			for (j = 0; j < thisDev->CIcnt; j++) {
				if (thisDev->CItaskEnable[j] == TRUE) DAQmxErrChk(DAQmxStopTask(thisDev->CItask[j]));
			}
			for (j = 0; j < thisDev->COcnt; j++) {
				if (thisDev->COtaskEnable[j] == TRUE) DAQmxErrChk(DAQmxStopTask(thisDev->COtask[j]));
			}
		}
		*/
	}
}

// read/write function definitions
void readAnalog(unsigned devNum, float64 *outputData)
{
	//printf("\n%d\n", DAQmxDevList[devNum].AItaskDataLen);
	if (quickDAQStatus == STATUS_RUNNING) {
		DAQmxErrChk(DAQmxReadAnalogF64(AItask->taskHandler, DAQmxDefaults.NIAIsampsPerCh, DAQmxDefaults.IOtimeout ,
									   DAQmxDefaults.AIdataLayout, (float64*)AItask->dataBuffer, AItask->pinCount, NULL, NULL));
		memcpy(outputData, AItask->dataBuffer, AItask->pinCount * sizeof(float64));
	}
}

void writeAnalog(unsigned devNum, float64 *inputData)
{
	if (quickDAQStatus == STATUS_RUNNING) {
		memcpy(AOtask->dataBuffer, inputData, AOtask->pinCount * sizeof(float64));
		DAQmxErrChk(DAQmxWriteAnalogF64(AOtask->taskHandler, DAQmxDefaults.NIsamplesPerCh, DAQmxDefaults.AnalogAutoStart,
										DAQmxDefaults.IOtimeout, DAQmxDefaults.dataLayout, (float64*)AOtask->dataBuffer, NULL, NULL));
	}

}

void writeDigital(unsigned devNum, uInt32 *inputData)
{
	if (quickDAQStatus == STATUS_RUNNING) {
		memcpy(DOtask->dataBuffer, inputData, DOtask->pinCount * sizeof(uInt32));
		DAQmxErrChk(DAQmxWriteDigitalU32(DOtask->taskHandler, DAQmxDefaults.NIsamplesPerCh, DAQmxDefaults.DigiAutoStart,
										 DAQmxDefaults.IOtimeout, DAQmxDefaults.dataLayout, (uInt32*)DOtask->dataBuffer, NULL, NULL));
	}

}

void writeDigitalPin(unsigned devNum, unsigned pinNum, bool bitState)
{
	uInt32 myWord;
	if (quickDAQStatus == STATUS_RUNNING) {
		memcpy(&myWord, &(((uInt32*)DOtask->dataBuffer)[pinNum]), sizeof(myWord));
		myWord = (bitState == TRUE) ? ((myWord & (0 << pinNum)) | (0x00000001 << pinNum)) : ((myWord & (0 << pinNum)) | (0x00000000 << pinNum));
		memcpy(&myWord, &(((uInt32*)DOtask->dataBuffer)[pinNum]), sizeof(uInt32));
		DAQmxErrChk(DAQmxWriteDigitalU32(DOtask->taskHandler, DAQmxDefaults.NIsamplesPerCh, DAQmxDefaults.DigiAutoStart,
										 DAQmxDefaults.IOtimeout, DAQmxDefaults.dataLayout, (uInt32*)DOtask->dataBuffer, NULL, NULL));

	}
}

void readCounterAngle(unsigned devNum, unsigned pinNum, float64 *outputData)
{
	if (quickDAQStatus == STATUS_RUNNING) {
		DAQmxErrChk(DAQmxReadCounterF64(DAQmxDevList[devNum].CItask[pinNum]->taskHandler, DAQmxDefaults.NIsamplesPerCh,
			DAQmxDefaults.IOtimeout, (float64*)DAQmxDevList[devNum].CItask[pinNum]->dataBuffer, DAQmxDevList[devNum].CItask[pinNum]->pinCount, NULL, NULL));
		memcpy(outputData, DAQmxDevList[devNum].CItask[pinNum]->dataBuffer, DAQmxDevList[devNum].CItask[pinNum]->pinCount * sizeof(float64));
	}

}

void syncSampling()
{
	if (DAQmxSampleMode == DAQmx_Val_HWTimedSinglePoint) {
		DAQmxErrChk(DAQmxWaitForNextSampleClock( ((NItask*)cListFirstData(NItaskList))->taskHandler, DAQmxDefaults.IOtimeout, &lateSampleWarning));
		/*
		switch (ioMode)
		{
		case ANALOG_IN:
			DAQmxErrChk(DAQmxWaitForNextSampleClock(DAQmxDevList[devNum].AItask, DAQmxDefaults.IOtimeout, FALSE));
			break;
		case ANALOG_OUT:
			DAQmxErrChk(DAQmxWaitForNextSampleClock(DAQmxDevList[devNum].AOtask, DAQmxDefaults.IOtimeout, FALSE));
			break;
		case DIGITAL_IN:
			DAQmxErrChk(DAQmxWaitForNextSampleClock(DAQmxDevList[devNum].DItask, DAQmxDefaults.IOtimeout, FALSE));
			break;
		case DIGITAL_OUT:
			DAQmxErrChk(DAQmxWaitForNextSampleClock(DAQmxDevList[devNum].DOtask, DAQmxDefaults.IOtimeout, FALSE));
			break;
		case CTR_ANGLE_IN:
			DAQmxErrChk(DAQmxWaitForNextSampleClock(DAQmxDevList[devNum].COtask[pinNum], DAQmxDefaults.IOtimeout, FALSE));
			break;
		case CTR_TICK_OUT:
			DAQmxErrChk(DAQmxWaitForNextSampleClock(DAQmxDevList[devNum].COtask[pinNum], DAQmxDefaults.IOtimeout, FALSE));
			break;
		default:
			break;
		}
		*/
	}
}

// shutdown function definitions
int quickDAQTerminate()
{
	// force shutdown all NI DAQmx tasks for all devices
	/*
	cListElem* myElem = NULL;
	for (myElem = cListFirstElem(NItaskList); myElem != NULL; myElem = cListNextElem(NItaskList, myElem)) {
		DAQmxStopTask ( *((TaskHandle*)myElem->obj) );
		DAQmxClearTask( *((TaskHandle*)myElem->obj) );
	}
	*/

	/*
	unsigned int i = 0, j = 0;
	for (i = 0; i <= DAQmxMaxCount; i++) {
		if ((DAQmxDevList[i]).isDevValid == TRUE) {
			//AI task
			DAQmxStopTask((DAQmxDevList[i]).AItask);
			DAQmxClearTask((DAQmxDevList[i]).AItask);

			//AO task
			DAQmxStopTask((DAQmxDevList[i]).AOtask);
			DAQmxClearTask((DAQmxDevList[i]).AOtask);

			//DI task
			DAQmxStopTask((DAQmxDevList[i]).DItask);
			DAQmxClearTask((DAQmxDevList[i]).DItask);

			//DO task
			DAQmxStopTask((DAQmxDevList[i]).DOtask);
			DAQmxClearTask((DAQmxDevList[i]).DOtask);

			// CI tasks
			for (j = 0; j < (DAQmxDevList[i]).CIcnt; j++) {
				DAQmxStopTask((DAQmxDevList[i]).CItask[j]);
				DAQmxClearTask((DAQmxDevList[i]).CItask[j]);
			}
			free((DAQmxDevList[i]).CItask);
			free((DAQmxDevList[i]).CItaskDataLen);
			free((DAQmxDevList[i]).CItaskEnable);

			// CO tasks
			for (j = 0; j < (DAQmxDevList[i]).COcnt; j++) {
				DAQmxStopTask((DAQmxDevList[i]).COtask[j]);
				DAQmxClearTask((DAQmxDevList[i]).COtask[j]);
			}
			free((DAQmxDevList[i]).COtask);
			free((DAQmxDevList[i]).COtaskDataLen);
			free((DAQmxDevList[i]).COtaskEnable);
		}
	}
	*/

	//free(AItaskHandle);
	//free(AOtaskHandle);
	//free(DItaskHandle);
	//free(DOtaskHandle);

	cListElem* thisElem = cListFirstElem(NItaskList);
	cListElem *nextElem = cListNextElem (NItaskList, thisElem);
	NItask* thisTask = NULL;

	while(thisElem != NULL) {
		thisTask = (NItask*)thisElem->obj;
		DAQmxErrChk(DAQmxStopTask (thisTask->taskHandler));
		DAQmxErrChk(DAQmxClearTask(thisTask->taskHandler));
		
		if (thisTask->dataBuffer != NULL) {
			free(thisTask->dataBuffer);
		}
		free(thisTask);
		
		cListUnlinkElem(NItaskList, thisElem);
		
		thisElem = nextElem;
		nextElem = cListNextElem(NItaskList, thisElem);
	}
	free(CItaskList);
	free(COtaskList);
	//cListUnlinkAll(NItaskList);
	free(NItaskList);
	AItask = NULL;
	AOtask = NULL;
	DItask = NULL;
	DOtask = NULL;
	CItaskList = NULL;
	COtaskList = NULL;
	//AIpinCount = 0;
	//AOpinCount = 0;
	//DIpinCount = 0;
	//DOpinCount = 0;
	//CIpinCount = 0;
	//COpinCount = 0;

	// Reset library status
	quickDAQSetStatus(STATUS_NASCENT, TRUE);
	DAQmxEnumerated = 0;

	// Free device list memory
	free(DAQmxDevList);
	DAQmxDevList = NULL;
	return 0;
}

#ifdef __cplusplus 
}
#endif 

