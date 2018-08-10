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
    
    ffs._initFFS((const uint8_t **)pAddrList, pageCount, fileCount);

    //int i = 0;

    //while (i < pageCount) {

    //    //cout << "addr: " << hex << *(pAddrList)<<endl;
    //    //printf("\n addr: %x", *pAddrList);
    //    i++;
    //    pAddrList++;
    //}

    return FFS_SUCCESS;
}

uint8_t getFile(uint8_t fileID, uint8_t ** pData, uint16_t *pDataLen) {
    ffs._getFileData(fileID, pData, pDataLen);
    return 0;
}

uint8_t writeFile(uint8_t fileID, uint8_t * pData, uint16_t dataLen) {
    ffs._writeFileData(fileID, pData, dataLen);
    return 0;
}
