// Matthew Wankling & Paul Lunn
//10/07/06
//FormatHeader.h

#ifndef __MRWFORMATHEADER__
#define __MRWFORMATHEADER__

 const int DEFAULT_SAMPLE_RATE = 44100;
const int DEFAULT_RESOLUTION = 16;


/**Class structure holds the 16 byte format chunk information*/
class CFormatHeader  
{
private:

	char fmtChunkID[4];
	int fmtChunkSize;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;

public:

	CFormatHeader();

	/**Outputs format chunk information to the screen*/
	void CFormatHeader::displayHeaderData(void);

	void setFmtChunkSize(int fmtsize);
	void setAudioFormat(short audioformat);
	void setByteRate(int rate);
	void setBlockAlign(short align);
	void setSampleRate(int sRate);
	void setBitsPerSample(short bits);
	void setNumChannels(short channels);

	short getBlockAlign();
	short getAudioFormat();
	short getNumChannels();
	short getBitsPerSample();
	int getSampleRate();
	
};

#endif