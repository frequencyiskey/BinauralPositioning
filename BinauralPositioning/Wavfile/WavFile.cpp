// Matthew Wankling & Paul Lunn
//10/07/06
//ProcessWav.cpp

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include <windef.h>
#include "FileHeader.h"
#include "FormatHeader.h"
#include "DataHeader.h"
#include "WavFile.h"

using namespace std;

CWavFile::CWavFile()		//constructor
{
	fmtStart=12;
	dataStart=36;
	length=0;
	validCheck=0;
	numSamples=0;
	validated=false;
}



//*************************************************************************************************************************
//OPEN A WAV FILE, SEARCH FOR THE DIFFERENT CHUNKS, VALIDATE THEM AND READ IN RAW SAMPLE DATA
//*************************************************************************************************************************
void CWavFile::openWav(char* wavfile)
{


	int i=0;
	bool bExit=false;

	infile.open (wavfile, ofstream::binary );

	if(infile.good() != 1)
	{
		cout<<  "***********************************"<<endl;
		cout<<  "  There is a problem with opening:  "<<endl;
		cout<<  "  "<<wavfile<<endl;
		cout<<  "  This file does not exist    "<<endl;
		cout<<  "***********************************"<<endl;
		exit(1);
	}

	infile.seekg (0, ios::end);									//send cursor to the end of the file
	length = infile.tellg();									//set the length to the current location
	infile.seekg (0, ios::beg);									//send cursor back to the beginning of the file

	buffer = new char [length];

	infile.read((char*)buffer,4);
	if (strncmp((char *) buffer, "RIFF", 4) != 0)
	{
		cout<<  "***********************************"<<endl;
		cout<<  "  There is a problem with opening:  "<<endl;
		cout<<  "  "<<wavfile<<endl;
		cout<<  "  File format is not wav   "<<endl;
		cout<<  "***********************************"<<endl;
		exit(1);
	}

	infile.seekg (8);
	infile.read((char*)buffer,4);
	if (strncmp((char *) buffer, "WAVE", 4) != 0) 
	{
		cout<<  "***********************************"<<endl;
		cout<<  "  There is a problem with opening:  "<<endl;
		cout<<  "  "<<wavfile<<endl;
		cout<<  "  File format is not wav   "<<endl;
		cout<<  "***********************************"<<endl;
		exit(1);
	}

	infile.seekg (0, ios::beg);
	while (bExit==false)
	{
		infile.read((char*)buffer,4);

		if (strncmp((char *) buffer, "fmt ", 4) == 0) 
		{
			fmtStart = i;
			validCheck++;
		}

		if (strncmp((char *) buffer, "data", 4) == 0) 
		{
			dataStart = i;
			validCheck++;
		}

		if (infile.eof())
		{
			bExit=true;
		}

		if(validCheck==2)
		{bExit=true;}
	
		i++;	
		infile.seekg(i);
	}


//	VALIDATE THE FILE
	if(validCheck!=2)
	{
		cout<<  "***********************************"<<endl;
		cout<<  "  Unsupported File         "<<endl;
		cout<<  "  Wave File Is Corrupt" <<endl;
		cout<<  "  Please Try Another File      "<<endl;
		cout<<  "***********************************"<<endl;
		exit(1);
	}

//	READ INFO FROM THE RIFF CHUNK
	infile.seekg (0);										//jump to offset from beginning of file
	infile.read((char *)&fileHeader,sizeof(fileHeader));	//read into the CFormatHeader instance

//	READ INFO FROM THE FORMAT CHUNK
	infile.seekg (fmtStart);							//jump to offset from beginning of file
	infile.read((char *)&fmtHeader,sizeof(fmtHeader));	//read into the CFormatHeader instance

	if (fmtHeader.getAudioFormat() != 1)
	{
		cout<<  "*******************************************************"<<endl;
		cout<<  "  Unsupported Wav Format      "<<endl;
		cout<<  "  Unable to read compression type  "<<endl<<endl;
		cout<<  "  This program supports compression type 1 files (PCM)"<<endl;
		cout<<	"  This file is compression type "<<fmtHeader.getAudioFormat();
		if(fmtHeader.getAudioFormat()==85)
		{cout<<" (MP3-Encoded-WAV)";}
		cout<<endl<<endl<<  "  Please Try Another File      "<<endl;
		cout<<  "*******************************************************"<<endl<<endl;
		exit(1);
	}

//	READ INFO FROM THE DATA CHUNK
	infile.seekg(dataStart);
	infile.read((char*)&dataHeader,sizeof(dataHeader));


//	READ THE ACTUAL SAMPLE DATA FROM THE DATA CHUNK
	infile.seekg(dataStart+8);	

	int length= (dataHeader.getDataChunkSize() / fmtHeader.getBlockAlign() * fmtHeader.getNumChannels()) ;

	numSamples=length;

	sampleData = new float[length];	//creates a new area of memory for the sample block
	for(i=0;i<length;i++)
	{sampleData[i]=0;}

	
	switch(fmtHeader.getBitsPerSample())
	{
	case 16:
		{	
			union uni			//union allows access to the same data in multiple forms
			{
				short s;		//here: as either a TWO BYTE short, referenced using dot operator & s
				struct bytes
				{
					char b0;	//or:   as TWO, SINGLE BYTE chars, referenced using dot operator & b
					char b1;
				}b;				
			}convertion_union;

			for(i=0; i < length; i++)		//loop for every sample
			{
				// read into short
				infile.read((char *)&convertion_union.b.b0,1);	//read ONE byte into the b0 variable
				infile.read((char *)&convertion_union.b.b1,1);	//read ONE byte into the b1 variable

				// convert to 1.0
				sampleData[i] = sixteenBitToFloat(convertion_union.s);		//read the b0 & b1 variables as a single SHORT
			}
			break;
		}

	case 24:
		{
			union uni24
			{
				int s;
				struct bytes
				{
					char b0;
					char b1;
					char b2;
					char b3;
				}b;
			}convertion_union24;

			for(i=0;i<length;i++)
			{
				infile.read((char*)&convertion_union24.b.b0,1);
				infile.read((char*)&convertion_union24.b.b1,1);
				infile.read((char*)&convertion_union24.b.b2,1);
				convertion_union24.b.b3=0;

				sampleData[i] = twentyfourBitToFloat(convertion_union24.s);
			}

			break;
		}

	case 32:
		{	
			union uni32
			{
				int s;
				struct bytes
				{
					char b0;
					char b1;
					char b2;
					char b3;
				}b;
			}convertion_union32;

			for(i=0;i<length;i++)
			{
				infile.read((char*)&convertion_union32.b.b0,1);
				infile.read((char*)&convertion_union32.b.b1,1);
				infile.read((char*)&convertion_union32.b.b2,1);
				infile.read((char*)&convertion_union32.b.b3,1);

				sampleData[i] = thirtytwoBitToFloat(convertion_union32.s);
			}
			break;
		}

	default:
		{
			cout<<  "***********************************"<<endl;
			cout<<  "  There is a problem reading:  "<<endl;
			cout<<  "  "<<wavfile<<endl;
			cout<<  "  You may only read 16, 24 or 32 bit files   "<<endl;
			cout<<  "  You tried to read a "<<fmtHeader.getBitsPerSample()<<" bit file"<<endl;
			cout<<  "***********************************"<<endl;
			exit(1);
			break;
		}
	}//end switch



	infile.close();
	validated=true;
}
//*************************************************************************************************************************
//*************************************************************************************************************************





