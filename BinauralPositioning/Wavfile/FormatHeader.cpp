// Matthew Wankling & Paul Lunn
//10/07/06
//FormatHeader.cpp


#include "FormatHeader.h"
#include <string.h>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFormatHeader::CFormatHeader()
{

	fmtChunkID[0] = 'f';
	fmtChunkID[1] = 'm';
	fmtChunkID[2] = 't';
	fmtChunkID[3] = ' ';

	fmtChunkSize = 16;

	audioFormat =  1; //PCM = 1

	numChannels = 2;
	
	sampleRate = 44100;//DEFAULT_SAMPLE_RATE;
	byteRate = 0;
	blockAlign = 0;
	bitsPerSample = 16;  //DEFAULT_RESOLUTION;

}


void CFormatHeader::displayHeaderData(void)
{

	cout<< "---------------------------------"<<endl;
	cout << "Chunk ID:\t";
	cout.write(fmtChunkID,4);
	cout <<endl<<"Chunk Size:\t" << fmtChunkSize<< endl;
	cout << "Audio Format: \t" << audioFormat<< endl;
	cout << "No. Channels: \t" << numChannels<< endl;
	cout << "Sample Rate: \t" << sampleRate<< endl;
	cout << "Byte Rate: \t" << byteRate<< endl;
	cout << "Block Align: \t" << blockAlign<< endl;
	cout << "Bit Depth:\t" << bitsPerSample<< endl;
	cout<< "---------------------------------"<<endl;

}

short CFormatHeader::getBlockAlign()
{
	return blockAlign;
}

short CFormatHeader::getAudioFormat()
{
	return audioFormat;
}

short CFormatHeader::getNumChannels()
{
	return numChannels;
}

short CFormatHeader::getBitsPerSample()
{
	return bitsPerSample;
}

int CFormatHeader::getSampleRate()
{
	return sampleRate;
}

void CFormatHeader::setSampleRate(int sRate)
{
	sampleRate=sRate;
}

void CFormatHeader::setBitsPerSample(short bits)
{
	bitsPerSample=bits;
}

void CFormatHeader::setNumChannels(short channels)
{
	numChannels=channels;
}

void CFormatHeader::setFmtChunkSize(int fmtsize)
{
	fmtChunkSize=fmtsize;
}

void CFormatHeader::setAudioFormat(short audioformat)
{
	audioFormat=audioformat;
}

void CFormatHeader::setByteRate(int rate)
{
	byteRate=rate;
}

void CFormatHeader::setBlockAlign(short align)
{
	blockAlign=align;
}

