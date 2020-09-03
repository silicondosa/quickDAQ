/*********************************************************************
*
* ANSI C Example program:
*    ContWriteDigChan-Burst.c
*
* Example Category:
*    DO
*
* Description:
*    This example demonstrates how to output a continuous digital
*    waveform using burst handshaking mode.
*    Note: This example program exports the sample clock from the
*          device. To import the sample clock, call the
*          DAQmxCfgBurstHandshakingTimingImportClock function
*          instead.
*
* Instructions for Running:
*    1. Select the Physical Channels that correspond to where your
*       signal is output on the device.
*    2. Enter the number of Samples per Buffer. This is the number of
*       samples that will be downloaded to the device every time the
*       DAQmx Write function is called.
*    3. Specify the Sample Clock Rate of the output Waveform.
*    4. Specify the Output Terminal for the Exported Sample Clock.
*    5. Specify the Sample Clock Pulse Polarity. When set to Active
*       High, the data lines will toggle on the rising edge of the
*       sample clock.
*    6. Specify the handshaking parameters. The Ready for Transfer
*       Event will be asserted any time this device is ready to
*       transfer data. The Pause Trigger Polarity tells this device
*       when to pause. If the polarity is set to High, then the
*       device will pause when the corresponding PFI line is high.
*
* Steps:
*    1. Create a task.
*    2. Create one Digital Output channel for each Digital Line in
*       the Task.
*    3. Call the DAQmxCfgBurstHandshakingTimingExportClock function
*       which configures the device for Burst Mode Handshaking, sets
*       the sample clock rate, exports the clock on the specified PFI
*       Line, and sets the sample mode to Continuous.
*    4. Call the Start function to start the task.
*    5. Write the data to the output buffer continuously until the
*       user presses the Stop button.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Connect the Pause Trigger and Ready For Transfer event to the
*    default PFI terminals for the device. The sample clock will be
*    exported to the specified PFI terminal. Make sure your waveform
*    output terminals match the Physical Channel I/O Control.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int main(void)
{
	int         error=0;
	TaskHandle  taskHandle=0;
	uInt32      i=0;
	uInt32      data[1000];
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"PXI1Slot3/port0","",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxCfgBurstHandshakingTimingExportClock(taskHandle,DAQmx_Val_ContSamps,1000,1000.0,"/Dev1/PFI4",DAQmx_Val_ActiveHigh,DAQmx_Val_Low,DAQmx_Val_ActiveHigh));
	for(;i<1000;++i)
		data[i] = i;

	DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));

	DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1000,0,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	printf("Generating digital output continuously. Press Enter to interrupt\n");
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
