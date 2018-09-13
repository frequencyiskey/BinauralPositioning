//TestMode: Gabriella Watkins U1154477

#include "TestMode.h"

TestMode::TestMode()
{
	paddedL = 0;
	frameLength = 0;
	finalLength = 0;

	ch1.addr = new char[20]();
	ch1.addr = "Audio/snare.wav";
}

TestMode::~TestMode()
{
}

//Take samples from wav file, move to float array and zero pad the array so it divides evenly by AUDIO_SEGMENT
void TestMode::initialiseAudio(CWavFile &audio, vector<float> &audioSample, int &lengthA, int &length) {
	for (int i = 0; i < lengthA; i++) {
		audioSample[i] = audio.getSample(i);
	}
	for (int i = lengthA; i < length; i++) {
		audioSample[i] = 0.0f;
	}
}

//Take samples from hrir wav, assign to a float array, zero pad the array to WINDOW_SIZE to leave room for the convolved data
//Copy sample data from float array to scalar array for kiss_fft function
//Convert data to frequency domain, free kiss_fftr_cfg fft
void TestMode::filterProcessing(CWavFile &hrir, float freq[], kiss_fft_cpx filterFFT[]) {
	float filter[WINDOW_SIZE] = { 0 };
	//an object of type kiss_fft_state
	kiss_fftr_cfg fft = kiss_fftr_alloc(WINDOW_SIZE, 0, NULL, NULL);
	for (int i = 0; i < WINDOW_SIZE; i++) {
		if (i < (hrir.getNumSamples())) {
			filter[i] = hrir.getSample(i);
		}
		else {
			filter[i] = 0.0f;
		}
	}
	for (int i = 0; i < WINDOW_SIZE; i++) {
		freq[i] = filter[i];
	}
	//Perform FFT
	kiss_fftr(fft, freq, filterFFT);
	free(fft);
}

//Take the addresses found by addressVectorAssign, convert to char array for use by CWavFile
//Open the wav file, pass to filterProcessing
void TestMode::loadFilters(vector<string> &address)
{
	//Number of elements in each vector - will define the length of the char arrays
	int addressLength = address[0].size();

	//Make sure the char array ends at the right place, 
	//so it does not continue to read random data values from memory
	left.addr[addressLength] = '\0';
	right.addr[addressLength] = '\0';

	//Copy data from string to char array to be used by wavfile class
	for (int i = 0; i < addressLength; i++)
	{
		left.addr[i] = address[0][i];
		right.addr[i] = address[1][i];
	}

	//From opening wav, to calculating fft array
	left.wav.openWav(left.addr);
	right.wav.openWav(right.addr);
	filterProcessing(left.wav, left.Freq, left.FFT);
	filterProcessing(right.wav, right.Freq, right.FFT);
}

