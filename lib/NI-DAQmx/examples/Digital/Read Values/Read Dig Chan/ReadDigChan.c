/*********************************************************************
*
* ANSI C Example program:
*    ReadDigChan.c
*
* Example Category:
*    DI
*
* Description:
*    This example demonstrates how to read values from one or more
*    digital input channels.
*
* Instructions for Running:
*    1. Select the digital lines on the DAQ device to be read.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Input channel. Use one channel for all
*       lines.
*    3. Call the Start function to start the task.
*    4. Read the digital data. This read function reads a single
*       sample of digital data on demand, so no timeout is necessary.
*    5. Call the Clear Task function to clear the Task.
*    6. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminals match the Lines I/O
*    Control. In this case wire your digital signals to the first
*    eight digital lines on your DAQ Device.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32		error=0;
	TaskHandle	taskHandle=0;
	uInt8		data[100];
	char		errBuff[2048]={'\0'};
	int32		i;
	int32		read,bytesPerSamp;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port0/line0:7","",DAQmx_Val_ChanForAllLines));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadDigitalLines(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,data,100,&read,&bytesPerSamp,NULL));

	// assuming 8 channels acquired
	for(i=0;i<8;++i)
		printf("Data acquired, channel %d: 0x%X\n",(int)i,data[i]);
		
Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}
