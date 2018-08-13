#pragma once

/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   ffs.h
* Author: mdvazeer
*
* Created on July 18, 2018, 3:13 AM
*/

#ifndef FFS_API_H
#define FFS_API_H
#include <stdint.h>    

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>    

    /* used by ffs and FFS user, user should know this error */
    typedef enum {
        FFS_SUCCESS = 0,                    /*!< No Error */
        FFS_ERR_FILE_NOT_FOUND = 100,       /*!< File ID not found */
        FFS_ERR_DATA_SIZE_EXCEEDS,          /*!< Data Size exceed limit */
        FFS_ERR_FILE_ID_OUT_OF_RANGE,       /*!< File ID out of range */
        FFS_ERR_INVALID_FILE_ID,            /*!< Invalid File ID */
        FFS_ERR_WRITE_FAIL,                 /*!< Write Fail */
        FFS_ERR_ERASE_FAIL,                 /*!< Erase Fail */
        FFS_ERR_NO_SPACE_TO_WRITE
    }FfsErrorCode;

    FfsErrorCode initFlashFileSystem(uint8_t **pAddrList, const uint8_t pageCount, const uint8_t fileCount);
    uint8_t getFile(uint8_t fileID, uint8_t **pData, uint16_t *pDataLen);
    uint8_t writeFile(uint8_t fileID, uint8_t *pData, uint16_t dataLen);

#ifdef __cplusplus
}
#endif

#endif /* FFS_API_H */

