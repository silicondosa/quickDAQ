#include "stdafx.h"
#include <stdio.h>
#include <cLinkedList.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <quickDAQ.h>
#include <macrodef.h>
#include <string.h>

//-----------------------------
// EasyDAQmx Glabal Definitions
//-----------------------------
char DAQmxDevPrefix[DAQMX_MAX_DEV_STR_LEN];

unsigned int				EasyDAQmxFirstEnumerate = 1;
long						DAQmxErrorCode = 0;

// Library support function definitions
inline char* setDAQmxDevPrefix(char *newPrefix)
{
	return DAQmxDevPrefix;
}


inline char* dev2string(char* strBuf, unsigned int devNum)
{
	snprintf(strBuf, 1 + DAQMX_MAX_DEV_STR_LEN, "%s%d", DAQmxDevPrefix, devNum);
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
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "port");
		break;
	case DIGITAL_OUT:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "port");
		break;
	case CTR_ANGLE_IN:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "ctr");
		break;
	case CTR_TICK_OUT:
		snprintf(pinType, DAQMX_MAX_PIN_STR_LEN, "ctr");
		break;
	default:
		fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Invalid I/O type requested.\n");
		break;
	}
	snprintf(strbuf, DAQMX_MAX_STR_LEN, "%s%d/%s%d", DAQmxDevPrefix, devNum, pinType, pinNum);
	return strbuf;
}void enumerateNIDevices()
{

}