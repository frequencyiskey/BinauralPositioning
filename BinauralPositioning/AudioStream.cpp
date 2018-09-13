//AudioStream: Gabriella Watkins U1154477

#include "AudioStream.h"

//Function called when streaming is done - or an error occurs and it needs to be terminated, with memory freed
void AudioStream::cleaningUp() {

	Pa_Terminate();						//PortAudio Function
	if (wavData.wavSamples)				// Sure it is NULL or valid
		free(wavData.wavSamples);
	if (err != paNoError)
	{
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		err = 1;          // Always return 0 or 1, but no other return codes
	}
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
int AudioStream::playCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	paData *data = (paData*)userData;									//Array data
	SAMPLE *rptr = &data->wavSamples[data->frameIndex * NUM_CHANNELS];	//Read pointer to address of data
	SAMPLE *wptr = (SAMPLE*)outputBuffer;								//Write pointer to output Buffer
	unsigned int i;
	int finished;														//Value assigned and returned to determine whether streaming is complete
	unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;	//Frames left in stream

	(void)inputBuffer; // Prevent unused variable warnings, despite no input device
	(void)timeInfo;
	(void)statusFlags;
	(void)userData;

	if (framesLeft < framesPerBuffer)					// final buffer
	{
		for (i = 0; i < framesLeft; i++)
		{
			*wptr++ = *rptr++;							// left
			*wptr++ = *rptr++;							// right
		}
		for (; i < framesPerBuffer; i++)				//i = framesLeft
		{
			*wptr++ = 0;								// left
			*wptr++ = 0;								// right
		}
		data->frameIndex += framesLeft;
		finished = paComplete;
	}
	else
	{
		for (i = 0; i < framesPerBuffer; i++)
		{
			*wptr++ = *rptr++;							// left
			*wptr++ = *rptr++;							// right
		}
		data->frameIndex += framesPerBuffer;
		finished = paContinue;
	}
	return finished;
}

//Receives data from BinauralConvolution, assign data to data.wavSamples and normalise ready for output
void AudioStream::getWavData(vector<float> &data, int &length) {

	wavData.maxFrameIndex = totalFrames = (length);			//the length of one channel of the wav file in samples
	wavData.frameIndex = 0;									//Beginning
	numBytes = length * 2 * sizeof(SAMPLE);					//Total number of bytes the data requires
	wavData.wavSamples = (SAMPLE *)malloc(numBytes);		//wavSamples is initialised

	if (wavData.wavSamples == NULL)							//error checking, make sure array exists
	{
		cleaningUp();										//Terminate function
	}
	for (int i = 0; i < length * 2; i++) {					//assign data to wavSamples
		wavData.wavSamples[i] = data[i];
	}
	//Normalise to irradicate potential distortion
	float max = 1.0f;
	for (int i = 0; i < length * 2; i++)
	{
		if (abs(wavData.wavSamples[i]) > max)
		{
			max = abs(wavData.wavSamples[i]);
		}
	}
	for (int i = 0; i < length * 2; i++)
	{
		wavData.wavSamples[i] *= (1.0f / max);
	}
}

//Function which sets all the parameters for playback and uses them to open, start and close the stream when playback is finished
void AudioStream::setupPlayback() {
	wavData.frameIndex = 0;									//Start at the beginning!

	err = Pa_Initialize();									//Initialise PortAudio
	if (err != paNoError) cleaningUp();						//Terminate function

	outputParameters.device = Pa_GetDefaultOutputDevice();	// default output device
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		cleaningUp();
	}
	outputParameters.channelCount = 2;						// stereo output
	outputParameters.sampleFormat = PA_SAMPLE_TYPE;			//data values betwee +/- 1
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;		//No specific API

															//Prints out important info about setup - Descriptions of any errors here!
	printf("\n=== Now playing! ===\n"); fflush(stdout);

	//Open the stream with the set parameters
	err = Pa_OpenStream(
		&stream,
		NULL,			// no input
		&outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,      // no out of range samples so no need to clip them
		playCallback,
		&wavData);
	if (err != paNoError) cleaningUp();

	//if the stream is open
	if (stream)
	{
		//Start stream
		err = Pa_StartStream(stream);
		if (err != paNoError) cleaningUp();

		printf("Waiting for playback to finish.\n"); fflush(stdout);

		//While stream is active, sleep
		while ((err = Pa_IsStreamActive(stream)) == 1) Pa_Sleep(100);
		if (err < 0) cleaningUp();

		//Close stream
		err = Pa_CloseStream(stream);
		if (err != paNoError) cleaningUp();

		printf("Done.\n"); fflush(stdout);
	}
}

//Controls when each function is called
int AudioStream::processControl(vector<float> &data, int &length)
{
	//Open wav file, assign data to data.wavSamples (will take data directly from buffer ultimately?)
	getWavData(data, length);

	//Function which sets all the parameters for playback and uses them to open, start and close the stream when playback is finished
	setupPlayback();

	//Function called when streaming is done - or an error occurs and it needs to be terminated, with memory freed
	cleaningUp();

	return err;
}