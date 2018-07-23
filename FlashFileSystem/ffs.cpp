#include "stdafx.h"
/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   ffsClass.cpp
* Author: mdvazeer
*
* Created on July 18, 2018, 3:24 AM
*/

#include <stdint.h>
#include <vector>
#include <iterator>

#include "ffs.h"
#include "ffs_api.h"

#define MAGIC_NUMBER 0xCC
#define PAGE_READY 0xCC

uint8_t FFS::cacheAllFiles(void)
{
	//Go through each page
	for (auto &page : pages) {
		uint16_t *pageAddress = page.baseAddress;

		uint16_t pageReady = *pageAddress;
		if (pageReady == PAGE_READY) {

			//uint8_t magicNumber = *pageAddress; // Read Magic number;
			//if (magicNumber == MAGIC_NUMBER) {

			//}
		}
	}
	return 0;
}

bool FFS::isFileExist(uint8_t fileID) 
{
	for (auto file : files) {
		if (file.header.fileID == fileID) {
			return true;
		}
	}
	return false;
}

uint8_t FFS::writeFileToFlash(FFS_PAGE page, FFS_FILE file) {
	uint16_t *pTargetAddr;
	uint16_t *pSourceAddr;
	uint16_t dataLen;


	// Write Header before writing Data
	pTargetAddr = page.baseAddress + (8000 - page.availableFreeMemory);
	pSourceAddr = (uint16_t*)&file.header;
	dataLen = sizeof(FFS_FILE_HEADER);
	writeToFlash(pTargetAddr, pSourceAddr, dataLen);

	// Write Data after writing Header
	pTargetAddr += sizeof(FFS_FILE_HEADER);
	pSourceAddr = file.pDataAddress;
	dataLen = file.header.dataLen;
	writeToFlash(pTargetAddr, pSourceAddr, dataLen);

	return 0;
}

uint8_t FFS::checkFileIntigrity(FFS_FILE file) {

	FFS_FILE *pFileOnFlash = (FFS_FILE*)file.pfileAddress;
	FFS_FILE_HEADER *pHeaderOnFlash = (FFS_FILE_HEADER*)file.pfileAddress;
	uint16_t *pDataOnFlash = file.pfileAddress + sizeof(FFS_FILE_HEADER);
	uint16_t dataLen = pHeaderOnFlash->dataLen;
	
	uint8_t dataCrc = crc8((uint8_t*)pDataOnFlash, dataLen);
	uint8_t headerCrc = crc8((uint8_t*)pHeaderOnFlash, sizeof(FFS_FILE_HEADER));

	if (dataCrc != pHeaderOnFlash->dataCrc) {
		return ERR_FFS_FILE_INTIGRITY_CHECK_FAIL;
	}

	return NO_ERROR;
}

uint8_t FFS::writeToFlash(const uint16_t * pTargetAddr, const uint16_t * pSourceAddr, const uint16_t dataLen) {
	return 0;
}

FFS_FILE_HEADER FFS::getHeader(uint8_t fileID, uint8_t * pData, uint16_t dataLen) {

	FFS_FILE_HEADER header;

	header.magic = MAGIC_NUMBER;
	header.fileID = fileID;
	header.ValidBit = 0;
	header.dataLen = dataLen;
	header.sequenceNumber = 0;
	header.dataCrc = crc8(pData, dataLen);

	if (isFileExist(fileID) == NO_ERROR) {
		header.sequenceNumber = files[fileID].header.sequenceNumber + 1;
	}
	return header;
}

uint8_t FFS::crc8(uint8_t * pData, uint16_t dataLen) {
	uint8_t crc = 0;
	while (dataLen--) {
		crc ^= *pData++;
	}
	return crc;
}

uint8_t FFS::erasePage(FFS_PAGE page) {
	uint16_t *pageAddress = page.baseAddress;

	/*       Erase the page    */
	// eraseFlashPage(pageAddress);

	/*       write  PAGE_READY  on top of page*/
	// uint16_t data = PAGE_READY
	// write16(pageAddress, &data);

	/*        Validate PAGE_READY         */
	if (PAGE_READY != PAGE_READY) {
		return ERR_FFS_PAGE_ERASE_FAIL;
	}

	return NO_ERROR;
}


/********* API Calls *******/
uint8_t FFS::_initFFS(const uint16_t *pAddressList, const uint8_t pageCount, const uint8_t fileCount)
{
	numberOfFiles = fileCount;
	numberOfPages = pageCount;

	// cache all Page aadresses
	while (numberOfPages--) {
		FFS_PAGE page;
		page.baseAddress = reinterpret_cast<uint16_t*>(*pAddressList);
		pages.push_back(page);
		pAddressList++;
	}
	//cacheAllFiles();

	return 0;
}

uint8_t FFS::_getFileData(const uint8_t fileID, uint16_t ** pData, uint16_t *pDataLen)
{
	if (isFileExist(fileID)) {
		*pData = files[fileID].pDataAddress;
		*pDataLen = files[fileID].header.dataLen;
	}
	else {
		return ERR_FFS_FILE_NOT_FOUNT;
	}
	return NO_ERROR;
}

uint8_t FFS::_writeFileData(const uint8_t fileID, const uint8_t * pData, const uint16_t dataLen) {
	//Make Header

	//Make File

	return 0;
}

uint16_t *FFS::_makeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen) {

	FFS_FILE_HEADER header = getHeader(fileID, pData, dataLen);

	uint16_t fileSize = sizeof(FFS_FILE_HEADER) + header.dataLen;
	uint8_t *pFile = (uint8_t*)malloc(fileSize);

	void * p = memcpy(pFile, &header, sizeof(FFS_FILE_HEADER));
	memcpy(pFile + sizeof(FFS_FILE_HEADER), pData, header.dataLen);

	return (uint16_t*)pFile;
}
