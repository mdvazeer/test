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

#define MAGIC_NUMBER                 0xCC                            /*! Magic Number */
#define PAGE_READY_BYTE              0x00                            /*! Page Ready Byte */
#define FILE_ID_MIN                  1                               /*! Minimum File ID */       
#define FILE_ID_MAX                  127                             /*! Maximum File ID */
#define PAGE_SIZE                    (uint32_t)0x2000                /*! Maximum Page Size */
#define BACKUP_PAGE_BYTE             0x01

#define HEADER_SIZE                  (sizeof(FFS_FILE_HEADER))

/* Start Address of a page */
#define PAGE_START_ADDRESS(pageNum)             PAGE_SIZE * (pageNum)  

/* Read Page Ready Byte(1st Byte) from page */
#define GET_PAGE_READY_BYTE(pBaseAddress)       uint8_t(*(pBaseAddress + PAGE_READY_BYTE_POS))

/* Read Next Backup Page Index(2nd Byte) from page */
#define GET_NXT_BKUP_PAGE_INDEX(pBaseAddress)   uint8_t(*(pBaseAddress + NXT_BACKUP_PAGE_INDEX_POS))



/* internal functions */

bool FFS::isFileIdValid(uint8_t id)
{
    return ((id >= FILE_ID_MIN) && (id <= FILE_ID_MAX)) ? true : false;
}

bool FFS::isBackupPage(uint8_t* pAddress)
{
    return (GET_PAGE_READY_BYTE(pAddress) & BACKUP_PAGE_BYTE) ? true : false;
}

uint16_t FFS::getAvailableFreeMemory(uint8_t* pPageBaseAddress) const
{
    for (auto &page : pages)
    {
        if (pPageBaseAddress == page.baseAddress)
            return page.availableFreeMemory;
    }

    /* No info available for given page address*/
    return 0;
}

/* unused functions */
uint8_t* FFS::getPageStartAddress(uint8_t pageNum)
{
    uint8_t *pageStartAddress = reinterpret_cast<uint8_t *> (PAGE_START_ADDRESS(pageNum));

    return pageStartAddress;
}

