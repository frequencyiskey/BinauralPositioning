//AudioStream: Gabriella Watkins U1154477

#include <vector>
#include "Wavfile\WavFile.h"
#include "portaudio.h"

#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

const int SAMPLE_RATE = 44100;								//Necessary sample rate of wav files
const int FRAMES_PER_BUFFER = 1024;							//number of samples streamed per buffer
const int NUM_CHANNELS = 2;									//Stereo output

															// Define sample format
const PaSampleFormat PA_SAMPLE_TYPE = paFloat32;			//data values between +/- 1
typedef float SAMPLE;										//define new type for use clarity

using namespace std;

class AudioStream
{
private:

	struct paData
	{
		int          frameIndex;	//Index into sample array
		int          maxFrameIndex;	//Possibly not needed if run on user input?
		SAMPLE		*wavSamples;	//sample data
	};

	PaStreamParameters  outputParameters;		//Information for output device
	PaStream*           stream;					//Provides access to audio hardware
	PaError             err = paNoError;		//error codes returned to tell the code how to continue after each function
	paData				wavData;				//Struct
	int                 totalFrames;			//The total number of frames that one channel of the wav data is comprised of
	int                 numBytes;				//The number of bytes needed to house the entire wav length

public:

	//Function called when streaming is done - or an error occurs and it needs to be terminated, with memory freed
	void cleaningUp();

	/* This routine will be called by the PortAudio engine when audio is needed.
	** It may be called at interrupt level on some machines so don't do anything
	** that could mess up the system like calling malloc() or free().
	*/
	static int playCallback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);

	//Receives data from BinauralConvolution, assign data to data.wavSamples and normalise ready for output
	void getWavData(vector<float> &data, int &length);

	//Function which sets all the parameters for playback and uses them to open, start and close the stream when playback is finished
	void setupPlayback();

	//Controls when each function is called
	int processControl(vector<float> &data, int &length);
};
#endif