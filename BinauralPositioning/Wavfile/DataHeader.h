// Matthew Wankling & Paul Lunn
//10/07/06
//DataHeader.h


#ifndef __MRWDATAHEADER__
#define __MRWDATAHEADER__


/**Class structure holds the data chunk information*/
class CDataHeader  
{
private:

	char dataChunkID[4];
	int dataChunkSize;

public:

	CDataHeader();

	/**Outputs data chunk information to the screen*/
	void CDataHeader::displayDataInfo(void);

	int getDataChunkSize();
	void setDataChunkSize(long size);

};

#endif