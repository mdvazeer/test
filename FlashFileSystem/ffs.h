#pragma once

#ifndef FFS_H
#define FFS_H

#include <stdint.h>
#include <vector>
#include "ffs_api.h"

using namespace std;

/* gs: changed error name from ERROR to  "FfsErrorCode" 
    currently we are exposing ffs.h to user/test application, we should not exposed ffs.h 
    Below enum should be mentioned in separate header file
*/

#define MAX_FILE_COUNT          0x03
#define MAX_PAGE_COUNT          0x03

/*! Flash Block Format */
/*

+--------+--------+--------+--------+
|          Block Read Byte          |
+--------+--------+--------+--------+
|              Header               |
+--------+--------+--------+--------+
|           File A Data             |
+--------+--------+--------+--------+
|              Header               |
+--------+--------+--------+--------+
|           File B Data             |
+--------+--------+--------+--------+
|              Header               |
+--------+--------+--------+--------+
|           File C Data             |
+--------+--------+--------+--------+
|                                   |
+--------+--------+--------+--------+
|                                   |
+--------+--------+--------+--------+
|                                   |
+--------+--------+--------+--------+
|                                   |
+--------+--------+--------+--------+
*/

/*! File Header Format */
/*

7                               1   0
+--------+--------+--------+--------+
|            Magic Number           |
+--------+--------+--------+--------+
|             File Id           |val|
+--------+--------+--------+--------+
|           Sequence Number         |
+--------+--------+--------+--------+
|           Sequence Number         |
+--------+--------+--------+--------+
|             Data Length           |
+--------+--------+--------+--------+
|             Data Length           |
+--------+--------+--------+--------+
|             Data CRC              |
+--------+--------+--------+--------+
|             header CRC            |
+--------+--------+--------+--------+
|               Data                |
+--------+--------+--------+--------+

*/

/* LSB store first
   So while mapping to header structure we can directly read sequence and data length */
#define FILE_HEADER_ELE \
    _H(MAGIC_NUM,       "Magic Number") \
    _H(FILE_ID,         "File Identifier")  \
    _H(SEQ_NUM_LSB,     "Sequence Number")  \
    _H(SEQ_NUM_MSB,     "Sequence Number")  \
    _H(DATA_LEN_LSB,    "Data Length")  \
    _H(DATA_LEN_MSB,    "Data Length")  \
    _H(CRC_D,           "Data CRC") \
    _H(CRC_H,           "Header CRC")   \

#undef _H
#define _H(field, name) field##_POS,
typedef enum
{
    FILE_HEADER_ELE
    //HEADER_SIZE
    /* DATA_START_POS */
}headerElePos_t;


/* This is Flashc.cpp error code, Used to resolve build error */
typedef enum {
    FLASH_SUCCESS,      //!< FLASH_SUCCESS
    FLASH_OP_ERROR,     //!< FLASH_OP_ERROR
    FLASH_WRONG_ADDRESS //!< FLASH_WRONG_ADDRESS
} FlashErrorCode;

typedef struct  {
    uint8_t pageNum;                /*!< Page ID */
    uint8_t pageIndex;              /*!< Page Index */
    uint8_t * baseAddress;          /*!< Page Start Address */
    uint16_t availableFreeMemory;   /*!< Available Free Memory in Page */
    uint16_t validMemorySize;       /*!< Size of valid File information present in page */
}FFS_PAGE;

typedef struct  {
    uint8_t magic;                  /*!< Magic Number */
    uint8_t fileID : 7;             /*!< Field ID (1-127) */
    uint8_t ValidBit : 1;           /*!< File Info Validation Bit */
    uint16_t sequenceNumber;        /*!< File Sequence Number */
    uint16_t dataLen;               /*!< Data Length */
    uint8_t dataCrc;                /*!< Data CRC */
    uint8_t headerCrc;              /*!< Header CRC */
}FFS_FILE_HEADER;

typedef struct  {
    FFS_FILE_HEADER header;
    uint8_t pageIndex;
    uint8_t * pDataAddress;
    uint8_t * pfileAddress;
}FFS_FILE;

class FFS {
public:

    /*************************************************************************************************/
    /*!
    *  \brief      Inititalise Flash File System
    *
    *  \param      
    *
    *  \return     
    *
    *  Initialize the Flash Space used for storing/retrieving file information. 
    *  This function should only be called once upon system initialization. 
    *  This function fetches predefined static list of Page Address, number of pages and number of files
    */
    /*************************************************************************************************/
    uint8_t _initFFS(const uint8_t **AddressList, const uint8_t pageCount, const uint8_t fileCount);

