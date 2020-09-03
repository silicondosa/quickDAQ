/*********************************************************************
*
* ANSI C Example program:
*    ContAccelSamps-IntClk-AnlgStart.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to create an analog input
*    acceleration task and perform a continuous acquisition using
*    option IEPE excitation, analog triggering, and overload
*    detection.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the device.
*    2. Enter the minimum and maximum expected acceleration values.
*    Note: To optimize gain selection, try to match the Input Ranges
*          to the expected level of the measured signal.
*    3. Program the analog input terminal configuration and IEPE
*       excitation settings for your device.
*    4. If your device supports overload detection, check the
*       Overload Detection checkbox. Refer to your device
*       documentation to see if overload protection is supported.
*    5. Set the rate of the acquisition. Also set the Samples to Read
*       control. This will determine how many samples are read at a
*       time. This also determines how many points are plotted on the
*       graph each time.
*    Note: The rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*    6. Set the source of the Analog Edge Start Trigger. By default
*       this is Dev1/ai0.
*    7. Set the slope and level of desired analog edge condition.
*    8. Set the Hysteresis Level.
*    9. Input the sensitivity and units for your accelerometer.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input acceleration channel. This step
*       defines accelerometer sensitivity, desired range, and IEPE
*       excitation.
*    3. Set the sample rate and define a continuous acquisition.
*    4. Define the trigger channel, trigger level, rising/falling
*       edge, and hysteresis window for an analog start trigger.
*    5. Call the Start function to start the acquisition.
*    6. Read the waveform data in the EveryNCallback function until
*       the user hits the stop button or an error occurs.
*    7. Check for overloaded channels.
*    8. Call the Clear Task function to clear the Task.
*    9. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. Also, make sure your analog trigger
*    terminal matches the Trigger Source Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIAccelChan(taskHandle,"Dev1/ai0","",DAQmx_Val_PseudoDiff,-100.0,100.0,DAQmx_Val_AccelUnit_g,50,DAQmx_Val_mVoltsPerG,DAQmx_Val_Internal,0.004,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
	DAQmxErrChk (DAQmxCfgAnlgEdgeStartTrig(taskHandle,"Dev1/ai0",DAQmx_Val_Rising,30.0));
	DAQmxErrChk (DAQmxSetAnlgEdgeStartTrigHyst(taskHandle, 10.0));

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
	/* Change this variable to 1 if you are using a DSA device and want to check for Overloads. */
	int32       overloadDetectionEnabled=0;
	bool32      overloaded=0;
	char        overloadedChannels[1000];

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByScanNumber,data,1000,&read,NULL));
	if ( overloadDetectionEnabled ) {
		DAQmxErrChk (DAQmxGetReadOverloadedChansExist(taskHandle,&overloaded));
	}

	if( read>0 )
		printf("Acquired %d samples. Total %d\r",(int)read,(int)(totalRead+=read));
	if( overloaded ) {
		DAQmxErrChk (DAQmxGetReadOverloadedChans(taskHandle,overloadedChannels,1000));
		printf("Overloaded channels: %s\n",overloadedChannels);
	}
	fflush(stdout);

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