//Index of all file addresses, returns the correct addresses for the L&R files depending on the desired angles.
void TestMode::addressVectorAssign(vector<vector<vector<string>>> &filter)
{
	//Common data amongst all addresses
	string addressStarter = "HRIRs/";

	//Number of azimuth files for a specified elevation
	int fileCount = 72;
	//The value in degrees between these files
	int fileIncrement = 360 / fileCount;

	// +/-40 elevations files have no difinitive increment between them to enable looping
	// as a result, an array has been created to effectively store all of these values
	//Currently, if the exact value isn't found, the application crashes as it runs out of the vectors range
	int file[56];
	file[0] = 0;
	file[1] = 6;
	file[2] = 13;
	file[3] = 19;
	file[4] = 26;
	file[5] = 32;
	file[6] = 39;
	file[7] = 45;
	file[8] = 51;
	file[9] = 58;
	file[10] = 64;
	file[11] = 71;
	file[12] = 77;
	file[13] = 84;
	file[14] = 90;
	file[15] = 96;
	file[16] = 103;
	file[17] = 109;
	file[18] = 116;
	file[19] = 122;
	file[20] = 129;
	file[21] = 135;
	file[22] = 141;
	file[23] = 148;
	file[24] = 154;
	file[25] = 161;
	file[26] = 167;
	file[27] = 174;
	file[28] = 180;
	file[29] = 186;
	file[30] = 193;
	file[31] = 199;
	file[32] = 206;
	file[33] = 212;
	file[34] = 219;
	file[35] = 225;
	file[36] = 231;
	file[37] = 238;
	file[38] = 244;
	file[39] = 251;
	file[40] = 257;
	file[41] = 264;
	file[42] = 270;
	file[43] = 276;
	file[44] = 283;
	file[45] = 289;
	file[46] = 296;
	file[47] = 302;
	file[48] = 309;
	file[49] = 315;
	file[50] = 321;
	file[51] = 328;
	file[52] = 334;
	file[53] = 341;
	file[54] = 347;
	file[55] = 354;

	//Set the amount of available elevation angles (-40 to +90)
	filter.resize(14);
	for (int i = 0; i < 14; i++)
	{
		//Folders with 72 files
		// -20, -10, 0, 10, 20
		if ((i > 1) && (i < 7))
		{
			fileCount = 72;
			fileIncrement = 360 / fileCount;
		}
		//Folders with 60 files
		//-30, 30
		else if ((i == 1) || (i == 7))
		{
			fileCount = 60;
			fileIncrement = 360 / fileCount;
		}
		//Folders with 56 files
		//-40, 40
		else if ((i == 0) || (i == 8))
		{
			fileCount = 56;
			fileIncrement = 360 / fileCount;
		}
		//Folder with 45 files
		//50
		else if (i == 9)
		{
			fileCount = 45;
			fileIncrement = 360 / fileCount;
		}
		//Folder with 36 files
		//60
		else if (i == 10)
		{
			fileCount = 36;
			fileIncrement = 360 / fileCount;
		}
		//Folder with 24 files
		//70
		else if (i == 11)
		{
			fileCount = 24;
			fileIncrement = 360 / fileCount;
		}
		//Folder with 12 files
		//80
		else if (i == 12)
		{
			fileCount = 12;
			fileIncrement = 360 / fileCount;
		}
		//Folders with 1 file
		//90
		else if (i == 13)
		{
			fileCount = 1;
			fileIncrement = 360 / fileCount;
		}

		//Make space for the correct amount of azimuth options depending on the elevation angle
		filter[i].resize(fileCount);
		for (int j = 0; j < fileCount; j++)
		{
			//Make space for L&R addresses
			filter[i][j].resize(2);
			for (int k = 0; k < 2; k++)
			{
				//No pattern between these files (all either 6 or 7 degrees apart) so array needs to be called
				if ((i == 0) || (i == 8))
				{
					//Number of figures in angle determines the number of '0's that need to be included in string
					if (file[j] < 10)
					{
						filter[i][j][k] = addressStarter + to_string(k + 1) + "/"
							+ to_string(((i + 1) * 10) - 50) + "/00" + to_string(file[j]) + ".wav";
					}

					else if (file[j] < 100)
					{
						filter[i][j][k] = addressStarter + to_string(k + 1) + "/"
							+ to_string(((i + 1) * 10) - 50) + "/0" + to_string(file[j]) + ".wav";
					}
					else
					{
						filter[i][j][k] = addressStarter + to_string(k + 1) + "/"
							+ to_string(((i + 1) * 10) - 50) + "/" + to_string(file[j]) + ".wav";
					}
				}
				//j can be multiplied by fileIncrement - a set value between each angle in all other cases
				else
				{
					//Number of figures in angle determines the number of '0's that need to be included
					if ((j * fileIncrement) < 10)
					{
						filter[i][j][k] = addressStarter + to_string(k + 1) + "/"
							+ to_string(((i + 1) * 10) - 50) + "/00" + to_string(j * fileIncrement) + ".wav";
					}

					else if ((j * fileIncrement) < 100)
					{
						filter[i][j][k] = addressStarter + to_string(k + 1) + "/"
							+ to_string(((i + 1) * 10) - 50) + "/0" + to_string(j * fileIncrement) + ".wav";
					}
					else
					{
						filter[i][j][k] = addressStarter + to_string(k + 1) + "/"
							+ to_string(((i + 1) * 10) - 50) + "/" + to_string(j * fileIncrement) + ".wav";
					}
				}
			}
		}
	}
}

