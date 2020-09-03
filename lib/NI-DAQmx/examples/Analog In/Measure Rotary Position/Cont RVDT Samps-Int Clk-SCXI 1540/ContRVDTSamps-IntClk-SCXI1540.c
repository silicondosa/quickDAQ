/*********************************************************************
*
* ANSI C Example program:
*    ContRVDTSamps-IntClk-SCXI1540.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to make continuous, hardware-timed
*    acceleration measurement using a SCXI-1540 module.
*
* Instructions for Running:
*    1. Specify the Physical Channel where you have connected the
*       RVDT.
*    2. Enter the Minimum and Maximum distance values, in units based
*       on the units control, you expect to measure. A smaller range
*       will allow a more accurate measurement.
*    3. Select the number of samples to acquire.
*    4. Set the rate of the acquisition
*    5. Specify the RVDT settings.
*    6. If you are using multiple RVDTs and would like to synchronize
*       their excitations, then enable synchronization for all the
*       secondary RVDT channels via the Synchronization Enabled
*       button. You must also connect the excitation output (EX+) of
*       your primary RVDT channel to all the secondary RVDT channel's
*       sync pin (SYNC).
*
* Steps:
*    1. Create a task.
*    2. Create an analog input RVDT channel.
*    3. Configure the synchronization of the SCXI-1540 module.
*    4. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    5. Call the Start function to start the acquisition.
*    6. Read the waveform data in the EveryNCallback function until
*       the user hits the stop button or an error occurs.
*    7. Call the Clear Task function to clear the Task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Connect your RVDT to the terminals corresponding to the Physical
*    Channel I/O Control value. The excitation lines connect to EX+
*    and EX- while the analog input lines connect to CH+ and CH-. If
*    you have set the Synchronization Enabled attribute, you must
*    connect the excitation output (EX+) of your primary RVDT channel
*    to all secondary RVDT channel's sync pin (SYNC).
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
	DAQmxErrChk (DAQmxCreateAIPosRVDTChan(taskHandle,"Dev1/ai0","",-70.0,70.0,DAQmx_Val_Degrees,50.0,DAQmx_Val_mVoltsPerVoltPerDegree,DAQmx_Val_Internal,1.0,2500,DAQmx_Val_4Wire,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"OnboardClock",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
	DAQmxErrChk (DAQmxSetAIACExcitSyncEnable(taskHandle, "", 0));

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
