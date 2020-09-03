/*********************************************************************
*
* ANSI C Example program:
*    ContFreq_IntClk_SCXI1126.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire frequency data from an
*    SCXI-1126 using the DAQ device's internal clock.
*
* Instructions for Running:
*    1. Select the physical channel that corresponds to a channel on
*       your SCXI-1126.
*    2. Enter the minimum and maximum frequency ranges.
*    Note: For better accuracy, try to match the input range to the
*          expected frequency level of the measured signal.
*    3. Enter the sample rate for the hardware-timed acquisition.
*       Also set the Samples to Read control. This will determine how
*       many samples are read at a time. This also determines how
*       many points are plotted on the graph each time.
*    4. Enter the Level and Hysteresis of the triggering window.
*    Note: Triggering window is defined as (Threshold-Hysteresis)
*    to Threshold Level and must be between -0.5 and 4.48
*
* Steps:
*    1. Create an Frequency Input Voltage channel.
*    2. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    3. Set the value of the Cutoff Frequency for the Low pass filter
*       on the SCXI-1126 module.
*    4. Call the Start function to start the acquisition.
*    5. Read the waveform data in the EveryNCallback function until
*       the user hits the stop button or an error occurs.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control. For further connection information, refer
*    to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int main(void)
{
	int         error=0;
	char        errBuff[2048]={'\0'};
	TaskHandle  taskHandle=0;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIFreqVoltageChan(taskHandle,"SC1Mod1/ai0","",1.0,1000.0,DAQmx_Val_Hz,0.0,0.2,""));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
	DAQmxErrChk (DAQmxSetAILowpassCutoffFreq(taskHandle, "", 1.0));

	DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL));
	DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	printf("Acquiring samples continuously. Press Enter to interrupt\n");
	getchar();

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

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32       error=0;
	char        errBuff[2048]={'\0'};
	static int  totalRead=0;
	int32       read=0;
	float64     data[1000];

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,10000,10.0,DAQmx_Val_GroupByScanNumber,data,10000,&read,NULL));
	if( read>0 ) {
		printf("Acquired %d samples. Total %d\r",(int)read,(int)(totalRead+=read));
		fflush(stdout);
	}

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}
