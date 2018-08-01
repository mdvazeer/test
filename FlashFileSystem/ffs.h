#pragma once

#ifndef FFS_H
#define FFS_H

#include <stdint.h>
#include <vector>

using namespace std;
enum ERROR {
	NO_ERROR = 0,
	ERR_FFS_FILE_NOT_FOUNT = 101,
	ERR_FFS_FILE_INTIGRITY_CHECK_FAIL,
	ERR_FFS_PAGE_ERASE_FAIL
};

typedef struct  {
	uint8_t pageID;
	uint16_t * baseAddress;
	uint16_t availableFreeMemory;
}FFS_PAGE;

typedef struct  {
	uint8_t magic;
	uint8_t fileID : 7;
	uint8_t ValidBit : 1;
	uint16_t sequenceNumber;
	uint16_t dataLen;
	uint8_t dataCrc;
	uint8_t headerCrc;
}FFS_FILE_HEADER;

typedef struct  {
	FFS_FILE_HEADER header;
	uint16_t * pDataAddress;
	uint16_t * pfileAddress;
}FFS_FILE;

class FFS {
public:
	uint8_t _initFFS(const uint16_t * AddressList, const uint8_t pageCount, const uint8_t fileCount);
	uint8_t _getFileData(const uint8_t fileID, uint16_t ** pData, uint16_t *pDataLen);
	uint8_t _writeFileData(const uint8_t fileID, const uint8_t * pData, const uint16_t dataLen);
	uint16_t *_makeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen);

private:
	uint8_t numberOfFiles, numberOfPages;
	vector<FFS_FILE> files;
	vector<FFS_PAGE> pages;
	uint8_t currentPage;

	bool isFileExist(uint8_t fileID);
	uint8_t cacheAllFiles(void);
	FFS_FILE_HEADER getHeader(uint8_t fileID, uint8_t * pData, uint16_t dataLen);
	uint8_t crc8(uint8_t * pData, uint16_t dataLen);
	uint8_t writeToFlash(const uint16_t * pTargetAddr, const uint16_t * pSourceAddr, const uint16_t dataLen);
	uint8_t writeFileToFlash(FFS_PAGE page, FFS_FILE file);
	uint8_t checkFileIntigrity(FFS_FILE file);
	uint8_t erasePage(FFS_PAGE page);

};


#endif /* FFS_H */