//Divide audio array into AUDIO_SEGMENT sized chunks, zero pad to give a length of WINDOW_SIZE to leave room for the convolved data
//Copy the WINDOW_SIZE array to kiss_fft scalar array ready for fft
//Perform fft to convert data to frequency domain, then duplicate the data so that it is available for both L & R hrir filters
//Free fft
void TestMode::frameProcessing(kiss_fft_cpx left[], kiss_fft_cpx right[], vector<float> &sample, float freq[], int &i) {

	float audioSegment[WINDOW_SIZE] = { 0 };
	kiss_fftr_cfg fft = kiss_fftr_alloc(WINDOW_SIZE, 0, nullptr, nullptr);

	//Assign audio samples a segment at a time
	for (int j = 0; j < AUDIO_SEGMENT; j++) {
		audioSegment[j] = sample[(AUDIO_SEGMENT * i) + j];
	}
	//Pad the rest with zeros to give it the final data length
	for (int j = AUDIO_SEGMENT; j < WINDOW_SIZE; j++) {
		audioSegment[j] = 0.0f;
	}
	//Copy sample data to scalar array
	for (int j = 0; j < WINDOW_SIZE; j++) {
		freq[j] = audioSegment[j];
	}
	//perform fft, assign result to output arrays
	kiss_fftr(fft, freq, left);
	//Duplicate audio to be convolved with each filter.
	for (int j = 0; j < WINDOW_SIZE; j++) {
		right[j].r = left[j].r;
		right[j].i = left[j].i;
	}
	free(fft);
}

//Perform complex multiplication of the audio data with the hrir filter
//Save in audio array
void TestMode::convolution(kiss_fft_cpx temp[], kiss_fft_cpx audio[], kiss_fft_cpx filter[]) {
	for (int j = 0; j < WINDOW_SIZE; j++) {
		temp[j].r = (audio[j].r * filter[j].r) - (audio[j].i * filter[j].i);
		audio[j].i = (audio[j].r * filter[j].i) + (audio[j].i * filter[j].r);
		audio[j].r = temp[j].r;
	}
}

//Perform an inverse fft to convert the processed data from the frequency domain back to the time domain
//Scale the data back from between +/- 1024 to +/- 1
void TestMode::inverseFFT(kiss_fft_cpx fft[], float wav[]) {
	kiss_fftr_cfg ifft = kiss_fftr_alloc(WINDOW_SIZE, 1, nullptr, nullptr);
	kiss_fftri(ifft, fft, wav);
	for (int j = 0; j < WINDOW_SIZE; j++) {
		wav[j] *= (1.0f / 1024.0f);
	}
}

//Add the overlap samples from the previous frame to the beginning of the current frame
//Save the end samples from the current frame ready to be added to the next frame
void TestMode::overlapSave(float wav[], vector<float> &overlap) {
	for (int i = 0; i < OVERLAP; i++) {
		wav[i] += overlap[i];
		overlap[i] = wav[i + AUDIO_SEGMENT];
	}
}

