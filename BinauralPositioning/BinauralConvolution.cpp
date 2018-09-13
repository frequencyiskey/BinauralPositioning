//BinauralConvolution: Gabriella Watkins U1154477

#include "BinauralConvolution.h"

BinauralConvolution::BinauralConvolution()
{
	paddedL = 0;
	finalLength = 0;

	instrument = new audioData[NO_INSTRUMENTS * 2];

	instrument[0].addr = new char[35]();
	instrument[2].addr = new char[35]();
	instrument[4].addr = new char[35]();
	instrument[6].addr = new char[35]();
	instrument[8].addr = new char[35]();
	instrument[10].addr = new char[35]();

	instrument[0].addr = "Audio_Files/Bass.wav";
	instrument[2].addr = "Audio_Files/AcousticGuitar.wav";
	instrument[4].addr = "Audio_Files/ElecGuitar.wav";
	instrument[6].addr = "Audio_Files/Bvox4.wav";
	instrument[8].addr = "Audio_Files/Bvox2.wav";
	instrument[10].addr = "Audio_Files/LeadVox.wav";

	instrument[0].distance = 1.0f;
	instrument[2].distance = 1.0f;
	instrument[4].distance = 1.0f;
	instrument[6].distance = 1.0f;
	instrument[8].distance = 1.0f;
	instrument[10].distance = 1.0f;

	instrument[0].gainCoef = 1.0f;
	instrument[2].gainCoef = 1.0f;
	instrument[4].gainCoef = 1.0f;
	instrument[6].gainCoef = 1.0f;
	instrument[8].gainCoef = 1.0f;
	instrument[10].gainCoef = 1.0f;

	//For UI's benefit
	instrument[0].instrumentName = "Bass";
	instrument[2].instrumentName = "Acoustic Guitar";
	instrument[4].instrumentName = "Electric Guitar";
	instrument[6].instrumentName = "Female Backing Vocals";
	instrument[8].instrumentName = "Male Backing Vocals";
	instrument[10].instrumentName = "Lead Vocals";
}

BinauralConvolution::~BinauralConvolution()
{
	delete[] instrument;
}

//Take samples from wav file, move to float array and zero pad the array so it divides evenly by AUDIO_SEGMENT
//Apply low pass filter and gain reduction for distance
void BinauralConvolution::initialiseAudio(audioData &audio, int &length) {
	//Difference in gain calculated by the desired distance and inv. square law
	audio.gainCoef = sqrt(1.0f / (audio.distance * audio.distance));

	//Get samples from wav file
	for (int i = 0; i < audio.length; i++) {
		audio.audioSample[i] = (audio.wav.getSample(i) * audio.gainCoef);
	}
	//Add zero padding so that it is equally divisible by AUDIO_SEGMENT
	for (int i = audio.length; i < length; i++) {
		audio.audioSample[i] = 0.0f;
	}
	//Low pass filter cut-off frequency calculated by 10000Hz divided by the desired distance
	//As this phenomenon is location specific, a default calculation was used to demonstrate it's effect
	if (audio.distance > 10)
	{
		audio.cutoffFreq = 10000 / (audio.distance);
	}
	else
	{
		audio.cutoffFreq = 20000;
	}

	//Apply low pass filter to data
	audio.audioSample = CononicalLowPass(audio, length);
	cout << "Low pass applied" << endl;
}

