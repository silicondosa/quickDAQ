/*********************************************************************
*
* ANSI C Example program:
*    Acq-ExtClk.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to Acquire a Finite amount of data
*    using an external clock.
*
* Instructions for Running:
*    1. Select the Physical Channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the Minimum and Maximum Voltage Ranges.
*    Note: For better accuracy try to match the Input Ranges to the
*          expected voltage level of the measured signal.
*    3. Select the Source of the Clock for the acquisition.
*    4. Select how many Samples to Acquire on Each Channel.
*    5. Set the approximate Rate of the external clock. This allows
*       the internal characteristics of the acquisition to be as
*       efficient as possible.
*    Note: The Rate should be AT LEAST twice as fast as the maximum
*          frequency component of the signal being acquired.
*
* Steps:
*    1. Create a task.
*    2. Create an Analog Input Voltage channel.
*    3. Define the parameters for an External Clock Source.
*       Additionally, define the sample mode to be finite.
*    4. Call the Start function to begin the acquisition.
*    5. Use the Read function to Measure Multiple Samples from N
*       Channels on the Data Acquisition Card. Set a timeout so an
*       error is returned if the samples are not returned in the
*       specified time limit.
*    6. Call the Clear Task function to clear the task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control. Also, make sure your external clock
*    terminal matches the Clock Source Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       read;
	float64     data[1000];
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"/Dev1/PFI7",1000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,-1,10.0,DAQmx_Val_GroupByChannel,data,1000,&read,NULL));

	if( read>0 )
		printf("Acquired %d samples\n",(int)read);

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