//Contains all the processing functions needed to be called per frame
void TestMode::processingLoop(int &i)
{
	//Assign 512 audio samples a segment at a time
	frameProcessing(ch1.FFT, ch2.FFT, audioSample, ch1.Freq, i);

	convolution(ch1.TEMP, ch1.FFT, left.FFT);
	convolution(ch2.TEMP, ch2.FFT, right.FFT);

	inverseFFT(ch1.FFT, ch1.Freq);
	inverseFFT(ch2.FFT, ch2.Freq);

	overlapSave(ch1.Freq, overlap[0]);
	overlapSave(ch2.Freq, overlap[1]);

	//put the segments together in the right place of the output array
	for (int j = 0; j < AUDIO_SEGMENT; j++) {
		out[0][(i * AUDIO_SEGMENT) + j] = ch1.Freq[j];
		out[1][(i * AUDIO_SEGMENT) + j] = ch2.Freq[j];
	}
}

//This function controls the processing and calls the AudioStream functions when the data is ready
//It is used instead of main so that the class is as complete as possible
//The reason for this is so that the C++ and C# wrappers will be as simple as possible, avoiding difficult to trace issues.
void TestMode::controlFunction() {

	//Unity gain for demonstration purposes
	float gain = 1;

	//3D vector for all variants of elevation, azimuth and their left and right files
	vector<vector<vector<string>>> filter;
	//L&R string addresses
	vector<string> address;
	address.resize(2);
	//Output vector that is sent to AudioStream
	vector<vector<float>> testOutput;
	testOutput.resize(14);

	//Desired angles are passed and the relevent file addresses are returned and stored in address vector
	addressVectorAssign(filter);

	//Nested loops cycle through every available position (from -40, 0 to 90, 0)
	for (unsigned int ele = 0; ele < filter.size(); ele++)
	{
		for (unsigned int azi = 0; azi < filter[ele].size(); azi++)
		{
			//Call L&R addresses for current position
			address = filter[ele][azi];

			//Time domain signal overlap array
			overlap.resize(2);
			for (int i = 0; i < 2; i++)
			{
				overlap[i].resize(OVERLAP);
			}

			//Find filter address, load the L&R wavs, take the time domain data and convert to frequency domain array
			loadFilters(address);

			//Open snare wav
			ch1.wav.openWav(ch1.addr);

			ch1.length = ch1.wav.getNumSamples();
			paddedL = ch1.length + (AUDIO_SEGMENT - (ch1.length % AUDIO_SEGMENT)); //length of padded audio file so that it is divisible by frame size
			frameLength = ch1.length + (left.wav.getNumSamples()) - 1;				//length of a single hit at a single position
			finalLength = frameLength * filter[ele].size();							//length of stream output (one stream per elevation)
			audioSample.resize(paddedL);

			//Left, right and interleaved output
			out.resize(3);
			for (int i = 0; i < 3; i++)
			{
				if (i == 2)
				{
					out[i].resize(frameLength * 2);
				}
				else
				{
					out[i].resize(frameLength);
				}
			}

			//Resize to fit sample data for each position in turn
			testOutput[ele].resize(finalLength * 2);

			initialiseAudio(ch1.wav, audioSample, ch1.length, paddedL);

			//for loop the cycles through each 512 segment of audio data
			for (int i = 0; i < (paddedL / AUDIO_SEGMENT); i++) {

				//Function that contains the window processing loop
				processingLoop(i);
			}

			//Add the last overlap to the end of the wav
			for (int i = 0; i < OVERLAP; i++) {
				out[0][frameLength - OVERLAP + i] = overlap[0][i];
				out[1][frameLength - OVERLAP + i] = overlap[1][i];
			}

			//Copy L&R data to interleaved vector
			for (int i = 0; i < frameLength; i++) {
				out[2][i * 2] = out[0][i];
				out[2][(i * 2) + 1] = out[1][i];
			}

			//Copy the data to the right position in the stream output array
			for (int j = 0; j < frameLength * 2; j++)
			{
				testOutput[ele][(azi * frameLength * 2) + j] = out[2][j];
			}
		}
		//AudioStream function that handles streaming
		stream.processControl(testOutput[ele], finalLength);
	}
}