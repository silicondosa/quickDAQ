/*********************************************************************
*
* ANSI C Example program:
*    WriteDigChan-ExtClk.c
*
* Example Category:
*    DO
*
* Description:
*    This example demonstrates how to output a finite digital
*    waveform using an external clock.
*
* Instructions for Running:
*    1. Select the Physical Channel to correspond to where your
*       signal is output on the DAQ device.
*    2. Select the Clock Source for the generation.
*    3. Specify the Rate of the output Waveform
*    4. Enter the Waveform Information.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Output Channel.
*    3. Call the DAQmxCfgSampClkTiming function which sets the sample
*       clock rate. Additionally, set the sample mode to Finite.
*    4. Write the waveform to the output buffer.
*    5. Call the Start function to start the task.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminal matches the Physical
*    Channel I/O Control. Also, make sure your external clock
*    terminal matches the Clock Source Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	uInt8       data[1000];
	uInt32      i=0;
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0/line0","",DAQmx_Val_ChanPerLine));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"/Dev1/PFI0",1000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000));

	for(;i<1000;i++)
		data[i] = (uInt8)(i%2);

	/*********************************************/
	// DAQmx Write Code
	/*********************************************/
	DAQmxErrChk (DAQmxWriteDigitalLines(taskHandle,1000,0,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Wait Code
	/*********************************************/
	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandle,10.0));

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
