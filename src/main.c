// quickDAQ.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <Windows.h>
#include <quickDAQ.h>
#include <stdlib.h>
#include <time.h>

//using namespace std;
int main()
{
/*
	unsigned input;
	printf_s("Hello World! Enter a number (0-9): ");
	input = getchar() - '0';
	printf_s("\nThe number you input was %d", input);
	printf_s("\nPress a key to continue...\n");
	getchar();
*/

	// initialize
	srand((unsigned)time(0));
	quickDAQinit();

	// configure channels and sample clock
	pinMode(5, ANALOG_IN, 0);
	for (int i = 0; i < 12; i++) {
		pinMode(2, ANALOG_OUT, i);
	}
	pinMode(2, ANALOG_OUT, 16);
	pinMode(2, ANALOG_OUT, 17);
	pinMode(2, ANALOG_OUT, 18);
	pinMode(2, ANALOG_OUT, 19);

	pinMode(2, DIGITAL_OUT, 0);

	for (int i = 0; i < 8; i++) {
		pinMode(3, CTR_ANGLE_IN, i);
	}

	setSampleClockTiming((samplingModes) HW_CLOCKED/*DAQmxSampleMode*/, DAQmxSamplingRate, DAQmxClockSource, (triggerModes) DAQmxTriggerEdge, DAQmxNumDataPointsPerSample, TRUE);

	printf("\nIO timeout is %f\n", DAQmxDefaults.IOtimeout);

	// read/write data
	float64 AI;
	float64 mtrCmd[16] = {0,0,0, 0,0,0, 0,0,0, 0,0,0,	0,0,0,0};
	const float64 muscleTone = 0.2;
	uInt32			DO1 = 0x000000ff;
	const float64	DO2[4] = {5,5,5,5};
	float64 CI[8] = { 0,0, 0,0, 0,0, 0,0 };

	//printf("\nData to be written: AO: %lf, DO: %lX\n", (double)AO, DO);
	printf("Press enter to start control of hand");
	getchar();

	// start tasks
	quickDAQstart();


		syncSampling(); //wait for HW timed sample

		/*
		readAnalog(5, &AI);
		printf("pass AI\n");
		
		writeAnalog(2, &AO);
		printf("pass AO\n");
		
		writeDigital(2, &DO);
		printf("pass DO\n");
		
		readCounterAngle(3, 0, &CI);
		printf("\nData read: CI: %lf\n", (double)CI);
		*/

		readAnalog(5, &AI);
		for (int i = 16; i < 20; i++) {
			mtrCmd[i] = DO2[i - 16];
		}
		writeDigital(2, &DO1);
		writeAnalog(2, &(mtrCmd[0]));
		printf("Motor Enabled\n");

		for (int i = 0; i < 12; i++) {
			mtrCmd[i] = muscleTone;
		}
		writeAnalog( 2, &(mtrCmd[0]) );
		printf("Motor Wound up\n");

		// Control Loop
		for (unsigned long t = 0; t < 10000; t++) {
			syncSampling();
			readAnalog(5, &AI);
			if (t % 1000 == 0) {
				for (int i = 0; i < 12; i++) {
					mtrCmd[i] = (float((rand()%8)) / 10) + muscleTone;
				}
				writeAnalog(2, &(mtrCmd[0]));
				printf("\n\nMOTOR: %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n\n",
					mtrCmd[0], mtrCmd[1], mtrCmd[2], mtrCmd[3], mtrCmd[4], mtrCmd[5], mtrCmd[6], mtrCmd[7], mtrCmd[8], mtrCmd[9], mtrCmd[10], mtrCmd[11]);
			}
			for (int j = 0; j < 8; j++) {
				readCounterAngle(3, j, &(CI[j]));
			}
			printf("ANGLE: %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\r",
				CI[0], CI[1], CI[2], CI[3], CI[4], CI[5], CI[6], CI[7]);
		}

		syncSampling();
		readAnalog(5, &AI);
		for (int i = 0; i < 16; i++) {
			mtrCmd[i] = 0;
		}
		writeAnalog(2, &(mtrCmd[0]));
		printf("Motor wound down\n");
		
		DO1 = 0x00000000;
		writeDigital(2, &(DO1) );
		printf("Motor disabled\n\n");

		syncSampling();

	// end tasks
	quickDAQstop();

	// Terminate library
	quickDAQTerminate();
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file