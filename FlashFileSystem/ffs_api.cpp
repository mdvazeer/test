#include "stdafx.h"
/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

#include "ffs_api.h"
#include "ffs.h"

using namespace std;

FFS ffs;

uint8_t initFlashFileSystem(const uint16_t *pAddrList, const uint8_t pageCount, const uint8_t fileCount) {
	return ffs._initFFS(pAddrList, pageCount, fileCount);
	return 0;
}

uint8_t getFile(uint8_t fileID, uint16_t ** pData, uint16_t *pDataLen) {
	return ffs._getFileData(fileID, pData, pDataLen);
	return 0;
}

uint8_t writeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen) {
	ffs._writeFileData(fileID, pData, dataLen);
	return 0;
}
