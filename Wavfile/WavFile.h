// Matthew Wankling & Paul Lunn
// 10/07/06
// ProcessWav.h

#ifndef __MRWPROCESSWAV__
#define __MRWPROCESSWAV__

#include <iostream>
#include <fstream>
#include "FileHeader.h"
#include "DataHeader.h"
#include "FormatHeader.h"

using namespace std;

/**Class provides user with methods to open, save and create new wavs and deal with sample data*/
class CWavFile
{

public:

	CWavFile();		//constructor

	/**Opens a wave file.<br>
	The file is read and validated in a number of ways, to ensure that it is
	a standard PCM Wav which can then be processed by other functions within the class*/
	void CWavFile::openWav(char* wavfile);
	/**Saves a wave file.<br>
	The new file is written using chunk header data currently held in the classes, and the sample data in 
	sampleData[] array. All previous formatting is stripped, so that the fmt chunk begins at byte 12 and the
	data chunk at byte 36*/
	void CWavFile::saveWav(char* wavfile);
	/**Function creates the header information for a new wav file.<br><br>
	This does not output to a file, but instead sets up new chunk
	information which can then be saved, or data manipulated within it.
	the length in milliseconds produces the appropriate data chunk size
	dependant on the other arguments.*/
	void newWav(int sampleRate, short bitDepth, int lengthMS, short numChannels);
	
	/**Get a single sample by passing a position*/
	float getSample(long position);
	/**Set a single sample by passing the sample value<br>
	This should be a float in the range +/-1<br>
	The position to store the sample at is also needed*/
	void setSample(float sample, long position);

	/**Returns a pointer to the sampleData array*/
	float* getSampleData();

	/**Sets the block of sample data by passing an array to the function.<br>
	This data is validated, (each sample must be in the range +/-1)
	The length of the array must also be sent, in samples
	This then recalculates and sets the information regarding the chunk sizes of the File and Data headers*/
	void setSampleData(float* data,long length);

	/**Can be called to check whether a file has been opened.<br>
	This is valid if some sample data is found.<br>
	A string argument can be sent which can be used to notify the user where the function was called, and therefore
	helps to debug the main function code*/
	void checkFileOpened(char* command);
	
	/**Returns the variable numSamples*/
	long getNumSamples();
	
	int getSampleRate();
	short getNumChannels();


	/**Calls the three chunk display routines to output all WAV file information to the screen*/
	void CWavFile::displayWavInfo(void);
	/**Writes the 'list' chunk at the current file position - the text is set to "CREATED AT HUDD UNI"*/
	void writeFooter(void);

private:

	float sixteenBitToFloat(short input);
	short floatToSixteenBit(float input);
	float twentyfourBitToFloat(int input);
	int floatToTwentyFourBit(float input);
	float thirtytwoBitToFloat(int input);
	int floatToThirtyTwoBit(float input);

	int fmtStart;		//variable to hold the start position in bytes of the format chunk
	int dataStart;		//variable to hold the start position in bytes of the data chunk

	int length;			//length of file in bytes

	char* buffer;		//buffer for initial reading of data from file

	CFileHeader fileHeader;		//instances of the three structures for holding Chunk info for File/Format/Data
	CFormatHeader fmtHeader;
	CDataHeader dataHeader;

	ifstream infile;			//creates an instance of the input sream class
	ofstream outfile;			//creates an instance to output data

	int validCheck;
	bool validated;

	long numSamples;			//the number of samples, determined during the openwav routine
	float* sampleData;			//pointer to memory to store the actual samples in


	/**structure for creating text tag for end of file*/
	struct text
	{
		/**Set to 'list' - the ID of the chunk allowing text to be stored at the end of the wave file */
		char endChunkID[4];
		/**Size in bytes of the character array*/
		long endChunkSize;
		/**Character array to hold the text at the end of all saved wave files using this class */
		char output[20];
	}endtext;

};

#endif
