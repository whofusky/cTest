/***********************************************************
*
*
*  IEventMeta.h
*
*  Create by muxd
*
*  2017/08/07
***********************************************************/

#define STORAGE_EVENT  "StorageEvent"
    #define STORAGE_FILE_ADDED      "FileAdded"
    #define STORAGE_FILE_DELETED    "FileDeleted"
    #define STORAGE_FILE_MOVEDTO    "MovedTo"
    #define STORAGE_FILE_MOVEDFROM  "MovedFrom"


#define SYSTEM_OPERATION_LOG  "SystemOperationLog"
    #define OPEN_FILE_ERROR     "OpenFileError"
    #define READ_FILE_ERROR	    "ReadFileError"
    #define WRITE_FILE_ERROR    "WriteFileError"
    #define CLOSE_FILE_ERROR    "CloseFileError"
    #define PARSE_XML_ERROR	    "ParseXMLError"

    #define INFRA_MESSAGE_QUEUE_FULL        "InfraMessageQueueFull"
    #define INFRA_CREATE_THREAD_ERROR       "InfraCreateThreadError"
    #define INFRA_START_THREAD_ERROR        "InfraStartThreadError"
    #define INFRA_STOP_THREAD_ERROR         "InfraStopThreadError"

    #define MERGE_BUSSINESSLIST_CONFLICT         "MergeBusinesslistConflict"
    #define MERGE_BUSSINESSLIST_ERROR            "MergeBusinesslistError"
    #define MERGE_BUSSINESSLIST_COORDINATE_ERROR "MergeBusinessCoordinateError"
    #define MERGE_BUSSINESSLIST_MODE_ERROR       "MergeBusinessModeError"

