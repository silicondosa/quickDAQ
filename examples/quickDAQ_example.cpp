/// See LICENSE.md at root of repository for copyright information
#include <stdio.h>
#include <quickDAQ.h>

#include <math.h> //needed only for the example

int main(void)
{
	//Initializing the quickDAQ library
    quickDAQinit();

	//setting up variables for inputs and outputs
	float64 analogInValues[3] = {0.0, 0.0, 0.0};
	float64 analogOutValues[4] = {0.0, 0.0, 0.0, 0.0};
	float64 counterAngleValues[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
	uInt32 DO1 = 0x000000ff;

	//defining cards and pins
	pinMode(4, ANALOG_IN, 0);
	int i = 0;
	for(i=0; i<4;i++){
		pinMode(1, ANALOG_OUT, i);
	}

	for(i=0; i<3; i++){
		pinMode(2, ANALOG_IN, i);
	}
	
	pinMode(2, DIGITAL_OUT, 0);

	for(i=0; i<5; i++){
		pinMode(3, CTR_ANGLE_IN, i);
	}

	//defining sampling rate and trigger mode
	setSampleClockTiming((samplingModes)HW_CLOCKED, DAQmxSamplingRate, DAQmxClockSource, (triggerModes)DAQmxTriggerEdge, DAQmxNumDataPointsPerSample, TRUE);
	printf("\nIO timeout is %f\n", DAQmxDefaults.IOtimeout);

	float64 AI;

	printf("\n\nPress enter to begin DAQ\n\n");
	getchar();

	//Starting the quickDAQ library
	quickDAQstart();
	syncSampling(); //syncronizing all triggers

	//sending data out via an analog signal
	
	writeDigital(2, &DO1);
	printf("Analog Enabled\n");

	writeAnalog(2, &(analogOutValues[0]));
	printf("Analog signalling has begun\n");

	readAnalog(4, &AI);

	float t = 0.01;
	int j = 0;
	while(t<(2*3.14)){
		for(j=0;j<4;j++){
			analogOutValues[j] = sin(t);
		}
		writeAnalog(1, &(analogOutValues[0]));
		printf("Analog writing out: %1.2f  %1.2f  %1.2f  %1.2f", analogOutValues[0],analogOutValues[1],analogOutValues[2],analogOutValues[3]);

		//reading an analog signal
		readAnalog(2, analogInValues);
		
		for(j=0; j<5; j++){
			//reading a counter angle
			readCounterAngle(3, j, &(counterAngleValues[i]));
		}

		t += 0.01;
		readAnalog(4, &AI);
		syncSampling();
	}

	printf("\nBeginning closing out\n");
	for(i=0;i<4;i++){
		analogOutValues[i] = 0;
	}
	writeAnalog(2, &(analogOutValues[0]));

	DO1 = 0x00000000;
	writeDigital(2, &DO1);
	printf("Analog signalling disabled\n");
	
	syncSampling();
	readAnalog(4, &AI);
	//stopping the quickDAQ library running
	quickDAQstop();
	//deactivating the quickDAQ library
	quickDAQTerminate();

	return 0;
}