/*********************************************************************
*
* ANSI C Example program:
*    ContWriteDigChan-PipeSampClkwHshk.c
*
* Example Category:
*    DO
*
* Description:
*    This examples demostrates how to interface the NI 6536/7 to a
*    synchonous FIFO.
*
* Instructions for Running:
*    1. Select the Physical Channels that correspond to where your
*       signal is output on the device.
*    2. Enter the number of Samples per Buffer. This is the number of
*       samples that will be downloaded to the device every time the
*       DAQmx Write function is called.
*    3. Specify the Sample Clock Rate of the output waveform.
*    4. Specify the Output Terminal for the Exported Sample Clock.
*    5. Specify the Sample Clock Pulse Polarity. When set to Active
*       High, the data lines will toggle on the rising edge of the
*       sample clock.
*    6. Specify the handshaking parameters. The Data Active Event
*       will be asserted when a valid sample is clocked out.The Pause
*       Trigger Polarity tells this device when to pause. If the
*       polarity is set to High, then the device will pause when the
*       corresponding PFI line is high. Note, that the device will
*       not pause on the next sample clock edge because of
*       pipelining.
*
* Steps:
*    1. Create a task.
*    2. Create one Digital Output channel for each Digital Line in
*       the Task.
*    3. Call the DAQmxCfgPipelinedSampClkTiming function which
*       configures the device for Pipelined Sample Clock.
*    4. Configure the pause trigger.
*    5. Configure the exported sample clock and data active event.
*    6. Configure the hardware to pause if the onboard memory becomes
*       empty.
*    7. Disallow Regeneration. When regeneration is disallowed, the
*       data transfer between the device and the DAQmx buffer will
*       pause when the device has emptied this buffer. It will resume
*       when more data has been written into the buffer.
*    8. Write the waveform to the output buffer.
*    9. Call the Start function to start the task.
*    10. Write the data to the output buffer continuously.
*    11. Call the Clear Task function to clear the Task.
*    12. Display an error if any.
*
* I/O Connections Overview:
*    Connect the FIFO's Almost Full Flag to the Pause Trigger.
*    Connect the FIFO's Write Enable signal to the Data Active Event.
*    Connect the FIFO's Writw Clock to the exported sample clock
*    terminal. Connect the data lines from the NI 6536/7 to the data
*    lines of the FIFO.
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
	DAQmxErrChk (DAQmxCfgPipelinedSampClkTiming(taskHandle,"",100000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
	DAQmxErrChk (DAQmxSetPauseTrigType(taskHandle,DAQmx_Val_DigLvl));
	DAQmxErrChk (DAQmxSetDigLvlPauseTrigSrc(taskHandle,"/Dev1/PFI1"));
	DAQmxErrChk (DAQmxSetDigLvlPauseTrigWhen(taskHandle,DAQmx_Val_High));
	DAQmxErrChk (DAQmxSetExportedSampClkOutputTerm(taskHandle,"/Dev1/PFI4"));
	DAQmxErrChk (DAQmxSetExportedSampClkPulsePolarity(taskHandle,DAQmx_Val_ActiveHigh));
	DAQmxErrChk (DAQmxSetExportedDataActiveEventLvlActiveLvl(taskHandle,DAQmx_Val_ActiveLow));
	DAQmxErrChk (DAQmxSetExportedDataActiveEventOutputTerm(taskHandle,"/Dev1/PFI0"));
	DAQmxErrChk (DAQmxSetSampClkUnderflowBehavior(taskHandle,DAQmx_Val_PauseUntilDataAvailable));
	DAQmxErrChk (DAQmxSetWriteRegenMode(taskHandle,DAQmx_Val_DoNotAllowRegen));

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