//*************************************************************************************************************************
//SAVE A WAV FILE USING THE DATA CURRENTLY STORED (THREE CHUNK HEADERS, AND RAW SAMPLES)
//*************************************************************************************************************************
void CWavFile::saveWav(char* wavfile)
{
	outfile.open(wavfile, ofstream::binary);

	if(outfile.good() != 1)
	{
		cout << "****************************************************" << endl;
		cout << "There is a problem with opening "<< wavfile << " to write" << endl;
		cout << "Check destination directory" << endl;
		cout << "****************************************************" << endl;
		exit(1);
	}

	//write Chunk Info
	outfile.seekp (0);
	outfile.write((char *)&fileHeader,sizeof(fileHeader));
	outfile.seekp (/*fmtStart*/12);
	outfile.write((char *)&fmtHeader,sizeof(fmtHeader));
	outfile.seekp (/*dataStart*/36);
	outfile.write((char *)&dataHeader,sizeof(dataHeader));

	//write Raw Sample Data
	int length= ( dataHeader.getDataChunkSize() / fmtHeader.getBlockAlign() * fmtHeader.getNumChannels());
	
	switch(fmtHeader.getBitsPerSample())
	{
	case 16:
		{		
			short buffer = 0; 
			for(int i =0;i<length;i++)
			{
				// convert back to 16 bit
				buffer = floatToSixteenBit(sampleData[i]);
				outfile.write((char *)&buffer,sizeof(buffer));
	
			}	
			break;
		}
	case 24:
		{
			int buffer = 0;
			for(int i=0;i<length;i++)
			{
				buffer = floatToTwentyFourBit(sampleData[i]);
				outfile.write((char*)&buffer,3);
			}
			break;
		}
	case 32:
		{			
			int buffer = 0;
			for(int i=0;i<length;i++)
			{
				buffer = floatToThirtyTwoBit(sampleData[i]);
				outfile.write((char*)&buffer,4);
			}
			break;
		}
	default:
		{
			cout<<  "***********************************"<<endl;
			cout<<  "  There is a problem saving:  "<<endl;
			cout<<  "  "<<wavfile<<endl;
			cout<<  "  You may only write 16, 24 or 32 bit files   "<<endl;
			cout<<  "  You tried to write a "<<fmtHeader.getBitsPerSample()<<" bit file"<<endl;
			cout<<  "***********************************"<<endl;
			exit(1);
			break;
		}
	}
	
	writeFooter();
	outfile.write((char*)&endtext,28);

	outfile.close();

	validated = true;
}
//*************************************************************************************************************************
//*************************************************************************************************************************




