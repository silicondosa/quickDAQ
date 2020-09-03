/*********************************************************************
*
* ANSI C Example program:
*    Input-8255withDelays.c
*
* Example Category:
*    Handshaking
*
* Description:
*    This example demonstrates how to read a finite number of values
*    from a digital input port using buffered 8255 handshaking and
*    the digital waveform datatype with the Delay after Transfer and
*    Deassert Delay attributes.
*
* Instructions for Running:
*    1. Select the digital lines on the DAQ device to be read.
*    2. Select the number of samples to read.
*    3. Specify the delays.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Input channel.
*    3. Setup the timing for the measurement. In this example we use
*       handshaking to acquire a finite number of samples.
*    4. Setup the delay after transfer. The delay after transfer
*       delays the device from asserting the Handshake Event.
*    5. Setup the delay for deasserting the Handshake Event. The
*       Handshake Event deasserts after the Handshake Trigger asserts
*       and the delay has elapsed.
*    6. Call the Start function to start the acquisition.
*    7. Use the Read function to acquire the data.
*    8. Call the Clear Task function to clear the task.
*    9. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminals match the Port I/O
*    Control on the 653x. Attach the handshaking trigger, STB, to
*    PFI2 if you are using Port0 or to PFI3 if you are using Port2.
*    Attach the handshaking event, IBF, to PFI6 if you are using
*    Port0 or to PFI7 if you are using Port2. See the product manual
*    for further information on how to setup handshaking signal
*    connections.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       numRead;
	uInt8       data[64];
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port0/line0:7","",DAQmx_Val_ChanPerLine));
	DAQmxErrChk (DAQmxCfgHandshakingTiming(taskHandle,DAQmx_Val_FiniteSamps,8));
	DAQmxErrChk (DAQmxSetHshkDelayAfterXfer(taskHandle, 1.0));
	DAQmxErrChk (DAQmxSetExportedHshkEventInterlockedDeassertDelay(taskHandle, 1E-7));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadDigitalLines(taskHandle,8,10.0,DAQmx_Val_GroupByChannel,data,64,&numRead,NULL,NULL));

	printf("Acquired %d samples\n",(int)numRead);

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
