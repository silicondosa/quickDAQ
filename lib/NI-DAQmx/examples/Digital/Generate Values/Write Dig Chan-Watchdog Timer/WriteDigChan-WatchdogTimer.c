/*********************************************************************
*
* ANSI C Example program:
*    WriteDigChan-WatchdogTimer.c
*
* Example Category:
*    DO
*
* Description:
*    This example demonstrates how to write values to a digital
*    output channel while being monitored by a watchdog timer.
*
* Instructions for Running:
*    1. Select the digital lines on the DAQ device to be written.
*    2. Select a value to write.
*    3. Select the device for the watchdog timer task. This should be
*       the same device used for digital output.
*    4. Select the watchdog timeout.
*    5. Select the expiration states.
*    6. Select the time interval between writes.
*
*    Note: If the watchdog timer expires it will stay in the expired
*          state after execution stops. To clear the expiration call
*          device reset or create a watchdog timer task and use the
*          Control Watchdog Task function with a Clear Expiration
*          action. Starting a watchdog task on the device also clears
*          expiration.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Output channel. Use one channel for all
*       lines. You can alternatively use one channel for each line,
*       but then use a different version of the DAQmx Write function.
*    3. Create a Watchdog Timer task.
*    4. Call the Start function to start the digital output task.
*    5. Call the Start function to start the watchdog timer task.
*    6. Write the digital data continuously until the user hits the
*       stop button or an error occurs.
*    7. Use the Control Watchdog Task function with a Reset Timer
*       action to prevent watchdog timer expiration.
*    8. Wait for the specified amount of time to simulate the extra
*       computation the application being monitored must perform.
*    9. Call the Clear Task function to clear the watchdog timer
*       task.
*    10. Call the Clear Task function to clear the digital output
*        task.
*    11. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminals match the Lines I/O
*    Control. In this case wire the item to receive the signal to the
*    first eight digital lines on your DAQ Device.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0,wdTaskHandle=0;
	uInt8       data[8]={0,1,0,1,1,1,1,1};
	char        errBuff[2048]={'\0'};
	int32 		numWritten;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0/line0:7","",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxCreateWatchdogTimerTask("Dev1","wd",&wdTaskHandle,0.01,"Dev1/port0/line0:7",DAQmx_Val_High,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));
	DAQmxErrChk (DAQmxStartTask(wdTaskHandle));

	printf("Continuously writing. Press Ctrl+C to interrupt\n");
	while( 1 ) {
		/*********************************************/
		// DAQmx Write Code
		/*********************************************/
		DAQmxErrChk (DAQmxWriteDigitalLines(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,data,&numWritten,NULL));
		DAQmxErrChk (DAQmxControlWatchdogTask(wdTaskHandle, DAQmx_Val_ResetTimer));
   	}

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);

	/*********************************************/
	// DAQmx Stop Code
	/*********************************************/
	DAQmxClearTask(taskHandle);
	DAQmxClearTask(wdTaskHandle);

	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}