//*************************************************************************************************************************
//CREATE A NEW WAV FILE USING HEADER INFORMATION PASSED TO THE FUNCTION
//*************************************************************************************************************************
void CWavFile::newWav(int sampleRate, short bitDepth, int lengthMS, short numChannels)
{

	fmtHeader.setFmtChunkSize(16);
	fmtHeader.setAudioFormat(1);
	fmtHeader.setNumChannels(numChannels);
	fmtHeader.setSampleRate(sampleRate);
	fmtHeader.setByteRate(sampleRate*bitDepth/8*numChannels);
	fmtHeader.setBlockAlign(bitDepth/8*numChannels);
	fmtHeader.setBitsPerSample(bitDepth);

	long	sizeSamples = (long)(lengthMS * 0.001 * sampleRate * numChannels);
	long	dataSize = (long)(lengthMS * 0.001 * sampleRate * fmtHeader.getBlockAlign());

	dataHeader.setDataChunkSize(dataSize);	//set up the data header size
	fileHeader.setChunkSize(dataSize+36);	//file header will be the data size plus 36 bytes

	// get rid of old stuff
	if(sampleData != 0 && numSamples>0)
	{
		float* start = &sampleData[0];
		sampleData = start;
		delete[] sampleData;
	}

	sampleData = new float[sizeSamples];

	// set to zero
	memset(sampleData,0,sizeSamples*sizeof(sampleData));

	numSamples=sizeSamples;
	validated = true;
}
//*************************************************************************************************************************
//*************************************************************************************************************************





//*************************************************************************************************************************
//RETURN A FLOAT VALUE FOR THE SPECIFIC SAMPLE SENT AS AN ARGUMENT
//*************************************************************************************************************************
float CWavFile::getSample(long position)
{	
	float sample = 0.0;

	checkFileOpened("getSample()");

	if(position < numSamples && position >= 0)
	{
		sample = sampleData[position];
	}
	else
	{
		cout << "*******************WARNING*******************" << endl;
		cout << "You are attempting to read out of range" << endl;
		cout << "The last sample is number: " << numSamples << endl;
		cout << "You have tried to read position " << position << endl;
		cout << "*********************************************" << endl;
	}
	return sample;
}
//*************************************************************************************************************************
//*************************************************************************************************************************





//*************************************************************************************************************************
//SET A FLOAT VALUE FOR THE SPECIFIC SAMPLE SENT AS AN ARGUMENT
//*************************************************************************************************************************
void CWavFile::setSample(float sample, long position)
{
	checkFileOpened("setSample()");

	if(position < numSamples)
	{
		if(sample <= 1.0 && sample >= -1.0)
		{
			sampleData[position] = sample;
		}
		else
		{
			sampleData[position] = 0.0;
			cout << "*******************WARNING*******************" << endl;
			cout << "You are attempting to write a value that is not between" << endl;
			cout << "1.0 and -1.0" << endl;
			cout << "Have you applied too much gain?" << endl;
			cout << "You sent a sample value: "<<sample<<endl;
			cout << "To position number: "<<position<<endl;
			cout << "*********************************************"<<endl;
			exit(1);
		}
	}
	else
	{
		cout << "*******************WARNING*******************" << endl;
		cout << "You are attempting to write beyond the last sample" << endl;
		cout << "The last sample is number: " << numSamples << endl;
		cout << "You have tried to write position: " << position << endl;
		cout << "*********************************************"<<endl;
		exit(1);
	}
}
//*************************************************************************************************************************
//*************************************************************************************************************************


float* CWavFile::getSampleData()
{
	return sampleData;
}



