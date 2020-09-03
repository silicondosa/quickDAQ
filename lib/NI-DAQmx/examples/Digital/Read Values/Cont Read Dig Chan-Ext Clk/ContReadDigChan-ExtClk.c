/*********************************************************************
*
* ANSI C Example program:
*    ContReadDigChan-ExtClk.c
*
* Example Category:
*    DI
*
* Description:
*    This example demonstrates how to input a continuous digital
*    waveform using an external clock.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Select the Clock Source for the acquistion.
*    3. Set the approximate Rate of the external clock. This allows
*       the internal characteristics of the acquisition to be as
*       efficient as possible. Also set the Samples to Read control.
*       This will determine how many samples are read each time. This
*       also determines how many points are plotted on the graph each
*       iteration.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Input channel.
*    3. Define the parameters for an External Clock Source.
*       Additionally, set the sample mode to be continuous.
*    4. Call the Start function to start the acquisition.
*    5. Read the waveform data continuously until the user hits the
*       stop button or an error occurs.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. Also, make sure your external clock
*    terminal matches the Physical Channel I/O Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32		error=0;
	TaskHandle	taskHandle=0;
	uInt32		data[1000];
	int32		sampsRead,totalRead=0;
	char		errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port0/line0","",DAQmx_Val_ChanPerLine));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"/Dev1/PFI0",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	printf("Acquiring samples continuously. Press Ctrl+C to interrupt\n");
	while( 1 ) {
		/*********************************************/
		// DAQmx Read Code
		/*********************************************/
		DAQmxErrChk (DAQmxReadDigitalU32(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&sampsRead,NULL));

		if( sampsRead>0 ) {
			totalRead += sampsRead;
			printf("Acquired %d samples. Total %d\r",(int)sampsRead,(int)totalRead);
			fflush(stdout);
		}
	}
	printf("\nAcquired %d total samples.\n",(int)totalRead);

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
