/*********************************************************************
*
* ANSI C Example program:
*    ContThrmcplSamps-IntClk.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to make continuous, hardware-timed
*    temperature measurement using a thermocouple.
*
* Instructions for Running:
*    1. Specify the Physical Channel where you have connected the
*       thermocouple.
*    2. Enter the Minimum and Maximum temperature values in degrees C
*       you expect to measure. A smaller range will allow a more
*       accurate measurement.
*    3. Enter the scan rate at which you want to run the acquisition.
*    4. Specify the type of thermocouple you are using.
*    5. Thermocouple measurements require cold-junction compensation
*       (CJC) to correctly scale them. Specify the source of your
*       cold-junction compensation.
*    6. If your CJC source is "Internal", skip the rest of the steps.
*    7. If your CJC source is "Constant Value", specify the value
*       (usually room temperature) in degrees C.
*    8. If your CJC source is "Channel", specify the CJC Channel
*       name.
*    9. Specify the appropriate Auto Zero Mode. See your device's
*       hardware manual to find out if your device supports this
*       attribute.
*
* Steps:
*    1. Create a task.
*    2. Create a Thermocouple (TC) temperature measurement channel.
*    3. If your device supports Auto Zero Mode, set the AutoZero
*       attribute for all channels in the task.
*    4. Call the Timing function to specify the hardware timing
*       parameters. Use device's internal clock, continuous mode
*       acquisition and the sample rate specified by the user.
*    5. Call the Start function to program and start the acquisition.
*    6. Read N samples and plot it. By default, the Read function
*       reads all available samples, but you can specify how many
*       samples to read at a time and the timeout value. Continue
*       reading data until the stop button is pressed or an error
*       occurs.
*    7. Call the Clear Task function to clear the Task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Connect your thermocouple to the terminals corresponding to the
*    Physical Channel I/O Control value.
*
*********************************************************************/

#include <stdio.h>
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
	DAQmxErrChk (DAQmxCreateAIThrmcplChan(taskHandle,"","",0.0,100.0,DAQmx_Val_DegC,DAQmx_Val_J_Type_TC,DAQmx_Val_BuiltIn,25.0,""));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

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
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,data,1000,&read,NULL));
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
