// FlashFileSystem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "ffs.h"
#include "ffs_api.h"
#include "ffs_common.h"
#include <string>
#include <stdint.h>

using namespace std;

#define NUMBER_OF_PAGES 3
#define NUMBER_OF_FILES  3

typedef bool(*FFS_TEST_CASE) (void);

#ifdef FFS_TEST
/* Test Case Registration */

#define TEST_CASES_LIST \
            /*  _TC(FFS_TC_P_INIT_01)   */  \
            /*  _TC(FFS_TC_P_INIT_02)   */  \
            /*  _TC(FFS_TC_P_INIT_03)   */  \
            /*  _TC(TEST_WRITE)         */  \
                _TC(EMPTY_TEST)             \

#undef _TC
/* Test Case Registration END */

/* Test Case Name List */
#define _TC(TC) #TC,

string TestCaseName[] = {
        TEST_CASES_LIST

};

#undef _TC
/* END */

/* Test Cases Declaration  Start */
#define _TC(TC)   bool TC (void);

TEST_CASES_LIST

#undef _TC
/* Declaration End */

/* Test Case Registration start */
#define _TC(TC)   TC,

FFS_TEST_CASE TestCases[] = {
    TEST_CASES_LIST
};
#undef _TC

/* Pages */
uint8_t* AddressList[MAX_PAGE_COUNT] = { NULL };

#define TEST_CASE_COUNT  (sizeof(TestCases) / sizeof(TestCases[0]))
#define PAGE_HEADER_SIZE    2

void CreatePages(void)
{
    for (int i = 0; i < MAX_PAGE_COUNT; i++)
    {
        AddressList[i] = (uint8_t*)malloc(PAGE_SIZE);
    }
}

void DestroyPages(void)
{
    for (int i = 0; i < MAX_PAGE_COUNT; i++)
    {
        free(AddressList[i]);
        AddressList[i] = NULL;
    }
}

void Startup(void)
{
    CreatePages();
}

void Cleanup(void)
{
    resetFFS();
    DestroyPages();
}


