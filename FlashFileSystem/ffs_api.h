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
	typedef enum {
		FLASH_SUCCESS,      //!< FLASH_SUCCESS
		FLASH_OP_ERROR,     //!< FLASH_OP_ERROR
		FLASH_WRONG_ADDRESS //!< FLASH_WRONG_ADDRESS
	} FlashErrorCode;


	FlashErrorCode initFlashFileSystem(uint8_t **pAddrList, const uint8_t pageCount, const uint8_t fileCount);
	uint8_t getFile(uint8_t fileID, uint16_t **pData, uint16_t *pDataLen);
	uint8_t writeFile(uint8_t fileID, uint8_t *pData, uint16_t dataLen);

#ifdef __cplusplus
}
#endif

#endif /* FFS_API_H */

