//BinauralConvolution: Gabriella Watkins U1154477
#include "TestMode.h"

#ifndef BINAURALCONVOLUTION_H
#define BINAURALCONVOLUTION_H

const int NO_INSTRUMENTS = 6;				//Number of audio sources (.wav files)

using namespace std;

class BinauralConvolution
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

	//all relevent parameters needed to process audio
	struct audioData
	{
		string					instrumentName;
		CWavFile				wav;
		char					*addr;
		int						length;
		int						elevation, azimuth;		//User defined angles
		float					distance;				//User defined distance (in metres)
		float					gainCoef;				//gain coefficient calculated based on the distance (inv. square law)
		int						cutoffFreq;				//Low pass filter cut-off frequency - based on the distance
		vector<float>			overlap;
		vector<float>			audioSample;			//vector for mono audio file
		vector<string>			address;				//L&R string addresses (HRIR .wav files)
		kiss_fft_scalar			Freq[WINDOW_SIZE];		//Array used to store the time domain data in/out of the fft process
		kiss_fft_cpx			FFT[WINDOW_SIZE];		//Data in the frequency domain
		kiss_fft_cpx			TEMP[WINDOW_SIZE];		//Temporary array used for complex multiplication
		hrirData				left, right;			//left/right hrir data - struct
		vector<float>			indivOut;				//Output for each source
	};

	AudioStream stream;			//Instance of AudioStream class
	audioData *instrument;		//Array of struct instances for each sound source
	int paddedL;				//length of audio file so that it is divisible by frame size (zero padded)
	int finalLength;			//length of output as a whole
	vector<float> out;			//Left, right and interleaved output
	string testEnable;			//Stores user input to determine test/mix mode

public:

	BinauralConvolution();
	~BinauralConvolution();

	//Take samples from wav file, move to float array and zero pad the array so it divides evenly by AUDIO_SEGMENT
	//Apply low pass filter and gain reduction for distance
	void initialiseAudio(audioData &audio, int &length);

	//Function that applies a low pass filter to the audio. 
	//Its cutoff frequency is dependant on the distance desired
	vector<float> CononicalLowPass(audioData &audio, int &length);

	//Take samples from hrir wav, assign to a float array, zero pad the array to WINDOW_SIZE to leave room for the convolved data
	//Copy sample data from float array to scalar array for kiss_fft function
	//Convert data to frequency domain, free kiss_fftr_cfg fft
	void filterProcessing(CWavFile &hrir, float freq[], kiss_fft_cpx filterFFT[]);

	//Take the addresses found by addressVectorAssign, convert to char array for use by CWavFile
	//Open the wav file, pass to filterProcessing
	void loadFilters(audioData &audio);

	//Divide audio array into AUDIO_SEGMENT sized chunks, zero pad to give a length of WINDOW_SIZE to leave room for the convolved data
	//Copy the WINDOW_SIZE array to scalar array ready for fft
	//Perform fft to convert data to frequency domain, then duplicate the data so that it is available for both L & R hrir filters
	void frameProcessing(kiss_fft_cpx left[], kiss_fft_cpx right[], vector<float> &sample, float freq[], int &i);

	//Perform complex multiplication of the audio data with the hrir filter for left and right channels
	//Save in audio array
	void convolution(kiss_fft_cpx temp[], kiss_fft_cpx audio[], kiss_fft_cpx filter[]);

	//Perform an inverse fft to convert the processed data from the frequency domain back to the time domain
	//Scale the data back from between +/- 1024 to +/- 1
	void inverseFFT(kiss_fft_cpx fft[], float wav[]);

	//Add the overlap samples from the previous frame to the beginning of the current frame
	//Save the end samples from the current frame ready to be added to the next frame
	void overlapSave(float wav[], vector<float> &overlap);

	//Index of all file addresses, returns the correct addresses for the L&R files depending on the desired angles.
	vector<string> addressVectorAssign(int &ele, int &azi);

	//Contains all the processing functions needed to be called per frame
	void processingLoop();

	//Asks for users desired parameters and stores them
	void mixParameters(audioData &audio);

	//This function controls the processing and calls the AudioStream functions when the data is ready
	//It is used instead of main so that the class is as complete as possible
	//The reason for this is so that the C++ and C# wrappers will be as simple as possible, avoiding difficult to trace issues.
	int controlFunction();
};
#endif