uint8_t crc8(const uint8_t * pData, const uint16_t dataLen) {

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

/* Test Case Implementaion */

bool EMPTY_TEST(void)
{
    return true;
}

bool FFS_TC_P_INIT_01(void)
{
    bool result = false;

    Startup();

    /* RAW */
    memset(AddressList[0], 0xFF, PAGE_SIZE);
    memset(AddressList[1], 0xFF, PAGE_SIZE);
    memset(AddressList[2], 0xFF, PAGE_SIZE);

    cout << "Run: " << __func__ << endl;

    (void)initFlashFileSystem(AddressList, MAX_PAGE_COUNT, MAX_FILE_COUNT);

    int pageCount = getPageCount();
    int fileCount = getValidFileCount();
    int backUpPageIndex = getBackUpPageIndex();

    if ((MAX_PAGE_COUNT == pageCount) && (0x00 == fileCount) && (MAX_PAGE_COUNT - 1 == backUpPageIndex))
    {
        result = true;
    }

    Cleanup();
    return result;
}

bool FFS_TC_P_INIT_02(void)
{
    uint16_t totalBuffSize = PAGE_HEADER_SIZE;
    bool result;
    uint8_t* pAddress;

    Startup();

    memset(AddressList[0], 0x00, PAGE_SIZE);
    memset(AddressList[1], 0x00, PAGE_SIZE);
    memset(AddressList[2], 0xFF, PAGE_SIZE);

    /* last page as backup page, */
    uint8_t tempBuff[8] = { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    memcpy(AddressList[2], &tempBuff, 8);

    uint8_t fileData1[] = {
        0x01, /* data */
        0x02, /* data */
        0x03, /* data */
        0x04, /* data */
        0x05, /* data */
        0x06, /* data */
        0x07, /* data */
        0x08, /* data */
        0x09, /* data */
        0x0A /* data */
    };

    FFS_FILE_HEADER fileHeader1;

    fileHeader1.magic = 0xCC;
    fileHeader1.fileID = 0x7F;
    fileHeader1.sequenceNumber = 1;
    fileHeader1.dataLen = sizeof(fileData1);
    fileHeader1.dataCrc = crc8(fileData1, sizeof(fileData1));
    fileHeader1.headerCrc = crc8((uint8_t*)&fileHeader1, sizeof(FFS_FILE_HEADER) - 1);

    fileHeader1.dataCrc = crc8(fileData1, sizeof(fileData1));
    fileHeader1.headerCrc = crc8((uint8_t*)&fileHeader1, sizeof(FFS_FILE_HEADER) - 1);

    pAddress= (uint8_t*)AddressList[0];

    memcpy((uint8_t*)(pAddress + totalBuffSize), (uint8_t*)&fileHeader1, sizeof(fileHeader1));
    totalBuffSize = totalBuffSize + sizeof(fileHeader1);

    memcpy((uint8_t*)(pAddress + totalBuffSize), (uint8_t*)&fileData1, sizeof(fileData1));
    totalBuffSize = totalBuffSize + sizeof(fileData1);

    cout << "Run: " << __func__ << endl;

    (void)initFlashFileSystem(AddressList, MAX_PAGE_COUNT, MAX_FILE_COUNT);

    int pageCount = getPageCount();
    int fileCount = getValidFileCount();
    int backUpPageIndex = getBackUpPageIndex();

    if ((MAX_PAGE_COUNT == pageCount) && (0x00 == fileCount) && (MAX_PAGE_COUNT - 1 == backUpPageIndex))
    {
        result = true;
    }

    /* compare file data */
    (void)getFile(0x7F, &pAddress, &totalBuffSize);
    if (0 == (memcmp(pAddress, fileData1, totalBuffSize)))
    {
        result = true;
    }

    Cleanup();
    return result;
}

bool FFS_TC_P_INIT_03(void)
{
    uint16_t totalBuffSize = PAGE_HEADER_SIZE;
    bool result;
    uint8_t* pAddress;

    Startup();

    memset(AddressList[0], 0x00, PAGE_SIZE);
    memset(AddressList[1], 0x00, PAGE_SIZE);
    memset(AddressList[2], 0xFF, PAGE_SIZE);

    /* last page as backup page, */
    uint8_t tempBuff[8] = { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    memcpy(AddressList[2], &tempBuff, 8);

    uint8_t fileData1[] = {
        0x01, /* data */
        0x02, /* data */
        0x03, /* data */
        0x04, /* data */
        0x05, /* data */
        0x06, /* data */
        0x07, /* data */
        0x08, /* data */
        0x09, /* data */
        0x0A /* data */
    };

    FFS_FILE_HEADER fileHeader1;

    fileHeader1.magic = 0xCC;
    fileHeader1.fileID = 0x7F; 
    fileHeader1.sequenceNumber = 1;
    fileHeader1.dataLen = sizeof(fileData1);
    fileHeader1.dataCrc = crc8(fileData1, sizeof(fileData1));
    fileHeader1.headerCrc = crc8((uint8_t*)&fileHeader1, sizeof(FFS_FILE_HEADER) - 1);

    fileHeader1.dataCrc = crc8(fileData1, sizeof(fileData1));
    fileHeader1.headerCrc = crc8((uint8_t*)&fileHeader1, sizeof(FFS_FILE_HEADER) - 1);

    pAddress = (uint8_t*)AddressList[1];

    memcpy((uint8_t*)(pAddress + totalBuffSize + 16), (uint8_t*)&fileHeader1, sizeof(fileHeader1));
    totalBuffSize = totalBuffSize + sizeof(fileHeader1);

    memcpy((uint8_t*)(pAddress + totalBuffSize + 16), (uint8_t*)&fileData1, sizeof(fileData1));
    totalBuffSize = totalBuffSize + sizeof(fileData1);

    cout << "Run: " << __func__ << endl;

    (void)initFlashFileSystem(AddressList, MAX_PAGE_COUNT, MAX_FILE_COUNT);

    int pageCount = getPageCount();
    int fileCount = getValidFileCount();
    int backUpPageIndex = getBackUpPageIndex();

    if ((MAX_PAGE_COUNT == pageCount) && (0x00 == fileCount) && (MAX_PAGE_COUNT - 1 == backUpPageIndex))
    {
        result = true;
    }

    /* compare file data */
    (void)getFile(0x7F, &pAddress, &totalBuffSize);
    if (0 == (memcmp(pAddress, fileData1, totalBuffSize)))
    {
        result = true;
    }

    Cleanup();
    return result;
}

#if 0
bool TEST_WRITE(void)
{
    {
        uint16_t totalBuffSize = PAGE_HEADER_SIZE;
        bool result = true;
        uint8_t* pAddress;

        Startup();

        memset(AddressList[0], 0x00, PAGE_SIZE);
        memset(AddressList[1], 0xFF, PAGE_SIZE);
        memset(AddressList[2], 0xFF, PAGE_SIZE);

        /* last page as backup page, */
        //uint8_t tempBuff[8] = { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        //memcpy(AddressList[2], &tempBuff, 8);

        FFS_FILE_HEADER fileHeader1;
        uint8_t fileData1[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13,
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,

            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,

            0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,

            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,

            0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6E
        };

        fileHeader1.magic = 0xCC;
        fileHeader1.sequenceNumber = 1;
        fileHeader1.dataLen = sizeof(fileData1);
        fileHeader1.fileID = 0x7F;
        fileHeader1.dataCrc = crc8(fileData1, sizeof(fileData1));
        fileHeader1.headerCrc = crc8((uint8_t*)&fileHeader1, sizeof(FFS_FILE_HEADER) - 1);

        pAddress = (uint8_t*)AddressList[0];

        memcpy((uint8_t*)(pAddress + totalBuffSize), (uint8_t*)&fileHeader1, sizeof(fileHeader1));
        totalBuffSize = totalBuffSize + sizeof(fileHeader1);

        memcpy((uint8_t*)(pAddress + totalBuffSize), (uint8_t*)&fileData1, sizeof(fileData1));
        totalBuffSize = totalBuffSize + sizeof(fileData1);

        cout << "Run: " << __func__ << endl;

        _TestWrite((uint8_t*)AddressList[1], pAddress, totalBuffSize);

        uint8_t numPaddingByte = 0x00;
        uint8_t offset = 0x00;

        uint8_t tempBuff[8] = { 0 };

        numPaddingByte = totalBuffSize & 7;
        cout << "numPaddingByte: " << (int)numPaddingByte << endl;

        if (numPaddingByte)
        {
            /* calculate target address on flash to write remaining data + padding */
            offset = (totalBuffSize - numPaddingByte);
            cout << "offset: " << (int)offset << endl;
            //pTargetAddr = file.pfileAddress + offset;

            memcpy(&tempBuff[0], (pAddress + offset), numPaddingByte);
        }

        _TestWrite((uint8_t*)(AddressList[1]+ offset), &tempBuff[0], 8);

        Cleanup();
        return result;
    }

}

#endif

#endif


int main(int argc, char** argv) {

#ifdef FFS_TEST
    int result[TEST_CASE_COUNT];
    int TestCasesToRun = TEST_CASE_COUNT - 1;

    cout << " ******** FFS TEST CASES ******** " << endl;
    cout << " Total Test Case count: " << TEST_CASE_COUNT << endl;
    cout << " Total Test Cases o Run: " << TestCasesToRun << endl;

    /* Execut Test Cases */
    for (int i = 0; i < TestCasesToRun; i++)
    {
        result[i] = (*TestCases[i])();
    }

    cout << "----------------------------------------------------------------" << endl;

    cout << "**** RESULT ****" << endl;
    cout << "----------------------------------------------------------------" << endl;

    for (int i = 0; i < TestCasesToRun; i++)
    {
        cout << TestCaseName[i] << "\t" << ((result[i]) ? "PASS" : "FAIL") << endl;
    }
    cout << "----------------------------------------------------------------" << endl;

    cout << "Test Cases Execution Completed " << endl << endl;
#endif
    //int *AddrList[] = {0x124, 0x125, 0x1222}

    //uint16_t AddrList[NUMBER_OF_PAGES] = { (uint16_t)page1, (uint16_t)page2, (uint16_t)page2 };
    uint8_t *AddrList[NUMBER_OF_PAGES] = { (uint8_t *)0x100000, (uint8_t *)0x102000, (uint8_t *)0x104000 };

    //(void)initFlashFileSystem(AddrList, NUMBER_OF_PAGES, NUMBER_OF_FILES);

    getchar();
    return 0;
}