/* unused functions */
uint8_t* FFS::getPageStartAddress(uint8_t* pAddress)
{
    uint8_t *pagestartaddress;
    /*
    Error	C2296	'&': illegal, left operand has type 'const uint8_t *'
    */
    pagestartaddress = reinterpret_cast<uint8_t *>((reinterpret_cast<uint32_t>(pAddress) & ~(0x1FFF)));

    return pagestartaddress;
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

/* CRC8 CCITT utilities */
uint8_t FFS::crc8(const uint8_t * pData, const uint16_t dataLen) {

    /* Table generated using polynomial: 0X07 */
    static const uint8_t CRC_TABLE[256] = {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
        0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
        0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
        0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
        0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
        0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
        0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
        0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
        0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
        0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
        0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
        0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
        0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
        0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
        0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
        0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
    };

    uint8_t crc = 0;
    uint8_t * pos = const_cast<uint8_t*>(pData);
    uint8_t temp = 0;

    while (temp < dataLen) {

        crc = CRC_TABLE[crc ^ *pos];
        pos++;
        temp++;
    }

    return crc;
}

bool FFS::ValidateFile(uint8_t* pFile)
{
    bool result = false;
    FFS_FILE_HEADER* pHeader = reinterpret_cast<FFS_FILE_HEADER*>(pFile);

    /* Magic Number */
    if (MAGIC_NUMBER != pHeader->magic)
    {
        return result;
    }

    /* Valid Bit */
    if (!(pHeader->ValidBit))
    {
        return result;
    }

    /* File Id */
    if (!(isFileIdValid(pHeader->fileID)))
    {
        return result;
    }

    /* validate Header CRC, Skip Header CRC byte in File Header */
    uint8_t crc = FFS::crc8(pFile, HEADER_SIZE - 1);
    if (crc != pHeader->headerCrc)
    {
        return result;
    }

    /* Data CRC */
    crc = FFS::crc8(&pFile[HEADER_SIZE], pHeader->dataLen);
    if (crc != pHeader->dataCrc)
    {
        return result;
    }

    result = true;

    return result;
}

/*
ToDo: Discuss with Vazeer

Assumption: After Magic number File header byte is present.
If Magic number present, increment pFile by header_size + data_len to point to next File in FFS
irrespective data is valid or not.
*/

uint8_t FFS::cacheAllFiles(void)
{
    /* Go through each page */
    for (auto &page : pages) {

        uint8_t *pageAddress = page.baseAddress;
        uint16_t availableFreeMemory = PAGE_SIZE;
        uint16_t validMemorySize = 0x00;

        //uint8_t pageReady = *pageAddress;

        /* number of byte processed/read in current page */
        uint16_t byteProcessed = 0;

        if (*pageAddress == PAGE_READY_BYTE) {

            uint8_t* pFile = page.baseAddress + 1;
            //uint8_t* pFile = page.baseAddress++;

            /* Minimum 8 byte of data should be present */
            while ((byteProcessed + HEADER_SIZE) < PAGE_SIZE) {

                /* get header info */
                FFS_FILE_HEADER* pHeader = reinterpret_cast<FFS_FILE_HEADER*>(pFile);

                /* get data position */
                uint8_t* pData = reinterpret_cast<uint8_t*>(&pFile[HEADER_SIZE]);

                if (ValidateFile(pFile)) {

                    //FFS_FILE_HEADER* pHeader = reinterpret_cast<FFS_FILE_HEADER*>(pFile);
                    //uint8_t* pData = reinterpret_cast<uint8_t*>(&pFile[HEADER_SIZE]);

                    FFS_FILE tmpFile;
                    memcpy((void*)&(tmpFile.header), (void*)pHeader, HEADER_SIZE);
                    tmpFile.pDataAddress = pData;
                    tmpFile.pfileAddress = pFile;
                    tmpFile.pageIndex = page.pageIndex;
                    files.push_back(tmpFile);

                    /* valid data size */
                    validMemorySize = validMemorySize + HEADER_SIZE + pHeader->dataLen;
                }

                /*
                ToDo: Discuss with Vazeer
                If Magic number is not present then increment pFile by one byte
                */

                /* update total byte processed/read */
                byteProcessed = byteProcessed + HEADER_SIZE + pHeader->dataLen;
                availableFreeMemory -= byteProcessed;

                /* point to next file
                next file = previous file location + header size + data size
                */
                pFile = pFile + HEADER_SIZE + pHeader->dataLen;
            }/*while*/

            /* update available free memory of page */
            page.availableFreeMemory = availableFreeMemory;

            /* upadte available valid data size */
            page.validMemorySize = validMemorySize;

            /* update Current Page */
            //currentPage = page;
        }
    }
    return 0;
}

FFS_FILE_HEADER FFS::createHeader(const uint8_t fileID, const uint8_t * pData, const uint16_t dataLen) {

    /*
    //Designated initialiser is not supported in this version of C++
    //https://blogs.msdn.microsoft.com/vcblog/2017/12/19/c17-progress-in-vs-2017-15-5-and-15-6/
    FFS_FILE_HEADER h = {
        .magic = 1;
        .fileID = fileID;
        .ValidBit = 0;
        .dataLen = dataLen;
        .sequenceNumber = 0;
        .dataCrc = crc8(pData, dataLen);
    };
    */

    FFS_FILE_HEADER header;

    header.magic = MAGIC_NUMBER;
    header.fileID = fileID;
    header.ValidBit = 1;
    header.dataLen = dataLen;
    header.sequenceNumber = 0;
    header.dataCrc = crc8(pData, dataLen);

    for (const auto &file : files) {

        if (file.header.fileID == fileID)
        {
            header.sequenceNumber = file.header.sequenceNumber + 1;
            break;
        }
    }

    return header;
}

FfsErrorCode FFS::getAddressToWrite(uint16_t dataLen, uint8_t** pNewAddress)
{
    /* Traversing from Start and see where data can be fit */
    for (auto &page : pages)
    {
        /* current page and backup page are not same */
        if (page.pageIndex != pBackupPage->pageIndex)
        {
            /* data length is less than available free memory */
            if (dataLen < page.availableFreeMemory)
            {
                /* return address to perform write */
                *pNewAddress = (page.baseAddress + (PAGE_SIZE - page.availableFreeMemory));
                return FFS_SUCCESS;
            }
        }
    }

    return FFS_ERR_NO_SPACE_TO_WRITE;
}
/********* API Calls *******/
uint8_t FFS::_initFFS(const uint8_t **pAddressList, const uint8_t pageCount, const uint8_t fileCount)
{
    numberOfFiles = MAX_FILE_COUNT;
    //numberOfPages = pageCount;

    uint8_t index = 0;
    uint8_t backupPageCount = 0;
    uint8_t pageReadyByte;
    uint8_t nextBkpUpPageIndex;
    //bool isCacheFileAllowed = false;

    /* set backupPage Index to last page index */
    uint8_t backupPageIndex = MAX_PAGE_COUNT - 1;

    /* cache all Page aadresses */
    while (numberOfPages < MAX_PAGE_COUNT) {

        FFS_PAGE page;

        /* get page start address*/
        page.baseAddress = const_cast<uint8_t*>(*pAddressList);

        /* Read 1st byte from page */
        uint8_t pageReadyByte = GET_PAGE_READY_BYTE(page.baseAddress); //uint8_t(*page.baseAddress);

        /* get page id*/
        page.pageNum = getPageNumber(page.baseAddress);

        /* store page index */
        page.pageIndex = index;

        /* Store page start address */
        pages.push_back(page);

        if (BACKUP_PAGE_BYTE == pageReadyByte)
        {
            /* increment backup page counter and update index */
            backupPageCount++;
            backupPageIndex = index;
        }

        /* next page address */
        pAddressList++;

        /* increment page count */
        numberOfPages++;

        index++;
    }

    /* RAW */
    if (backupPageCount == 0)
    {
        /*  backupPage index */
        backupPageIndex = MAX_PAGE_COUNT - 1;

        /* Iterate through each page, Erase Page if PageReady Byte is not present.
           Skip BackupPage
        */
        for (int i = 0; i < backupPageIndex; i++)
        {
            pageReadyByte = GET_PAGE_READY_BYTE(pages[i].baseAddress); //uint8_t(*(pages[i].baseAddress + PAGE_READY_BYTE_POS));

            if (0xFF == pageReadyByte)
            {
                /* Erase Page */
                erasePage(pages[i]);

                /* write page ready byte */
                uint8_t tempBuff[8] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
                writeToFlash(pages[index].baseAddress, &tempBuff[0], 8);
            }
        }

        /* Set last page as Backup Page */
        uint8_t tempBuff[8] = { BACKUP_PAGE_BYTE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        writeToFlash(pages[backupPageIndex].baseAddress, &tempBuff[0], 8);

        //isCacheFileAllowed = true;
    }
    else if (backupPageCount == 1)
    {
        /* Get Next backup Page index */
        nextBkpUpPageIndex = GET_NXT_BKUP_PAGE_INDEX(pages[backupPageIndex].baseAddress);

        /* Reclaim Procedure to continue */
        if (0xFF != nextBkpUpPageIndex)
        {
            /* ToDo: reconfirm with Vazeer */
            reclaimProcedureStart(nextBkpUpPageIndex, backupPageIndex);
            //isCacheFileAllowed = true;
        }
        else
        {
            //isCacheFileAllowed = true;
        }
    }
    /* Incomplete Reclaim Procedure */
    else if (backupPageCount > 1)
    {
        for (auto page : pages)
        {
            pageReadyByte = GET_PAGE_READY_BYTE(pages[backupPageIndex].baseAddress);
            nextBkpUpPageIndex = GET_NXT_BKUP_PAGE_INDEX(pages[backupPageIndex].baseAddress);
            //nextBkpUpPageIndex = uint8_t(*(pages[backupPageIndex].baseAddress + NXT_BACKUP_PAGE_INDEX_POS));

            if ((BACKUP_PAGE_BYTE == pageReadyByte) && (0xFF != nextBkpUpPageIndex))
            {
                (void)reclaimProcedureData(nextBkpUpPageIndex, backupPageIndex);
                (void)reclaimProcedureErase(nextBkpUpPageIndex, backupPageIndex);

                break;
            }
        }
    }

    /* Store all valid file from flash */
    if (numberOfPages)
        cacheAllFiles();

    return 0;
}

uint8_t FFS::_getFileData(const uint8_t fileID, uint8_t ** pData, uint16_t *pDataLen)
{
    uint8_t retVal = FFS_ERR_FILE_NOT_FOUND;

    /* Validate fileID(1-127) and check is files empty */
    if (!isFileIdValid(fileID))
    {
        retVal = FFS_ERR_INVALID_FILE_ID;
    }
    else if (!files.empty())
    {
        for (const auto &file : files) {

            if (file.header.fileID == fileID) {

                /* File Data address */
                *pData = file.pDataAddress;

                /*File Data len */
                *pDataLen = file.header.dataLen;

                retVal = FFS_SUCCESS;
                break;
            }
        }
    }
    else
    {
        /* Do Nothing */
    }

    return retVal;
}

FfsErrorCode FFS::getReclaimablePageIndex(uint8_t* pPageIndex, uint16_t newDataLen)
{
    FfsErrorCode retVal = FFS_ERR_FAIL;
    uint8_t index = 0x00;

    /* find page where data can be fit, skip backup page */
    for (index = (pBackupPage->pageIndex + 1); index < MAX_PAGE_COUNT; index++)
    {
        if (PAGE_SIZE > (pages[index].validMemorySize + newDataLen))
        {
            *pPageIndex = index;
            retVal = FFS_SUCCESS;
        }
    }

    for (index = 0; index < pBackupPage->pageIndex - 1; index++)
    {
        if (PAGE_SIZE > (pages[index].validMemorySize + newDataLen))
        {
            *pPageIndex = index;
            retVal = FFS_SUCCESS;
        }
    }

    return retVal;
}

FfsErrorCode FFS::reclaimProcedureErase(uint8_t reclaimPageIndex, uint8_t backupPageIndex)
{
    FfsErrorCode retVal = FFS_SUCCESS;
    /*Erase Reclaim Page */
    erasePage(pages[reclaimPageIndex]);

    /* Write Page Ready byte of reclaim page */
    uint8_t tempBuff[8] = { BACKUP_PAGE_BYTE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    writeToFlash(pages[reclaimPageIndex].baseAddress, &tempBuff[0], 8);

    /* Write page Ready to current backup page */
    memcpy(&tempBuff, pBackupPage->baseAddress, 8);
    tempBuff[0] = PAGE_READY_BYTE;
    writeToFlash(pBackupPage->baseAddress, &tempBuff[0], 8);

    /* update Pointer to backup Page*/
    pBackupPage = &pages[reclaimPageIndex];

    return retVal;
}

FfsErrorCode FFS::reclaimProcedureData(uint8_t reclaimPageIndex, uint8_t backupPageIndex)
{
    FfsErrorCode retVal = FFS_SUCCESS;

    uint8_t* pTargetAddress = pages[backupPageIndex].baseAddress;

    for (auto file : files)
    {
        if (file.pageIndex == reclaimPageIndex)
        {
            /* write valid file header + data together */
            uint16_t totalDataLen = file.header.dataLen + HEADER_SIZE;
            writeToFlash(pTargetAddress, (uint8_t*)&file.pfileAddress, totalDataLen);

            /* point to next address to write */
            pTargetAddress = pTargetAddress + totalDataLen;
        }
    }

    return retVal;
}

FfsErrorCode FFS::reclaimProcedureStart(uint8_t reclaimPageIndex, uint8_t backupPageIndex)
{
    FfsErrorCode retVal = FFS_SUCCESS;

    /* Write ReclaimPageIndex in BkpPageRegistry Byte of current backup page */
    uint8_t tmpBuff[8];
    memcpy(&tmpBuff, pBackupPage->baseAddress, 8);
    tmpBuff[1] = reclaimPageIndex;
    writeToFlash(pBackupPage->baseAddress, &tmpBuff[0], 8);

    /* Reclaim Data */
    retVal = reclaimProcedureData(reclaimPageIndex, backupPageIndex);

    /* Erase */
    retVal = reclaimProcedureErase(reclaimPageIndex, backupPageIndex);

    return retVal;
}

uint8_t FFS::_writeFileData(const uint8_t fileID, const uint8_t * pData, const uint16_t dataLen)
{

    uint8_t* pAddressToWrite;
    FfsErrorCode retVal = FFS_SUCCESS;

    /* validate File ID*/
    if (!isFileIdValid(fileID))
    {
        return FFS_ERR_INVALID_FILE_ID;
    }

    /*Validate Data Lenght */
    if (dataLen > PAGE_SIZE)
    {
        return FFS_ERR_DATA_SIZE_EXCEEDS;
    }

    /* Create File Header */
    FFS_FILE_HEADER header = createHeader(fileID, pData, dataLen);

    /* get address to wirte */
    retVal = getAddressToWrite(dataLen, &pAddressToWrite);

    if (FFS_ERR_NO_SPACE_TO_WRITE == retVal)
    {
        /* Reclaim Memory */
        uint8_t reclaimPageIndex = 0x00;
        retVal = getReclaimablePageIndex(&reclaimPageIndex, dataLen);
        if (FFS_SUCCESS == retVal)
        {
            /* Start Reclaim Procedure */
            retVal = reclaimProcedureStart(reclaimPageIndex, pBackupPage->pageIndex);
        }
        /* update backupPage */
        if (FFS_SUCCESS == retVal)
        {
            pBackupPage = &pages[reclaimPageIndex];
        }
    }

    /* Write to flash */
    if (FFS_SUCCESS == retVal)
    {
        /* prepare new file */
        FFS_FILE newFile;
        memcpy((uint8_t*)&newFile.header, (uint8_t*)&header, HEADER_SIZE);
        newFile.pfileAddress = pAddressToWrite;
        newFile.pDataAddress = pAddressToWrite + HEADER_SIZE;

        /* write data to flash */
        retVal = writeFileToFlash(newFile, pData, dataLen);

        /* Write Successful, update files and invalid old file data */
        if (FFS_SUCCESS == retVal)
        {
            /* traverse for file and erase previous file */
            for (int i = 0; i < files.size(); i++)
            {
                if (fileID == files[i].header.fileID)
                {
                    /* make previous file invalid */
                    files[i].header.ValidBit = 0;
                    writeFileToFlash(files[i], NULL, 0);

                    /* Erase file */
                    files.erase(files.begin() + i);
                    break;
                }
            }

            /* store updated file information */
            files.push_back(newFile);
        }
    }
    return 0;
}

FfsErrorCode FFS::writeFileToFlash(FFS_FILE &file, const uint8_t* pData, const uint16_t dataLen)
{
    FlashErrorCode retVal;

    /* Todo: Discuss with Vazeer
       Should data size be alweays multiple of 8, as write raw will write 8 byte minimum */

       /* write Header */
    uint8_t* pSourceAddr = (uint8_t*)&file.header;
    uint8_t* pTargetAddr = file.pfileAddress;
    retVal = writeToFlash(pTargetAddr, pSourceAddr, HEADER_SIZE);

    if (FLASH_SUCCESS != retVal)
    {
        return FFS_ERR_WRITE_FAIL;
    }

    if (dataLen > 0)
    {
        /* Write Data */
        pSourceAddr = const_cast<uint8_t*>(pData);
        pTargetAddr = file.pDataAddress;

        retVal = writeToFlash(pTargetAddr, pSourceAddr, dataLen);
        if (FLASH_SUCCESS != retVal)
        {
            return FFS_ERR_WRITE_FAIL;
        }

        /* validate write */
        if (!ValidateFile(file.pfileAddress))
        {
            return FFS_ERR_WRITE_FAIL;
        }
    }

    /* write succesful */
    return FFS_SUCCESS;
}

/* dummy function of FlasC class */
uint8_t FFS::getPageNumber(uint8_t *address) const
{
    return (reinterpret_cast<uint32_t>(address)) / PAGE_SIZE;
}

/* previous and unused functions */
uint16_t *FFS::_makeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen) {

    FFS_FILE_HEADER header = createHeader(fileID, pData, dataLen);

    uint16_t fileSize = sizeof(FFS_FILE_HEADER) + header.dataLen;
    uint8_t *pFile = (uint8_t*)malloc(fileSize);

    void * p = memcpy(pFile, &header, sizeof(FFS_FILE_HEADER));
    memcpy(pFile + sizeof(FFS_FILE_HEADER), pData, header.dataLen);

    return (uint16_t*)pFile;
}

uint8_t FFS::erasePage(FFS_PAGE& page)
{

    uint8_t *pageAddress = page.baseAddress;

    return FFS_SUCCESS;
}
//uint8_t FFS::writeFileToFlash(FFS_PAGE page, FFS_FILE file) {
//    uint8_t *pTargetAddr;
//    uint8_t *pSourceAddr;
//    uint16_t dataLen;
//
//
//    // Write Header before writing Data
//    pTargetAddr = page.baseAddress + (8000 - page.availableFreeMemory);
//    pSourceAddr = (uint8_t*)&file.header;
//    dataLen = sizeof(FFS_FILE_HEADER);
//    writeToFlash(pTargetAddr, pSourceAddr, dataLen);
//
//    // Write Data after writing Header
//    pTargetAddr += sizeof(FFS_FILE_HEADER);
//    pSourceAddr = file.pDataAddress;
//    dataLen = file.header.dataLen;
//    writeToFlash(pTargetAddr, pSourceAddr, dataLen);
//
//    return 0;
//}

//uint8_t FFS::checkFileIntigrity(FFS_FILE file) {
//
//    FFS_FILE *pFileOnFlash = (FFS_FILE*)file.pfileAddress;
//    FFS_FILE_HEADER *pHeaderOnFlash = (FFS_FILE_HEADER*)file.pfileAddress;
//    uint8_t *pDataOnFlash = file.pfileAddress + sizeof(FFS_FILE_HEADER);
//    uint16_t dataLen = pHeaderOnFlash->dataLen;
//
//    uint8_t dataCrc = crc8((uint8_t*)pDataOnFlash, dataLen);
//    uint8_t headerCrc = crc8((uint8_t*)pHeaderOnFlash, sizeof(FFS_FILE_HEADER));
//
//    if (dataCrc != pHeaderOnFlash->dataCrc) {
//        //return ERR_FFS_FILE_INTIGRITY_CHECK_FAIL;
//    }
//
//    return FFS_SUCCESS;
//}

/* Call FlashC api */
FlashErrorCode FFS::writeToFlash(const uint8_t * pTargetAddr, const uint8_t * pSourceAddr, const uint16_t dataLen) {
    return FLASH_SUCCESS;
}
