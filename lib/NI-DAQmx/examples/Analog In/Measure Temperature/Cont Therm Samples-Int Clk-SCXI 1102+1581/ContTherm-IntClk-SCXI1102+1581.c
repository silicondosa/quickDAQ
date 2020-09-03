/*********************************************************************
*
* ANSI C Example program:
*    ContTherm-IntClk-SCXI1102+1581.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire temperature data from a
*    thermistor using the DAQ device's internal clock. This example
*    uses the SCXI 1102 module in conjuction with the SCXI 1581
*    module.
*
* Instructions for Running:
*    1. Select the Physical Channel(s) to correspond to where your
*       signal is input on the SCXI 1102.
*    2. Enter the minimum and maximum Temperature Ranges.
*    Note: For better accuracy try to match the Input Ranges to the
*          expected temperature level of the measured signal.
*    3. Enter the acquisition rate.
*    Note: The Rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*    4. Enter the thermistor parameters (A, B, and C).
*    5. Enter the Resistance Configuration, the current excitation
*       source, and the excitation value in Amps. The Current has
*       been set to 100uA by default for the SCXI-1581 which provides
*       100uA excitation.
*
* Steps:
*    1. Create a task.
*    2. Create an Analog Input Temperature Thermistor Channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    4. Call the Start function to start acquiring samples.
*    5. Read the waveform data in the EveryNCallback function until
*       the user hits the stop button or an error occurs.
*    6. Call the Clear Task function to stop the acquistion.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control. For further connection information, refer
*    to the SCXI-1102 and SCXI-1581 hardware reference manuals.
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
	DAQmxErrChk (DAQmxCreateAIThrmstrChanIex(taskHandle,"SC1Mod1/ai0","",0.0,100.0,DAQmx_Val_DegC,DAQmx_Val_4Wire,DAQmx_Val_External,0.00010,1.2953610000E-3,234.3159000000E-6,101.8703000000E-9));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",1000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

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
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByScanNumber,data,1000,&read,NULL));
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
