#include "stdafx.h"
#include <stdio.h>
#include <cLinkedList.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <quickDAQ.h>
#include <macrodef.h>
#include <string.h>
#include <stdlib.h>

//------------------------------
// EasyDAQmx Glabal Definitions
//------------------------------
quickDAQErrorCodes			quickDAQErrorCode;
int32						NIDAQmxErrorCode;
quickDAQStatusModes			quickDAQStatus;

// NI-DAQmx specific declarations
char						DAQmxDevPrefix[DAQMX_MAX_DEV_STR_LEN] = DAQMX_DEF_DEV_PREFIX;
unsigned int				DAQmxEnumerated = 0;
//long						DAQmxErrorCode = 0;
NIdefaults					DAQmxDefaults;
deviceInfo					*DAQmxDevList = NULL;
unsigned int				DAQmxDevCount = 0;
unsigned int				DAQmxMaxCount = 0;
int32						DAQmxTriggerEdge					= DAQmxDefaults.NItriggerEdge;
samplingModes				DAQmxSampleMode						= (samplingModes) DAQmxDefaults.NIsamplingMode;
float64						DAQmxSamplingRate					= DAQmxDefaults.NIsamplingRate;
uInt64						DAQmxNumDataPointsPerSample			= DAQmxDefaults.NIsamplesPerCh;
char						internal_DAQmxClockSource[DAQMX_MAX_STR_LEN] = DAQMX_SAMPLE_CLK_SRC;

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
		quickDAQTerminate();
		quickDAQSetStatus(STATUS_UNKNOWN, FALSE);
		quickDAQSetError(ERROR_NIDAQMX, TRUE);
		fprintf(ERRSTREAM, "NI-DAQmx Error %ld: %s\n", (long)NIDAQmxErrorCode, errBuff);
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
	int devicesernum_buffersize;

	// Device info linked list
	cLinkedList		*newDevList;
	cListElem		*newDevElem;
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
	for (idx = 0; idx < DAQmxMaxCount + 1; idx++) {
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
	const unsigned bufSize = 15000;
	char data[bufSize];
	char DevIDstring[DAQMX_MAX_DEV_STR_LEN];
	char* rem_data, * oneCh_data;

	dev2string(DevIDstring, myDev);

	switch (IOtype)
	{
	case ANALOG_IN:		// ENUM value 0
		DAQmxGetDevAIPhysicalChans(DevIDstring, data, bufSize);
		break;
	case ANALOG_OUT:	// ENUM value 1
		DAQmxGetDevAOPhysicalChans(DevIDstring, data, bufSize);
		break;
	case DIGITAL_IN:	// ENUM value 2
		DAQmxGetDevDIPorts(DevIDstring, data, bufSize);
		break;
	case DIGITAL_OUT:	// ENUM value 3
		DAQmxGetDevDOPorts(DevIDstring, data, bufSize);
		break;
	case CTR_ANGLE_IN:	// ENUM value 4
		DAQmxGetDevCIPhysicalChans(DevIDstring, data, bufSize);
		break;
	case CTR_TICK_OUT:	// ENUM value 5
		DAQmxGetDevCOPhysicalChans(DevIDstring, data, bufSize);
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
	const unsigned bufSize = 15000;
	char data[bufSize];
	char* rem_data;
	char* oneCh_data;

	dev2string(myDev, deviceNumber);
	NIDAQmxErrorCode = DAQmxGetDevTerminals(myDev, data, bufSize);
	int charLength = (int)strnlen_s(data, bufSize);
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


void setupTaskHandles()
{
	// create task handles for all NI DAQmx tasks within each valid device
	unsigned int i = 0, j = 0;
	if (quickDAQStatus != STATUS_NASCENT) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Library must be reset and enumerated before setting up NI-DAQmx tasks.\n");
		return;
	}
	if (DAQmxEnumerated != 1) {
		fprintf(ERRSTREAM, "QuickDAQ library: Warning: Enumerate NI-DAQmx devices before setting up NI-DAQmx tasks.\n");
		return;
	}

	for (i = 0; i <= DAQmxMaxCount; i++) {
		if ((DAQmxDevList[i]).isDevValid == TRUE) {
			DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).AItask))); //AI task

			DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).AOtask))); //AO task

			DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).DItask))); //DI task

			DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).DOtask))); //DO task

			// CI tasks
			(DAQmxDevList[i]).CItask = (TaskHandle*)malloc(((DAQmxDevList[i]).CIcnt) * sizeof(TaskHandle));
			for (j = 0; j < (DAQmxDevList[i]).CIcnt; j++) {
				DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).CItask[j])));
			}

			// CO tasks
			(DAQmxDevList[i]).COtask = (TaskHandle*)malloc(((DAQmxDevList[i]).COcnt) * sizeof(TaskHandle));
			for (j = 0; j < (DAQmxDevList[i]).COcnt; j++) {
				DAQmxErrChk(DAQmxCreateTask("", &((DAQmxDevList[i]).COtask[j])));
			}
		}
	}


}

