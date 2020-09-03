/*********************************************************************
*
* ANSI C Example program:
*    ContAcqSamps-IntClk.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to continuously acquire data using
*    the device's internal timing (rate is governed by an internally
*    generated pulse train).
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage ranges.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the sample rate of the acquisiton.
*    Note: The rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*    4. Set the number of samples to read per channel. This will
*       determine how many samples are read at a time. This also
*       determines how many points are plotted on the chart each
*       time.
*    Note: If this value is set too low, the data buffer could
*          overflow.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continous.
*    4. Call the Start function to start acquiring samples.
*    5. Read the waveform data in the EveryNCallback function until
*       the user hits the stop button or an error occurs.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else


int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32		read,totalRead=0;
	float64		data[10];
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",100.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	printf("Acquiring samples continuously. Press Ctrl+C to interrupt\n");
	while( 1 ) {
		/*********************************************/
		// DAQmx Read Code
		/*********************************************/
		DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,10,10.0,DAQmx_Val_GroupByScanNumber,data,10,&read,NULL));

		if( read>0 ) {
			printf("Acquired %d samples. Total %d\r",(int)read,(int)(totalRead+=read));
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