    /*************************************************************************************************/
    /*!
    *  \brief     Get File Data from flash file system
    *
    *  \param       fileID      File ID
    *
    *  \param       pData       Hold address of data in flash
    *
    *  \param       pDataLen    Hold length of data available in flash
    *
    *  \return      return error code    
    *
    *  Get the file information of give fileID from Flash if exists, on successful read return 0 otherwise return error code.
    */
    /*************************************************************************************************/

    uint8_t _getFileData(const uint8_t fileID, uint8_t ** pData, uint16_t *pDataLen);
    
    /*************************************************************************************************/
    /*!
    *  \brief     Write File Data to Flash File System
    *
    *  \param       fileID      File ID
    *
    *  \param       pData       Hold address of data in flash
    *
    *  \param       pDataLen    Hold length of data available in flash
    *
    *  \return      return error code
    *
    *  Write the file data of given fileID on Flash, on successful write return 0 otherwise return error code.
    */
    /*************************************************************************************************/
    
    uint8_t _writeFileData(const uint8_t fileID, const uint8_t * pData, const uint16_t dataLen);
    
    uint16_t *_makeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen);


    FFS()
    {
        pBackupPage = nullptr;
        numberOfFiles = 0;
        numberOfPages = 0;
    }

    ~FFS()
    {
    
    }
private:
    uint8_t numberOfFiles;
    uint8_t numberOfPages;

    /* Holds all valid file information */
    vector<FFS_FILE> files;

    /* Holds all page information */
    vector<FFS_PAGE> pages;
    uint8_t currentPageNumber = 0;

    /* Holds Backup Page information, used during Reclaim Memory Procedure */
    FFS_PAGE backupPage;
    FFS_PAGE* pBackupPage;

    /* yet to decide exate use of currentPage.
    Currently it point to last Page whose PAGE_READY is set to 0x00

    currentPage update in cacheAllPage()
    */
    //FFS_PAGE currentPage;

    /* old file */
    FFS_FILE oldFile;

    /* internal functions */
    bool isFileExist(uint8_t fileID);
    bool ValidateFile(uint8_t* pFile);

    /*************************************************************************************************/
    /*!
    *  \brief     Check whether given page is backup page
    *
    *  \param       pPageBaseAddress    Page start address
    *
    *  \return      return true if it is backup page, otherwise false
    *
    *  This function return whether given page is set as backup page or not.
    */
    /*************************************************************************************************/
    bool isBackupPage(uint8_t* pPageBaseAddress);

    /*************************************************************************************************/
    /*!
    *  \brief     Available free memry
    *
    *  \param       pPageBaseAddress    Page start address
    *
    *  \return      reutn available free memory
    *
    *  This function return available free memory in given page.
    */
    /*************************************************************************************************/
    uint16_t getAvailableFreeMemory(uint8_t* pPageBaseAddress) const;

    bool isFileIdValid(uint8_t id);
    uint8_t* getPageStartAddress(uint8_t* pAddress);
    uint8_t* getPageStartAddress(uint8_t pageNum);
    FfsErrorCode getReclaimablePageIndex(uint8_t* pPageIndex, uint16_t newDataLen);
    uint8_t reclaimProcedureStart(uint8_t ReclaimPageIndex, uint8_t targetPageIndex);
    uint8_t reclaimProcedureErase(uint8_t ReclaimPageIndex, uint8_t targetPageIndex);
    uint8_t reclaimProcedureData(uint8_t ReclaimPageIndex, uint8_t targetPageIndex);
    uint8_t cacheAllFiles(void);
    FFS_FILE_HEADER createHeader(const uint8_t fileID, const uint8_t * pData, const uint16_t dataLen);
    uint8_t crc8(const uint8_t * pData, const uint16_t dataLen);
    FlashErrorCode writeToFlash(const uint8_t * pTargetAddr, const uint8_t * pSourceAddr, const uint16_t dataLen);
    FfsErrorCode writeFileToFlash(FFS_FILE &file, const uint8_t* pData, const uint16_t dataLen);
    //uint8_t writeFileToFlash(FFS_PAGE page, FFS_FILE file);
    //uint8_t checkFileIntigrity(FFS_FILE file);
    uint8_t erasePage(FFS_PAGE page);
    FfsErrorCode getAddressToWrite(uint16_t dataLen, uint8_t** pNewAddress);

    /* dummy function of Flash Class */
    uint8_t getPageNumber(uint8_t *address) const;
};

#endif /* FFS_H */
