/*********************************************************************
*
* ANSI C Example program:
*    AcqWheatstoneBridge9237.c
*
* Example Category:
*    AI
*
* Description:
*    This example performs Wheatstone Bridge measurements with offset
*    nulling if desired.
*
* Instructions for Running:
*    1. Enter the list of physical channels, and set the attributes
*       of the bridge configuration connected to all the channels.
*       The 'Maximum Value' and 'Minimum Value' inputs specify the
*       range, in Custom Scale units, that you expect of your
*       measurements.
*    2. Make sure your Bridge sensor is in its relaxed state.
*    3. You may turn on the 'Do Offset Null?' option to automatically
*       null out your offset by performing a hardware nulling
*       operation (if supported by the hardware) followed by a
*       software nulling operation. (NOTE: The software nulling
*       operation will cause a loss in dynamic range while a hardware
*       nulling operation will not cause any loss in the dynamic
*       range).
*    4. Specify Sensor Scaling Parameters. You can choose a Linear
*       Scale or Map Ranges Scale. The channel Maximum and Minimum
*       values are specified in terms of the scaled units.
*    5. Run the example and do not disturb your bridge sensor until
*       data starts being plotted.
*
* Steps:
*    1. Create custom scale.
*    2. Create a task.
*    3. Create a Voltage with Excitation channel.
*    4. Set timing parameters. Note that sample mode set to
*       Continuous Samples.
*    5. If nulling is desired, call the DAQmx Perform Bridge Offset
*       Nulling Calibration function to perform both hardware nulling
*       (if supported) and software nulling.
*    6. Call the Start function to start the acquisition.
*    7. Read the data in the EveryNCallback function until the user
*       hits the stop button or an error occurs.
*    8. Call the Clear Task function to clear the Task.
*    9. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. For more detailed connection information
*    and bridge calibration procedures refer to your NI 9237s
*    module's hardware reference manual.
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
	const char	*const customScaleName="Acq Wheatstone Bridge Samples Scale";
	const char	*const scaledUnits="psi";
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateLinScale(customScaleName,1000.0,0.0,DAQmx_Val_Volts,scaledUnits));
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChanWithExcit(taskHandle,"cDAQ1Mod1/ai0","",DAQmx_Val_Cfg_Default,-0.025,0.025,DAQmx_Val_FromCustomScale,DAQmx_Val_FullBridge,DAQmx_Val_Internal,2.5,1,customScaleName));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",5000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,5000));
	// If nulling is desired, call the DAQmx Perform Bridge Offset Nulling Calibration function to perform both hardware nulling (if supported) and software nulling.
	DAQmxErrChk (DAQmxPerformBridgeOffsetNullingCal(taskHandle,""));

	DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,5000,0,EveryNCallback,NULL));
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
