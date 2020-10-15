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
long						NIDAQmxErrorCode;
quickDAQStatusModes			quickDAQStatus;

// NI-DAQmx specific declarations
char						DAQmxDevPrefix[DAQMX_MAX_DEV_STR_LEN];
unsigned int				DAQmxEnumerated = 0;
long						DAQmxErrorCode = 0;
NIdefaults					DAQmxDefaults;
deviceInfo					*DAQmxDevList = NULL;
unsigned int				DAQmxDevCount = 0;
unsigned int				DAQmxMaxCount = 0;

//-------------------------------
// quickDAQ Function Definitions
//-------------------------------
// support functions
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
			fprintf(ERRSTREAM, "QuickDAQ library: ERROR %d: NI-DAQmx has generated error code %l.\n", (int)newError, NIDAQmxErrorCode);
			char NIerrorString[1000];
			DAQmxGetErrorString(NIDAQmxErrorCode, NIerrorString, sizeof(NIerrorString));
			fprintf(ERRSTREAM, "QuickDAQ library: NI-DAQmx Error %l: %s\n", NIDAQmxErrorCode, NIerrorString);
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
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS NASCENT (code %d): quickDAQ is nascent and uninitialized.", (int)newStatus);
		break;
	case STATUS_INIT:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS INITIALIZED (code %d): quickDAQ has been initialized and ready to configure with active I/O.", (int)newStatus);
		break;
	case STATUS_READY:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS READY (code %d): quickDAQ is ready to run. I/O, sync, clock, and trigger resources reserved.", (int)newStatus);
		break;
	case STATUS_RUNNING:  
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS RUNNING (code %d): quickDAQ is running and data is being collected now.", (int)newStatus);
		break;
	case STATUS_SHUTDOWN: 
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS SHUTDOWN (code %d): quickDAQ has been stopped and resources freed.", (int)newStatus);
		break;
	default:
		newStatus = STATUS_UNKNOWN;
		if (printFlag != 0) fprintf(ERRSTREAM, "QuickDAQ library: STATUS UNKNOWN (code %d): quickDAQ in an unknown status mode.", (int)newStatus);
		break;
	}
	quickDAQStatus = newStatus;
	return (int)quickDAQStatus;
}


// initialization function definitions
inline char* setDAQmxDevPrefix(char* newPrefix)
{
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
	printf("List of devices in this computer: \n \n");
	printf("***************************************************************************\n");
	printf("Device Number || Device Name || Device type || Device Serial# || Simulated?\n");
	printf("***************************************************************************\n");

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
		newDev->devNum = strtol(&(devName[8]), NULL, 10);

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

		// TODO Copy above info into devInfo structure
			// also enumerate and copy channel counts
		
		if (is_simulated == 0)
			printf("%u		%s		%s		%ld		No\n" , newDev->devNum, newDev->devName, newDev->devType, newDev->devSerial);
		else
			printf("%u		%s		%s		%ld		Yes\n", newDev->devNum, newDev->devName, newDev->devType, newDev->devSerial);

	}
	printf("***************************************************************************\n");
	printf("\n");

	// Create array of NI devices for fast access and clear linked list memory at the same time
	DAQmxDevList = (deviceInfo*) malloc( sizeof(deviceInfo) * (DAQmxMaxCount+1) );
		// set default validity of all elements in the array to 0
	unsigned idx;
	for (idx = 0; idx < DAQmxMaxCount + 1; idx++) {
		DAQmxDevList[idx].isDevValid = 0;
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

	for (oneCh_data = strtok_s(rem_data, ",", &rem_data); oneCh_data != NULL; oneCh_data = strtok_s(rem_data, ",", &rem_data), i++) {
		fprintf(LOGSTREAM, "Terminal %d: %s\n", i + 1, oneCh_data);
	}

	fprintf(LOGSTREAM, "\n\n %s - %d Terminals (%d characters)\n\n", myDev, i, charLength);

	return i;
}

// configuration function definitions

// library run function definitions

// shutdown function definitions
int quickDAQTerminate()
{
	// force shutdown all NI DAQmx tasks
	
	// Reset library status
	

	// Free device list memory
	free(DAQmxDevList);
	DAQmxDevList = NULL;
	return 0;
}