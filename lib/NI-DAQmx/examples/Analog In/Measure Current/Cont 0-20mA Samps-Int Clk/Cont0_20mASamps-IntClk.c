/*********************************************************************
*
* ANSI C Example program:
*    Cont0_20mASamps-IntClk.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to continuously measure current
*    using an internal hardware clock for timing.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum current ranges, in Amps.
*    Note: For better accuracy try to match the input ranges to the
*          expected current level of the measured signal.
*    3. Set the rate of the acquisition. Higher values will result in
*       faster updates, approximately corresponding to samples per
*       second. Also, set the number of samples to read at a time.
*       This will correspond to how many samples are shown on the
*       graph at once.
*    4. Enter in the parameters of your current shunt resistor. The
*       shunt resistor location will usually be "External" unless you
*       are using an SCXI current input terminal block or SCC current
*       input module. The shunt resistor value should correspond to
*       the shunt resistor that you are using, and is specified in
*       ohms. If you are using an SCXI current input terminal block
*       or SCC current input module, you must select "Internal" for
*       the shunt resistor location.
*
* Steps:
*    1. Create an analog input current channel.
*    2. Create a new Task and Setup Timing.
*    3. Use the DAQmxReadAnalogF64 to measure multiple samples from
*       multiple channels on the data acquisition card. Set a timeout
*       so an error is returned if the samples are not returned in
*       the specified time limit.
*    4. Call the Clear Task function to clear the Task.
*    5. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the physical
*    channel I/O control. If you are using an external shunt
*    resistor, make sure to hook it up in parallel with the current
*    signal you are trying to measure.
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
	DAQmxErrChk (DAQmxCreateAICurrentChan(taskHandle,"SC1Mod1/ai0","",DAQmx_Val_Cfg_Default,0.0,0.02,DAQmx_Val_Amps,DAQmx_Val_Default,249.0,""));
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
		printf("DAQmx Error %s",errBuff);
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