void quickDAQinit()
{
	char newPrefix[] = DAQMX_DEF_DEV_PREFIX;
	setDAQmxDevPrefix(newPrefix);
	enumerateNIDevices();
	setupTaskHandles();
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

void setSampleClockTiming(samplingModes sampleMode, float64 samplingRate, char *triggerSource, triggerModes triggerEdge, uInt64 numDataPointsPerSample)
{
	if (quickDAQStatus == STATUS_INIT) {
		DAQmxSampleMode = sampleMode;
		DAQmxSamplingRate = samplingRate;
		strcpy_s(internal_DAQmxClockSource, DAQMX_MAX_STR_LEN, triggerSource);
		DAQmxTriggerEdge = triggerEdge;
		DAQmxNumDataPointsPerSample = numDataPointsPerSample;

		unsigned int i, j, k, isSet;
		for (i = 0; i <= DAQmxMaxCount; i++) {
			deviceInfo* thisDev = &(DAQmxDevList[i]);
			if (thisDev->isDevValid == TRUE) {
				//AI task
				for (k = 0, isSet = 0; k < thisDev->AIcnt && isSet == 0; k++) {
					if (thisDev->AIpins[k].isPinValid != 0) {
						isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->AItask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
					}
				}

				//AO task
				for (k = 0, isSet = 0; k < thisDev->AOcnt && isSet == 0; k++) {
					if (thisDev->AOpins[k].isPinValid != 0) {
						isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->AOtask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
					}
				}

				//DI task
				for (k = 0, isSet = 0; k < thisDev->AIcnt && isSet == 0; k++) {
					if (thisDev->DIpins[k].isPinValid != 0) {
						isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->DItask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
					}
				}
				
				//DO task
				for (k = 0, isSet = 0; k < thisDev->AIcnt && isSet == 0; k++) {
					if (thisDev->DOpins[k].isPinValid != 0) {
						isSet = 1;
						DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->DOtask, DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
					}
				}
				
				// CI tasks
				for (j = 0; j < (DAQmxDevList[i]).CIcnt; j++) {
					DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->CItask[j], DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
				}

				// CO tasks
				for (j = 0; j < (DAQmxDevList[i]).COcnt; j++) {
					DAQmxErrChk(DAQmxCfgSampClkTiming(thisDev->COtask[j], DAQmxClockSource, DAQmxSamplingRate,
														  DAQmxTriggerEdge, DAQmxSampleMode, DAQmxNumDataPointsPerSample));
				}
			}
		}
	}
}



// library run function definitions

// shutdown function definitions
int quickDAQTerminate()
{
	// force shutdown all NI DAQmx tasks for all devices
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

			// CO tasks
			for (j = 0; j < (DAQmxDevList[i]).COcnt; j++) {
				DAQmxStopTask((DAQmxDevList[i]).COtask[j]);
				DAQmxClearTask((DAQmxDevList[i]).COtask[j]);
			}
			free((DAQmxDevList[i]).COtask);
		}
	}

	// Reset library status
	quickDAQSetStatus(STATUS_NASCENT, TRUE);

	// Free device list memory
	free(DAQmxDevList);
	DAQmxDevList = NULL;
	return 0;
}