//Applies a low pass filter to the audio data
//The cut-off frequency is dependant on the distance the sound source is from the listener
vector<float> BinauralConvolution::CononicalLowPass(audioData &audio, int &length)
{
	float K = tan(PI * audio.cutoffFreq / SAMPLE_RATE);
	float Q = 1.0f / sqrt(2.0f);	//Q-factor, controls height of resonanceat 1/sqrt(2) filter is maximally flat up to cut-off freq

	vector<vector<float>> coefs;	//(a0, a1, a2, b0, b1, b2)
	vector<float> x;
	vector<float> xh;
	vector<float> y;

	x.resize(length);
	xh.resize(length);
	y.resize(length);

	//a,b
	coefs.resize(2);

	//0,1,2
	for (int i = 0; i < 2; i++)
	{
		coefs[i].resize(3);
	}

	x = audio.audioSample;

	coefs[0][0] = 1;												//a0
	coefs[0][1] = (2 * Q * ((K * K) - 1)) / ((K * K * Q) + K + Q);	//a1
	coefs[0][2] = ((K * K * Q) - K + Q) / ((K * K * Q) + K + Q);	//a2

	coefs[1][0] = (K * K * Q) / ((K * K * Q) + K + Q);				//b0
	coefs[1][1] = (2 * K * K * Q) / ((K * K * Q) + K + Q);			//b1
	coefs[1][2] = (K * K * Q) / ((K * K * Q) + K + Q);				//b2

																	//Difference Equations
	for (int i = 0; i < length; i++)
	{
		//if i is less that 2, the delays in the equation will cause an error
		//Therefore, they have to be replaced with 0 until there is a sample in the correct position for the delay to use
		if (i == 1)
		{
			xh[i] = (coefs[0][0] * x[i]) - (coefs[0][1] * xh[i - 1])
				- (coefs[0][2] * 0);
			y[i] = (coefs[1][0] * xh[i]) + (coefs[1][1] * xh[i - 1])
				+ (coefs[1][2] * 0);
		}
		else if (i == 0)
		{
			xh[i] = (coefs[0][0] * x[i]) - (coefs[0][1] * 0)
				- (coefs[0][2] * 0);
			y[i] = (coefs[1][0] * xh[i]) + (coefs[1][1] * 0)
				+ (coefs[1][2] * 0);
		}
		else
		{
			xh[i] = (coefs[0][0] * x[i]) - (coefs[0][1] * xh[i - 1])
				- (coefs[0][2] * xh[i - 2]);
			y[i] = (coefs[1][0] * xh[i]) + (coefs[1][1] * xh[i - 1])
				+ (coefs[1][2] * xh[i - 2]);
		}
	}
	return y;
}

//Take samples from hrir wav, assign to a float array, zero pad the array to WINDOW_SIZE to leave room for the convolved data
//Copy sample data from float array to scalar array for kiss_fft function
//Convert data to frequency domain, free kiss_fftr_cfg fft
void BinauralConvolution::filterProcessing(CWavFile &hrir, float freq[], kiss_fft_cpx filterFFT[]) {

	float filter[WINDOW_SIZE] = { 0 };
	kiss_fftr_cfg fft = kiss_fftr_alloc(WINDOW_SIZE, 0, NULL, NULL);	//an object of type kiss_fft_state

																		//Take filter samples from wav file, then zero pad the end to give it a length of WINDOW_SIZE
	for (int i = 0; i < WINDOW_SIZE; i++) {
		if (i < (hrir.getNumSamples())) {
			filter[i] = hrir.getSample(i);
		}
		else {
			filter[i] = 0.0f;
		}
	}
	//Copy data to scalar array for use by kiss_fft
	for (int i = 0; i < WINDOW_SIZE; i++) {
		freq[i] = filter[i];
	}
	//Perform FFT
	kiss_fftr(fft, freq, filterFFT);
	free(fft);
}

//Take the addresses found by addressVectorAssign, convert to char array for use by CWavFile
//Open the wav file, pass to filterProcessing
void BinauralConvolution::loadFilters(audioData &audio)
{
	//Desired angles are passed and the relevent file addresses are returned and stored in address vector
	audio.address = addressVectorAssign(audio.elevation, audio.azimuth);

	//Number of elements in each vector - will define the length of the char arrays
	int addressLength = audio.address[0].size();

	//Make sure the char array ends at the right place, 
	//so it does not continue to read random data values from memory
	audio.left.addr[addressLength] = '\0';
	audio.right.addr[addressLength] = '\0';

	//Copy data from string to char array to be used by wavfile class
	for (int i = 0; i < addressLength; i++)
	{
		audio.left.addr[i] = audio.address[0][i];
		audio.right.addr[i] = audio.address[1][i];
	}

	cout << audio.left.addr << endl;
	cout << audio.right.addr << endl;

	//From opening wav, to calculating fft array
	audio.left.wav.openWav(audio.left.addr);
	audio.right.wav.openWav(audio.right.addr);
	filterProcessing(audio.left.wav, audio.left.Freq, audio.left.FFT);
	filterProcessing(audio.right.wav, audio.right.Freq, audio.right.FFT);
	cout << "Filters Processed" << endl;
}

