/*********************************************************************
*
* ANSI C Example program:
*    Output-8255withDelays.c
*
* Example Category:
*    Handshaking
*
* Description:
*    This example demonstrates how to write a finite number of
*    waveforms to a digital output port using buffered finite 8255
*    handshaki It displays the data to be output on a graph.
*
* Instructions for Running:
*    1. Select the digital port on the DAQ device to be written.
*    2. Select the number of samples to write.
*    Note: This example outputs a binary counter from 0 to Number of
*          Samples - 1.
*    3. Specify the delays
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Output channel.
*    3. Setup the timing for the measurement. In this example we use
*       handshaking to generate a finite number of samples.
*    4. Setup the delay after transfer. The delay after transfer
*       delays the device from asserting the Handshake Event.
*    5. Setup the delay for deasserting the Handshake Event. The
*       Handshake Event deasserts after the Handshake Trigger asserts
*       and the delay has elapsed.
*    6. Create digital data to output.
*    7. Use the Write function to write the data. The auto start
*       parameter is set to False, so the Start function must be
*       called explicitly to begin the generation.
*    8. Call the Start function to start the generation.
*    9. Call the Clear Task function to clear the task.
*    10. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminals match the Port I/O
*    Control on the 653x. Attach the handshaking trigger, ACK, to
*    PFI2 if you are using Port0 or to PFI3 if you are using Port2.
*    Attach the handshaking event, OBF, to PFI6 if you are using
*    Port0 or to PFI7 if you are using Port2. See the product manual
*    for further information on how to setup handshaking signal
*    connections.
*
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else


int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	uInt8       data[64]={1,1,1,1,1,1,1,1,
						  0,0,1,1,1,1,1,1,
						  0,0,0,0,1,1,1,1,
						  0,0,0,0,0,0,1,1,
						  1,1,0,0,0,0,0,0,
						  1,1,1,1,0,0,0,0,
						  1,1,1,1,1,1,0,0,
						  1,1,1,1,1,1,1,1};
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0/line0:7","",DAQmx_Val_ChanPerLine));
	DAQmxErrChk (DAQmxCfgHandshakingTiming(taskHandle,DAQmx_Val_FiniteSamps,8));
	DAQmxErrChk (DAQmxSetHshkDelayAfterXfer(taskHandle, 1.0));
	DAQmxErrChk (DAQmxSetExportedHshkEventInterlockedDeassertDelay(taskHandle, 1E-7));

	/*********************************************/
	// DAQmx Write Code
	/*********************************************/
	DAQmxErrChk (DAQmxWriteDigitalLines(taskHandle,8,0,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Wait Code
	/*********************************************/
	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandle,10.0));

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
