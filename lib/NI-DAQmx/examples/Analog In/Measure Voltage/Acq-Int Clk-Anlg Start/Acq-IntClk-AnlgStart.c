/*********************************************************************
*
* ANSI C Example program:
*    Acq-IntClk-AnlgStart.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire a finite amount of data
*    using the DAQ device's internal clock, started by an analog edge
*    condition.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage range.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the number of samples to acquire per channel.
*    4. Set the rate of the acquisiton.
*    Note: The rate should be AT LEAST twice as fast as the maximum
*          frequency component of the signal being acquired.
*    5. Set the source of the start trigger. By default this is
*       analog input channel 0.
*    6. Set the slope and level of desired analog edge condition.
*    7. Set the Hysteresis Level.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be finite.
*    4. Define the parameters for an Analog Slope Start Trigger.
*    5. Call the Start function to start the acquistion.
*    6. Read all of the waveform data.
*    7. Call the Clear Task function to stop the acquistion.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control. In this case wire your signal to the ai0
*    pin on your DAQ Device. By default, this will also be the signal
*    used as the analog start trigger.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       read;
	float64     data[1000];
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000));
	DAQmxErrChk (DAQmxCfgAnlgEdgeStartTrig(taskHandle,"APFI0",DAQmx_Val_Rising,1.0));
	DAQmxErrChk (DAQmxSetAnlgEdgeStartTrigHyst(taskHandle, 1.0));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,-1,10.0,DAQmx_Val_GroupByChannel,data,1000,&read,NULL));

	if( read>1 )
		printf("Acquired %d samples\n",(int)read);

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
