// Matthew Wankling & Paul Lunn
//10/07/06
//DataHeader.cpp


#include "DataHeader.h"
#include <string.h>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDataHeader::CDataHeader()
{
	dataChunkID[0] = 'd';
	dataChunkID[1] = 'a';
	dataChunkID[2] = 't';
	dataChunkID[3] = 'a';

	dataChunkSize = 0;
}

void CDataHeader::displayDataInfo(void)
{
	cout << "---------------------------------"<<endl;
	cout << "Chunk ID: \t";	
	cout.write(dataChunkID,4);
	cout << endl<<"Chunk Size: \t" << dataChunkSize << endl;
	cout<< "---------------------------------"<<endl;
}

int CDataHeader::getDataChunkSize()
{
	return dataChunkSize;
}

void CDataHeader::setDataChunkSize(long size)
{
	dataChunkSize=size;
}