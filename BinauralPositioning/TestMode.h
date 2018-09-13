//TestMode: Gabriella Watkins U1154477

#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include "Wavfile\WavFile.h"
#include "kiss_fft130\kiss_fftr.h"
#include "AudioStream.h"

#ifndef TESTMODE_H
#define TESTMODE_H

const int WINDOW_SIZE = 1024;								//Size of each processing frame
const int AUDIO_SEGMENT = 512;								//Amount of audio data in each frame
const int FILTER_SIZE = WINDOW_SIZE - AUDIO_SEGMENT + 1;	//Filter size needed to satisfy correct data length after convolution
const int OVERLAP = WINDOW_SIZE - AUDIO_SEGMENT;			//Amount of samples that need to be overlapped
const float PI = 3.1415926535897932385f;
const int ADDR_LENGTH = 19;									//Number of elements in the char array holding the file address

using namespace std;

class TestMode
{
private:

	//all relevent parameters needed to process hrir's
	struct hrirData
	{
		CWavFile			wav;
		char				addr[ADDR_LENGTH];
		kiss_fft_scalar		Freq[WINDOW_SIZE];
		kiss_fft_cpx		FFT[WINDOW_SIZE];
	};

	//all relevent parameters needed to process snare hit
	struct audioData
	{
		CWavFile			wav;
		char				*addr;
		int					length;
		kiss_fft_scalar		Freq[WINDOW_SIZE];
		kiss_fft_cpx		FFT[WINDOW_SIZE];
		kiss_fft_cpx		TEMP[WINDOW_SIZE];
	};

	AudioStream stream;			//Instance of AudioStream class
	hrirData left, right;		//left/right hrir data - struct
	audioData ch1, ch2;			//left/right audio data - struct
	int paddedL;				//length of audio file so that it is divisible by frame size (zero padded)
	int frameLength;			//length of a single hit at a single position
	int finalLength;			//length of stream output (one stream per elevation), 
								//This makes it much easier to keep track of where in the test you are

								//saves overlap samples from frame to add to next frame
	vector<vector<float>> overlap;
	//vector for mono audio file
	vector<float> audioSample;
	//Left, right and interleaved output
	vector<vector<float>> out;

public:

	TestMode();
	~TestMode();

	//Take samples from wav file, move to float array and zero pad the array so it divides evenly by AUDIO_SEGMENT
	void initialiseAudio(CWavFile &audio, vector<float> &audioSample, int &lengthA, int &length);

	//Take samples from hrir wav, assign to a float array, zero pad the array to WINDOW_SIZE to leave room for the convolved data
	//Copy sample data from float array to scalar array for kiss_fft function
	//Convert data to frequency domain, free kiss_fftr_cfg fft
	void filterProcessing(CWavFile &hrir, float freq[], kiss_fft_cpx filterFFT[]);

	//Take the addresses found by addressVectorAssign, convert to char array for use by CWavFile
	//Open the wav file, pass to filterProcessing
	void loadFilters(vector<string> &address);

	//Index of all file addresses, returns the correct addresses for the L&R files depending on the desired angles.
	void addressVectorAssign(vector<vector<vector<string>>> &filter);

	//Divide audio array into AUDIO_SEGMENT sized chunks, zero pad to give a length of WINDOW_SIZE to leave room for the convolved data
	//Copy the WINDOW_SIZE array to kiss_fft scalar array ready for fft
	//Perform fft to convert data to frequency domain, then duplicate the data so that it is available for both L & R hrir filters
	//Free fft
	void frameProcessing(kiss_fft_cpx left[], kiss_fft_cpx right[], vector<float> &sample, float freq[], int &i);

	//Perform complex multiplication of the audio data with the hrir filter
	//Save in audio array
	void convolution(kiss_fft_cpx temp[], kiss_fft_cpx audio[], kiss_fft_cpx filter[]);

	//Perform an inverse fft to convert the processed data from the frequency domain back to the time domain
	//Scale the data back from between +/- 1024 to +/- 1
	void inverseFFT(kiss_fft_cpx fft[], float wav[]);

	//Add the overlap samples from the previous frame to the beginning of the current frame
	//Save the end samples from the current frame ready to be added to the next frame
	void overlapSave(float wav[], vector<float> &overlap);

	//Contains all the processing functions needed to be called per frame
	void processingLoop(int &i);

	//This function controls the processing and calls the AudioStream functions when the data is ready
	//It is used instead of controlFunction to avoid confusion between the two
	void controlFunction();
};
#endif