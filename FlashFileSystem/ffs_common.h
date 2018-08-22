#ifndef FFS_COMMON_H
#define FFS_COMMON_H


/*  
ToDO: Discuss with Vazeer

FFS.H is internal header file. Ideally we should not expose this file to Application which used ffs lib.
FFS_COMMON.H will resolve this depedency.

All Struct, Enum, Macro used by application and ffs lib will be defined in this header file.

PAGE and FILE Config should also be defined in this file

Application should only require to include ffs_api.h and ffs_common.h

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>    

    /* Maximum File count */
#define MAX_FILE_COUNT          0x03

    /* Maximum PAGE count */
#define MAX_PAGE_COUNT          0x03

#define PAGE_SIZE                    (uint32_t)0x2000                /*! Maximum Page Size */
    
    /* used by ffs and FFS user, user should know this error */
    typedef enum {
        FFS_SUCCESS = 0,                    /*!< No Error */
        FFS_ERR_FILE_NOT_FOUND = 100,       /*!< File ID not found */
        FFS_ERR_DATA_SIZE_EXCEEDS,          /*!< Data Size exceed limit */
        FFS_ERR_FILE_ID_OUT_OF_RANGE,       /*!< File ID out of range */
        FFS_ERR_INVALID_FILE_ID,            /*!< Invalid File ID */
        FFS_ERR_WRITE_FAIL,                 /*!< Write Fail */
        FFS_ERR_ERASE_FAIL,                 /*!< Erase Fail */
        FFS_ERR_NO_SPACE_TO_WRITE,
        FFS_ERR_FAIL
    }FfsErrorCode;

#ifdef __cplusplus
}
#endif

#endif