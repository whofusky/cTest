/***********************************************************
 *
 * @file    IEventMeta.h
 *
 * @brief   IEventMeta头文件
 *
 * @author  
 *
 * @date    2022-09-30
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _I_IEVENTMETA_H_
#define _I_IEVENTMETA_H_

#define STORAGE_EVENT  "StorageEvent"
    #define STORAGE_FILE_ADDED      "FileAdded"
    #define STORAGE_FILE_DELETED    "FileDeleted"
    #define STORAGE_FILE_MOVEDTO    "MovedTo"
    #define STORAGE_FILE_MOVEDFROM  "MovedFrom"

#define SYSTEM_OPERATION_LOG  "SystemOperationLog"
    #define OPEN_FILE_ERROR      "OpenFileError"
    #define READ_FILE_ERROR      "ReadFileError"
    #define WRITE_FILE_ERROR     "writeFileError"
    #define CLOSE_FILE_ERROR     "CloseFileError"
    #define PARSE_FILE_ERROR     "ParseXMLError"

#endif//_I_IEVENTMETA_H_
