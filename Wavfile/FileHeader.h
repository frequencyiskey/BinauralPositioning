// Matthew Wankling & Paul Lunn
//10/07/06
//FileHeader.h


#ifndef __MRWFILEHEADER__
#define __MRWFILEHEADER__

/**Class structure holds the 12 byte file information*/
class CFileHeader  
{
private:

	char chunkID[4];
	long chunkSize;
	char format[4];

public:

	CFileHeader();

	/**Outputs file chunk information to the screen*/
	void CFileHeader::displayFileData(void);

	void setChunkSize(long filesize);

};

#endif