//*************************************************************************************************************************
//SET A FLOAT VALUE FOR THE SPECIFIC SAMPLE SENT AS AN ARGUMENT
//*************************************************************************************************************************
void CWavFile::setSampleData(float* data,long length)
{	
	
	dataHeader.setDataChunkSize(length * fmtHeader.getBitsPerSample()/8 );
	fileHeader.setChunkSize(length+36);

	// get rid of old stuff
	if(sampleData != 0 && numSamples>0)
	{
		float* start = &sampleData[0];
		sampleData = start;
		delete[] sampleData;
	}

	sampleData = new float[length];
	numSamples=length;

	for(int i=0;i<length;i++)
	{
		if(data[i] <= 1.0 && data[i] >= -1.0)
		{
			sampleData[i] = data[i];
		}
		else
		{
			sampleData[i] = 0.0;
			cout << "*******************WARNING*******************" << endl;
			cout << "The array of raw data you have sent includes a value that is not between" << endl;
			cout << "1.0 and -1.0" << endl;
			cout << "You sent a sample value: "<<data[i]<<endl;
			cout << "Located in array position: "<<i<<endl;
			cout << "*********************************************"<<endl;
			exit(1);
		}

	}
}
//*************************************************************************************************************************
//*************************************************************************************************************************





float CWavFile::sixteenBitToFloat(short input)
{
	float min = 32767.0;
	float max = 32768.0;
	float sample = 0.0;

	if(input > 0)
	{
		sample = (float)input/max;
	}
	else
	{
		sample = (float)input/min;
	}

	return sample;
}



short CWavFile::floatToSixteenBit(float input)
{
	float max = 32767.0;
	float min = 32768.0;
	float sample = 0.0;

	if(input > 0)
	{
		sample = (float)input*max;
	}
	else
	{
		sample = (float)input*min;
	}
	return sample;
}

float CWavFile::twentyfourBitToFloat(int input)
{
	float min = 8388608.0;
	float max = 8388607.0;
	float sample = 0.0;

	if(input > 0)
	{
		sample = (float)input/max;
	}
	else
	{
		sample = (float)input/min;
	}

	return sample;
}

int CWavFile::floatToTwentyFourBit(float input)
{
	float max = 8388607.0;
	float min = 8388608.0;
	float sample = 0.0;

	if(input > 0)
	{
		sample = (float)input*max;
	}
	else
	{
		sample = (float)input*min;
	}
	return sample;
}


float CWavFile::thirtytwoBitToFloat(int input)
{	
	double min = 2147483648.0;
	double max = 2147483647.0;
	double sample = 0.0;

	if(input > 0)
	{
		sample = (double)input/max;
	}
	else
	{
		sample = (double)input/min;
	}

	return sample;
}

int CWavFile::floatToThirtyTwoBit(float input)
{
	double max = 2147483647.0;
	double min = 2147483648.0;
	double sample = 0.0;

	if(input > 0)
	{
		sample = (double)input*max;
	}
	else
	{
		sample = (double)input*min;
	}
	return sample;
}

void CWavFile::checkFileOpened(char* command)
{
	if(validated != true)
	{
		cout<<  "***********************************"<<endl;
		cout<<  "No File Open           "<<endl<<endl;
		cout<<  "Unable To Process Command: "<<"'"<< command<<"'"<<endl<<endl;
		cout<<  "Please Open A WAV Before Processing"<<endl;
		cout<<  "***********************************"<<endl;
		exit(1);
	}
}


void CWavFile::displayWavInfo()
{
	checkFileOpened("displayChunkInfo()");

    fileHeader.displayFileData();
    fmtHeader.displayHeaderData();		//display the 'fmt ' chunk info 
	dataHeader.displayDataInfo();

}

long CWavFile::getNumSamples()
{
	checkFileOpened("getNumSamples()");

	return numSamples;
}

short CWavFile::getNumChannels()
{
	return fmtHeader.getNumChannels();
}

int CWavFile::getSampleRate()
{
	return fmtHeader.getSampleRate();
}

void CWavFile::writeFooter(void)
{
	endtext.endChunkID[0] = 'l';
	endtext.endChunkID[1] = 'i';
	endtext.endChunkID[2] = 's';
	endtext.endChunkID[3] = 't';

	endtext.endChunkSize=20;

	endtext.output[0] = 'C';
	endtext.output[1] = 'R';
	endtext.output[2] = 'E';
	endtext.output[3] = 'A';
	endtext.output[4] = 'T';
	endtext.output[5] = 'E';
	endtext.output[6] = 'D';
	endtext.output[7] = ' ';
	endtext.output[8] = 'A';
	endtext.output[9] = 'T';
	endtext.output[10] = ' ';
	endtext.output[11] = 'H';
	endtext.output[12] = 'U';
	endtext.output[13] = 'D';
	endtext.output[14] = 'D';
	endtext.output[15] = ' ';
	endtext.output[16] = 'U';
	endtext.output[17] = 'N';
	endtext.output[18] = 'I';
	endtext.output[19] = ' ';
}