//Divide audio array into AUDIO_SEGMENT sized chunks, zero pad to give a length of WINDOW_SIZE to leave room for the convolved data
//Copy the WINDOW_SIZE array to scalar array ready for fft
//Perform fft to convert data to frequency domain, then duplicate the data so that it is available for both L & R hrir filters
void BinauralConvolution::frameProcessing(kiss_fft_cpx left[], kiss_fft_cpx right[], vector<float> &sample, float freq[], int &i) {

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

//Perform complex multiplication of the audio data with the hrir filter for left and right channels
//Save in audio array
void BinauralConvolution::convolution(kiss_fft_cpx temp[], kiss_fft_cpx audio[], kiss_fft_cpx filter[]) {
	for (int j = 0; j < WINDOW_SIZE; j++) {
		temp[j].r = (audio[j].r * filter[j].r) - (audio[j].i * filter[j].i);
		audio[j].i = (audio[j].r * filter[j].i) + (audio[j].i * filter[j].r);
		audio[j].r = temp[j].r;
	}
}

//Perform an inverse fft to convert the processed data from the frequency domain back to the time domain
//Scale the data back from between +/- 1024 to +/- 1
void BinauralConvolution::inverseFFT(kiss_fft_cpx fft[], float wav[]) {

	kiss_fftr_cfg ifft = kiss_fftr_alloc(WINDOW_SIZE, 1, nullptr, nullptr);

	kiss_fftri(ifft, fft, wav);
	for (int j = 0; j < WINDOW_SIZE; j++) {
		wav[j] *= (1.0f / 1024.0f);
	}
}

//Add the overlap samples from the previous frame to the beginning of the current frame
//Save the end samples from the current frame ready to be added to the next frame
void BinauralConvolution::overlapSave(float wav[], vector<float> &overlap) {
	for (int i = 0; i < OVERLAP; i++) {
		wav[i] += overlap[i];
		overlap[i] = wav[i + AUDIO_SEGMENT];
	}
}

//Index of all file addresses, returns the correct addresses for the L&R files depending on the desired angles.
vector<string> BinauralConvolution::addressVectorAssign(int &ele, int &azi)
{
	//Common data amongst all addresses
	string addressStarter = "HRIRs/";

	//3D vector for all variants of elevation, azimuth and their left and right files
	vector<vector<vector<string>>> filter;

	//Number of azimuth files for a specified elevation
	int fileCount = 72;
	//The value in degrees between these files
	int fileIncrement = 360 / fileCount;

	// +/-40 elevations files have no difinitive increment between them to enable looping
	// as a result, an array has been created to effectively store all of these values
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
	//If elevation is equal to 0 then it can simply be set to the 5th element (4) (-40, -30, -20, -10, 0)
	//Otherwise the calculation from above is reversed to get the vector element number from the given angle
	if (ele != 0)
	{
		ele = ((ele + 50) / 10) - 1;
	}
	else
	{
		ele = 4;
	}

	if ((ele > 1) && (ele < 7))
	{
		fileCount = 72;
		fileIncrement = 5;
	}
	else if ((ele == 1) || (ele == 7))
	{
		fileCount = 60;
		fileIncrement = 6;
	}
	else if ((ele == 0) || (ele == 8))
	{
		fileCount = 56;
		fileIncrement = 7;
	}
	else if (ele == 9)
	{
		fileCount = 45;
		fileIncrement = 8;
	}
	else if (ele == 10)
	{
		fileCount = 36;
		fileIncrement = 10;
	}
	else if (ele == 11)
	{
		fileCount = 24;
		fileIncrement = 15;
	}
	else if (ele == 12)
	{
		fileCount = 12;
		fileIncrement = 30;
	}
	else if (ele == 13)
	{
		azi = 0;
	}
	//If azi is 0 then nothing needs to be done
	//Otherwise it is either divided by the gap between available angles to give the vector element number
	//or, when attempting to access the array, an algorithm is used to search the array for the desired value and returning the index
	//Currently, this means the application crashes if an unavailable value is requested
	if (azi > 0)
	{
		if ((ele == 0) || (ele == 8))
		{
			int x = std::distance(file, find(file, file + 56, azi));
			azi = x;
		}
		else
		{
			azi = azi / fileIncrement;
		}
	}
	//return the correct addresses
	return filter[ele][azi];
}

//Contains all the processing functions needed to be called per frame
void BinauralConvolution::processingLoop()
{
	for (int j = 0; j < NO_INSTRUMENTS; j++)
	{
		for (int i = 0; i < (paddedL / AUDIO_SEGMENT); i++) {

			//Assign 512 audio samples a segment at a time
			frameProcessing(instrument[j * 2].FFT, instrument[(j * 2) + 1].FFT,
				instrument[j * 2].audioSample, instrument[j * 2].Freq, i);

			convolution(instrument[j * 2].TEMP, instrument[j * 2].FFT, instrument[j * 2].left.FFT);
			convolution(instrument[(j * 2) + 1].TEMP, instrument[(j * 2) + 1].FFT,
				instrument[j * 2].right.FFT);

			inverseFFT(instrument[j * 2].FFT, instrument[j * 2].Freq);
			inverseFFT(instrument[(j * 2) + 1].FFT, instrument[(j * 2) + 1].Freq);

			overlapSave(instrument[j * 2].Freq, instrument[j * 2].overlap);
			overlapSave(instrument[(j * 2) + 1].Freq, instrument[(j * 2) + 1].overlap);

			//put the segments together in the right place of the output array
			for (int k = 0; k < AUDIO_SEGMENT; k++) {
				instrument[j * 2].indivOut.push_back(instrument[j * 2].Freq[k]);
				instrument[(j * 2) + 1].indivOut.push_back(instrument[(j * 2) + 1].Freq[k]);
			}
		}
	}
}

//Asks for users desired parameters and stores them
void BinauralConvolution::mixParameters(audioData &audio)
{
	//Ask for user data
	cout << "What is the elevation angle for " + audio.instrumentName + "?" << endl << "(10 degree increments between -40 and 90):" << endl;
	cin >> audio.elevation;
	cout << endl;
	cout << "What is the azimuth angle for " + audio.instrumentName + "?" << endl << "(check USER GUIDE for available positions):" << endl;
	cin >> audio.azimuth;
	cout << endl;
	cout << "How far away is the " + audio.instrumentName + "? (in metres, 1m minimum): " << endl;
	cin >> audio.distance;
	cout << endl;
}

//This function controls the processing and calls the AudioStream functions when the data is ready
//It is used instead of main so that the class is as complete as possible
//The reason for this is so that the C++ and C# wrappers will be as simple as possible, avoiding difficult to trace issues.
int BinauralConvolution::controlFunction() {

	TestMode test;
	bool correctInput = false;

	for (int i = 0; i < (NO_INSTRUMENTS * 2); i++)
	{
		//Time domain signal overlap array
		instrument[i].overlap.resize(OVERLAP);
		//(first channel of instruments only)
		if (i % 2 == 0)
		{
			instrument[i].address.resize(2);
		}
	}

	//While loop that allows you to select run mode, will continue asking until one is chosen
	while (!correctInput)
	{
		cout << "To run test mode, enter 'test'" << endl
			<< "To run mix mode, enter 'mix': " << endl
			<< "To exit the application, enter 'exit': " << endl;
		cin >> testEnable;
		cout << endl;

		//Calls test class
		if (testEnable == "test")
		{
			test.controlFunction();
			correctInput = true;
		}
		//Continues with Binaural Convolution
		else if (testEnable == "mix")
		{
			correctInput = true;

			//For loop to take all of the user input before doing anything else
			for (int i = 0; i < NO_INSTRUMENTS; i++)
			{
				mixParameters(instrument[i * 2]);
			}

			for (int i = 0; i < (NO_INSTRUMENTS * 2); i++)
			{
				if (i % 2 == 0)
				{
					//Find filter address, load the L&R wavs, take the time domain data and convert to frequency domain array
					loadFilters(instrument[i]);
					//Open wav, assign samples to array.
					instrument[i].wav.openWav(instrument[i].addr);
					instrument[i].length = instrument[i].wav.getNumSamples();
					//Only needs to be run once at the beginning
					if (i == 0)
					{
						//length of audio file so that it is divisible by frame size
						paddedL = instrument[i].length + (AUDIO_SEGMENT - (instrument[i].length % AUDIO_SEGMENT));
						//length of final data (one channel)
						finalLength = instrument[i].length + (instrument[i].left.wav.getNumSamples()) - 1;
						//interleaved output
						out.resize(finalLength * 2);
					}

					instrument[i].audioSample.resize(paddedL, 0);		//set vector size
					initialiseAudio(instrument[i], paddedL);
					cout << "Audio initialised" << endl;
				}
			}


			//Function that contains the window processing loop
			processingLoop();

			cout << "Processing complete" << endl;
			for (int i = 0; i < (NO_INSTRUMENTS * 2); i++)
			{
				//Add the last overlap to the end of the data
				for (int j = 0; j < OVERLAP; j++) {
					instrument[i].indivOut[finalLength - OVERLAP + j] = instrument[i].overlap[j];
				}

				if (i % 2 == 0)
				{
					//Copy both channels' data to interleaved vector
					for (int j = 0; j < finalLength; j++) {
						out[j * 2] += instrument[i].indivOut[j];
						out[(j * 2) + 1] += instrument[i + 1].indivOut[j];
					}
				}
			}
			//AudioStream function that handles streaming
			stream.processControl(out, finalLength);
		}
		//return to main
		else if (testEnable == "exit")
		{
			return 1;
		}
		//loop back to choices
		else
		{
			cout << "Incorrect selection, please try again." << endl;
			cout << endl;
			correctInput = false;
		}
	}
	return 0;
}