// quickDAQ.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <quickDAQ.h>

//using namespace std;
int main()
{
	unsigned input;
	printf_s("Hello World! Enter a number (0-9): ");
	input = getchar() - '0';
	printf_s("\nThe number you input was %d", input);
	printf_s("\nPress a key to continue...\n");
	getchar();

	// initialize
	quickDAQinit();

	// configure channels and sample clock
	pinMode(5, ANALOG_IN, 0);
	pinMode(2, ANALOG_OUT, 8);
	pinMode(2, DIGITAL_OUT, 0);
	pinMode(3, CTR_ANGLE_IN, 0);

	setSampleClockTiming(DAQmxSampleMode, DAQmxSamplingRate, DAQmxClockSource, (triggerModes) DAQmxTriggerEdge, DAQmxNumDataPointsPerSample, TRUE);

	// start tasks
	quickDAQstart();

	// read/write data

	//wait for HW timed sample

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