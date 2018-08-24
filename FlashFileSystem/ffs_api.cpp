#include "stdafx.h"
/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/
#include <iostream>
#include <cstdio>
#include "ffs_api.h"
#include "ffs.h"

using namespace std;

FFS ffs;

FfsErrorCode initFlashFileSystem(uint8_t **pAddrList, const uint8_t pageCount, const uint8_t fileCount) {
    
    return ffs._initFFS((const uint8_t **)pAddrList, pageCount, fileCount);

    //int i = 0;

    //while (i < pageCount) {

    //    //cout << "addr: " << hex << *(pAddrList)<<endl;
    //    //printf("\n addr: %x", *pAddrList);
    //    i++;
    //    pAddrList++;
    //}

    //return FFS_SUCCESS;
}

FfsErrorCode getFile(uint8_t fileID, uint8_t ** pData, uint16_t *pDataLen) {
    return ffs._getFileData(fileID, pData, pDataLen);
    //return 0;
}

FfsErrorCode writeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen) {
    return ffs._writeFileData(fileID, pData, dataLen);
    //return 0;
}

#ifdef FFS_TEST
int getPageCount()
{
    return ffs._getPageCount();
}

int getValidFileCount()
{
   return ffs._getValidFileCount();
}

int getBackUpPageIndex()
{
    return ffs._getBackUpPageIndex();
}

int resetFFS()
{
    return ffs._resetFFS();
}

void _TestWrite(uint8_t *address, const uint8_t *pAddress, const uint32_t size)
{
    int8_t         pageNumber = 1; // getPageNumber(address);
    FlashErrorCode errorCode = FLASH_WRONG_ADDRESS;
    bool           abort = false;
    /* these are defined in flashc.h */
    static const uint32_t ROW_SIZE = 0x0200;     // flash memory row (column) size 512 bytes
    static const uint32_t WRITE_SIZE = 0x0008;     // number of bytes for atomic flash write (minimum size)
    static const uint32_t PAGE_NUMBER_MASK = 0x7E000;
    static const uint32_t ADDRESS_OFFSET_MASK = 0x01FFF;
    static const uint32_t PAGE_NUMBER_OFFSET = 13;

    if (-1 != pageNumber)
    {
        //if (unlock(key))
        {

            uint32_t *pPageAddress = reinterpret_cast<uint32_t *>(address);
            uint32_t  numRows = size >> 9;     // number of full rows
            uint32_t  numWordRem = size & 0x01FF; // number of words in partial row. remainder

            for (uint8_t row = 0; (row < numRows); ++row)
            {
                //pRegisters->progErase.w = MODE_PROGRAM;
                //enableIrqProgErase();
                for (uint32_t word = 0; (word < ROW_SIZE / WRITE_SIZE); word++)
                {
                    uint32_t rack = (*pAddress++ & 0xFF) << 0;
                    rack |= static_cast<uint32_t>(*pAddress++) << 8;
                    rack |= static_cast<uint32_t>(*pAddress++) << 16;
                    rack |= static_cast<uint32_t>(*pAddress++) << 24;
                    *pPageAddress++ = rack;

                    rack = (*pAddress++ & 0xFF) << 0;
                    rack |= static_cast<uint32_t>(*pAddress++) << 8;
                    rack |= static_cast<uint32_t>(*pAddress++) << 16;
                    rack |= static_cast<uint32_t>(*pAddress++) << 24;
                    *pPageAddress++ = rack;
                    //abort = not waitReady();
                }
                //pRegisters->progErase.w = MODE_IDLE;
                //enableIrq(DISABLE_IRQ_EN);
            }
            // in case there is partial row and there are words remained to be written
            // ACHTUNG! IT IS USER RESPONSIBILITY TO ENSURE that "size" is divisible by 8 (WRITE_SIZE)

            //cout << "remaining word: " << (numWordRem % WRITE_SIZE) << endl;
            //cout << "remaining word: " << (numWordRem & 7)<<endl;
            if (numWordRem)
            {
                //pRegisters->progErase.w = MODE_PROGRAM;
                //enableIrqProgErase();
                uint32_t word = 0;
                uint32_t WordToWrite = numWordRem / WRITE_SIZE;



                for (word = 0; (word < WordToWrite); word++)
                {
                    uint32_t rack = (*pAddress++ & 0xFF) << 0;
                    rack |= static_cast<uint32_t>(*pAddress++) << 8;
                    rack |= static_cast<uint32_t>(*pAddress++) << 16;
                    rack |= static_cast<uint32_t>(*pAddress++) << 24;
                    *pPageAddress++ = rack;

                    rack = (*pAddress++ & 0xFF) << 0;
                    rack |= static_cast<uint32_t>(*pAddress++) << 8;
                    rack |= static_cast<uint32_t>(*pAddress++) << 16;
                    rack |= static_cast<uint32_t>(*pAddress++) << 24;
                    *pPageAddress++ = rack;
                    //abort = not waitReady();
                }
                //pRegisters->progErase.w = MODE_IDLE;
                //enableIrq(DISABLE_IRQ_EN);
            }
            errorCode = FLASH_SUCCESS;
        }
        //else
        //{
        //    errorCode = FLASH_OP_ERROR;
        //}
        //lock();
    }
    //return errorCode;

//return 0;
}
#endif