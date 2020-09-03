// NI device list.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <NIDAQmx.h>
#include <ansi_c.h>

void main(int argc, char* argv[])
{


int buffersize,n=1;
//Buffer size datatypes
int devicetype_buffersize;
int devicesernum_buffersize;

//Device Info variable initialization
char* devicenames= NULL;
char* devicetype;
int devicesernum;
int is_simulated; 
char * pch; 

//get the size of buffer
buffersize=DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames,devicenames);  
devicenames=(char*)malloc(buffersize);
//devicenames = (char*)malloc(devicenames, sizeof(char)*buffersize);
//Get the string of devicenames in the computer
DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames,devicenames,buffersize);

printf("List of devices in this computer: \n \n");
printf("**********************************************************\n");
printf("Device Name || Device type || Device Serial# || Simulated?\n");
printf("**********************************************************\n");

//Get information about the device
pch = strtok (devicenames,",");

while (pch != NULL)
	{

  	//Get Product Type for a device
	devicetype_buffersize=DAQmxGetDeviceAttribute(pch,DAQmx_Dev_ProductType,NULL);
	devicetype= (char*) malloc(devicetype_buffersize);
	DAQmxGetDeviceAttribute(pch,DAQmx_Dev_ProductType,devicetype,devicetype_buffersize);

	//Get Product Serial Number for the device
	DAQmxGetDeviceAttribute (pch, DAQmx_Dev_SerialNum, &devicesernum,1);
	
	
	//Get Is device simulated? for the device
	DAQmxGetDeviceAttribute (pch, DAQmx_Dev_IsSimulated, &is_simulated, 1);
	if (is_simulated =0)
	printf("%s		%s		%d		no\n",pch,devicetype,devicesernum,is_simulated);
	else
	printf("%s		%s		%d		yes\n",pch,devicetype,devicesernum,is_simulated);

	//Get the next device name in the list
	pch = strtok (NULL, ", "); 
   	n++;
	 }
printf("\n");

}
