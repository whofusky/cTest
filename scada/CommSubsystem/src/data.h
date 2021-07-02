/*************************************************************************************************
 *        File: data.h
 *  Created on: 2017-09-28
 *      Author: SunJianjun/QQ:37489753/WeChat:fatsunabc/Email:sunsjj@126.com
 *    Function: The head file for external use.
 *    Revision History:
 *      2018-07-12    Modify by SunJianjun.
 *      2018-06-05    Modify by SunJianjun.
 *      2018-05-30    Modify by SunJianjun.
 *      2018-05-08    Modify by SunJianjun.
 *      2018-01-24    Modify by SunJianjun.
 *      2017-12-20    Modify by SunJianjun.
 *      2017-11-04    Modify by SunJianjun.
 *      2017-10-25    Modify by SunJianjun.
 *
 * Examples:
 * extern DI_API int di_v;
 * DI_API void di_func(void);
 * class DI_API di_cls {}
 */

/*************************************************************************************************/
#ifndef DATA_H_
#define DATA_H_
/*************************************************************************************************/
// Windows/Linux.
#if (defined(_WIN32) || defined(_WIN64))
#define D_MSWIN
#endif

// Dll or not.
#if (defined(D_MSWIN) && defined(_USRDLL))
#define DI_API __declspec(dllexport)
#elif (defined(D_MSWIN) && !defined(_USRDLL))
#define DI_API __declspec(dllimport)
#else
#define DI_API
#endif

// NULL define.
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

// Macros for different platform.
#ifdef D_MSWIN
#define STR_LF                      "\r\n"
#define PATH_SEPRATER               '\\'
#define FILE_MACRO                  __FILE__
#define FUNC_MACRO                  __FUNCTION__
#define LINE_MACRO                  __LINE__
#else  // D_MSWIN
#define STR_LF                      "\n"
#define PATH_SEPRATER               '/'
#define FILE_MACRO                  __FILE__
#define FUNC_MACRO                  __func__
#define LINE_MACRO                  __LINE__
#endif  // D_MSWIN

/*************************************************************************************************/
#pragma pack(push,1)
/*************************************************************************************************/

// ===============================================================================================
// ==  General data define for data ID of data process.
// ===============================================================================================
/*
 * Define index for every field in ID,
 *   This is the ID_FIELD_NO for use.
 *   Any change of these define must be careful. 
 */
typedef enum _ID_FIELD
{
    IDFIELD_CONFIDENCE =0,// 0  B_Confidence[7:0]
    IDFIELD_INTERVAL,     // 1  B_Interval[7:0]
    IDFIELD_HEIGHT,       // 2  B_Height[7:0]
    IDFIELD_SOURCE0_2,    // 3  B_Source[2:0]
    IDFIELD_SOURCE3_7,    // 4  B_Source[7:3]
    IDFIELD_SOURCE,       // 5  B_Source[7:0]
    IDFIELD_KIND,         // 6  B_Kind[7:0]
    IDFIELD_BLENGTH,      // 7  B_Length[7:0]
    IDFIELD_AMOUNT,       // 8  B_Amount[7:0]
    IDFIELD_WLENGTH,      // 9  W_Length[15:0]
    IDFIELD_ENCODING0,    // 10 W_Encoding[0]
    IDFIELD_ENCODING1,    // 11 W_Encoding[1]
    IDFIELD_ENCODING2,    // 12 W_Encoding[2]
    IDFIELD_ENCODING3,    // 13 W_Encoding[3]
    IDFIELD_ENCODING4,    // 14 W_Encoding[4]
    IDFIELD_ENCODING5,    // 15 W_Encoding[5]
    IDFIELD_ENCODING6,    // 16 W_Encoding[6]
    IDFIELD_ENCODING0_6,  // 17 W_Encoding[6:0]
    IDFIELD_ENCODING7_9,  // 18 W_Encoding[9:7]
    IDFIELD_ENCODING10_11,// 19 W_Encoding[11:10]
    IDFIELD_ENCODING12_15,// 20 W_Encoding[15:12]
    IDFIELD_ENCODING0_9,  // 21 W_Encoding[9:0]
    IDFIELD_ENCODING0_11, // 22 W_Encoding[11:0]
    IDFIELD_ENCODING,     // 23 W_Encoding[15:0]
    IDFIELD_TYPE,         // 24 B_Type[7:0]
    IDFIELD_SERIAL,       // 25 B_Serial[7:0]
    IDFIELD_CATALOG0_5,   // 26 B_Catalog[5:0]
    IDFIELD_CATALOG6_7,   // 27 B_Catalog[7:6]
    IDFIELD_CATALOG,      // 28 B_Catalog[7:0]
    // ...
    // ...
    // ...
    IDFIELD_LAST,         // Append Above.
    IDFIELD_INVALID=63,   // 63 Invalid.  NEVER change
    IDFIELD_ALL           //              NEVER change
}ID_FIELD;

// Define for general catalog.
typedef enum _ID_CATALOG
{
    IDCATALOG_FILE=0,        // File.
    IDCATALOG_SQL,           // SQL.
    IDCATALOG_DATA,          // Data.
    IDCATALOG_SETS,          // Sets.
    IDCATALOG_INVALID=0xFF   //
}ID_CATALOG;

// Define for file catalog.
typedef enum _FILE_CATALOG
{
    FILECATALOG_NWP=0x00,      // NWP file.
    FILECATALOG_STF=0x04,      // Short-term forecast file.
    FILECATALOG_UTF=0x08,      // Ultra short-term forecast file.
    FILECATALOG_TIF=0x0C,      // Turbine information file for report.
    FILECATALOG_NWP24_RPT=0x10,// 24-hour NWP file for Report.
    FILECATALOG_NWP72_RPT=0x14,// 72-hour NWP file for Rerpot.
    FILECATALOG_STF_RPT=0x18,  // Short-term forecast file for Report.
    FILECATALOG_UTF_RPT=0x1C,  // Ultra short-term forecast file for Report.
    FILECATALOG_TPF_RPT=0x20   // Theoretical power file for Report.
}FILE_CATALOG;

// Define for SQL catalog.
typedef enum _SQL_CATALOG
{
    SQLCATALOG_SELECT_TABLE=0x00,  // The table name for SELECT statement of SQL.
    SQLCATALOG_SELECT_FIELD,       // The fields list for SELECT statement of SQL.
    SQLCATALOG_SELECT_WHERE,       // The where clause for SELECT statement of SQL.
    SQLCATALOG_SELECT_GROUP,       // The group clause for SELECT statement of SQL.
    SQLCATALOG_SELECT_ORDER,       // The order clause for SELECT statement of SQL.
    SQLCATALOG_SELECT_RESULT=0x07, // The result of the SELECT statement.
    SQLCATALOG_INSERT_TABLE=0x08,  // The table name for INSET INTO statement of SQL.
    SQLCATALOG_INSERT_FIELD,       // The fields list for INSET INTO statement of SQL.
    SQLCATALOG_INSERT_VALUE,       // The values list for INSET INTO statement of SQL.
    SQLCATALOG_UPDATE_TABLE=0x10,  // The table name for UPDATE statement of SQL.
    SQLCATALOG_UPDATE_SET,         // The SET list for UPDATE statement of SQL.
    SQLCATALOG_UPDATE_WHERE,       // The where clause for UPDATE statement of SQL.
}SQL_CATALOG;

// Define for data catalog.
typedef enum _DATA_CATALOG
{
    DATACATALOG_INVALID=0x00,
    DATACATALOG_WS,              // Wind Speed.         
    DATACATALOG_WD,              // Wind Direction.     
    DATACATALOG_AT,              // Air Temperature.    
    DATACATALOG_AH,              // Air Humidity.       
    DATACATALOG_AD,              // Air Density.        
    DATACATALOG_AP,              // Air Pressure.       
    DATACATALOG_BL,              // Battery Level.      
    DATACATALOG_CT,              // Case Temperature.   
    DATACATALOG_RF,              // Rainfall.           
    DATACATALOG_TA,              // Turbine Amount.     
    DATACATALOG_EC,              // Encoding.           
    DATACATALOG_HT,              // Height.             
    DATACATALOG_SD,              // Save Date.          
    DATACATALOG_DD,              // Data Date.          
    DATACATALOG_FD,              // File Date.          
    DATACATALOG_PD,              // Prediction Date.    
    DATACATALOG_DS,              // Date of Start.
    DATACATALOG_DT,              // Date of Stop.
    DATACATALOG_NT,              // Northern Latitude.  
    DATACATALOG_ET,              // East Longitude.     
    DATACATALOG_SL,              // Sea Level.          
    DATACATALOG_PP,              // Active Power.       
    DATACATALOG_PQ,              // Reactive Power.     
    DATACATALOG_CP,              // Capacity.           
    DATACATALOG_GE,              // Generated Energy.   
    DATACATALOG_TP,              // Theory Power.       
    DATACATALOG_TE,              // Theory Energy.      
    DATACATALOG_SF,              // State Flag.         
    DATACATALOG_PF,              // Power Factor.       
    DATACATALOG_FQ,              // Frequency.          
    DATACATALOG_RS,              // Rotate Speed.       
    DATACATALOG_PA,              // Pitch Angle.        
    DATACATALOG_YA,              // Yaw Angle.          
    DATACATALOG_PV,              // Voltage.            
    DATACATALOG_PI,              // Current.            
    DATACATALOG_TS,              // Time Span.          
    DATACATALOG_FN,              // File Name.          
    DATACATALOG_TN,              // Table Name.         
    DATACATALOG_PM,              // Product Model.      
    DATACATALOG_SN,              // Sequence Number.    
    DATACATALOG_NM,              // Name.               
    DATACATALOG_ID,              // ID.                 
    DATACATALOG_SC,              // Score.              
    DATACATALOG_IS,              // Information String. 
    DATACATALOG_VP,              // Available Power/Valid Power.
    DATACATALOG_BP,              // Blocked Power.
    DATACATALOG_CM,              // Computing Method.
    DATACATALOG_OT,              // Oil Temperature.
    DATACATALOG_BT,              // Bearing Temperature.
    DATACATALOG_AS,              // Accelerated Speed.
    DATACATALOG_LAST,            // The last one.
    DATACATALOG_ALL=256,         // All values with invalid.
    DATACATALOG_VALID            // All values without invalid.
}DATA_CATALOG;

// Define for data type.
typedef enum _DATA_TYPE
{
    DATATYPE_BOOL      = 0x00,  // bit7=0B,bit6-2=00000B;
    DATATYPE_UINT8     = 0x04,  // bit7=0B,bit6-2=00001B;
    DATATYPE_SINT8     = 0x08,  // bit7=0B,bit6-2=00010B;
    DATATYPE_UINT16    = 0x0C,  // bit7=0B,bit6-2=00011B;
    DATATYPE_SINT16    = 0x10,  // bit7=0B,bit6-2=00100B;
    DATATYPE_UINT32    = 0x14,  // bit7=0B,bit6-2=00101B;
    DATATYPE_SINT32    = 0x18,  // bit7=0B,bit6-2=00110B;
    DATATYPE_FLOAT16   = 0x1C,  // bit7=0B,bit6-2=00111B;
    DATATYPE_FLOAT32   = 0x20,  // bit7=0B,bit6-2=01000B;
    DATATYPE_DATE7B    = 0x24,  // bit7=0B,bit6-2=01001B;
    DATATYPE_UINT64    = 0x28,  // bit7=0B,bit6-2=01010B;
    DATATYPE_SINT64    = 0x2C,  // bit7=0B,bit6-2=01011B;
    DATATYPE_FLOAT64   = 0x30,  // bit7=0B,bit6-2=01100B;
    DATATYPE_POINTER64 = 0x34,  // bit7=0B,bit6-2=01101B;
    DATATYPE_POINTER32 = 0x38,  // bit7=0B,bit6-2=01110B;
    DATATYPE_ASCII     = 0x7C,  // bit7=0B,bit6-2=11111B;
    DATATYPE_GBK,
    DATATYPE_UTF8,
    DATATYPE_CUSTOM1   = 0x80,  // bit7=1B,bit6-2=00000B;
    //
    DATATYPE_INVALID   = 0xFF,  //
    //
    DATATYPE_ALL,               // All values with invalid.
    DATATYPE_VALID              // All values without invalid.
}DATA_TYPE;

// Define for encoding type.
typedef enum _DATA_ENCODETYPE
{
    ENCODETYPE_INVALID=0,
    ENCODETYPE_FARM,                   // Farm.
    ENCODETYPE_MODEL,                  // Model.
    ENCODETYPE_STAGE,                  // Stage.
    ENCODETYPE_TURBINE,                // Turbine.
    ENCODETYPE_AMT,                    // Anemometer Tower.
    ENCODETYPE_NWP,                    // Numerical Weather Prediction.
    ENCODETYPE_TSS,                    // Turbine Supervisor System.
    ENCODETYPE_TPS,                    // Theory Power System.
    ENCODETYPE_EMS,                    // Energy Manage System.
    ENCODETYPE_IAS,                    // Integrated Automation System.
    ENCODETYPE_AGC,                    // AGC.
    ENCODETYPE_ALL=256,                // All values with invalid.
    ENCODETYPE_VALID                   // All values without invalid.
}DATA_ENCODETYPE;

// Define for encoding serial number.
typedef enum _DATA_ENCODESN
{
    ENCODESN_IDINVALID=0,              //
    ENCODESN_LAST=65535,
    ENCODESN_INVALID,                  //
    ENCODESN_ALL,                      // All values with invalid.
    ENCODESN_VALID                     // All values without invalid.
}DATA_ENCODESN;

// Define for encoding method.
typedef enum _DATA_ENCODEMETHOD
{
    ENCODEMETHOD_ORIGINAL=0,           //
    ENCODEMETHOD_SAMPLE,               //
    ENCODEMETHOD_MEAN,                 //
    ENCODEMETHOD_CONFIG                //
}DATA_ENCODEMETHOD;

// Define for data kind.
typedef enum _DATA_KIND
{
    DATAKIND_INVALID = 0,              // Invalid.
    DATAKIND_RTV,                      // Real-time Value.
    DATAKIND_AVG,                      // Average.
    DATAKIND_MAX,                      // Maximum value.
    DATAKIND_MIN,                      // Minimum value.
    DATAKIND_AVG2,                     // Average 2.
    DATAKIND_PLV,                      // Plan Value.
    DATAKIND_EXV,                      // Execute Value.
    DATAKIND_THV,                      // Threshold Value.
    DATAKIND_UTV,                      // Upper Threshold Value.
    DATAKIND_LTV,                      // Lower Threshold Value.
    DATAKIND_MDV,                      // Median Value.
    DATAKIND_SDV,                      // Standard Deviation Value.
    DATAKIND_3SAVG,                    // 3s Average.
    DATAKIND_OGNPLV,                   // Original Plan Value.
    DATAKIND_ILD,                      // Installed Value.
    DATAKIND_PGG,                      // Power generating value.
    DATAKIND_WWG,                      // Value of waiting for wind.
    DATAKIND_SPG,                      // Value of stopping value.
    DATAKIND_CBG,                      // Value of communication breaking.
    DATAKIND_TGV,                      // Theory Generating Value.
    DATAKIND_UGV,                      // Usable Generating Value.
    DATAKIND_IBV,                      // Inner Block Value.
    DATAKIND_OBV,                      // Outer Block Value.
    DATAKIND_CWSCLV,                   // Calculated value with the cabin wind speed.
    DATAKIND_PTBCLV,                   // Calculated value with prototype turbine.
    DATAKIND_OGN,                      // Original Value.
    DATAKIND_STD,                      // Stand Value.
    DATAKIND_DPH,                      // Dispatch Value.
    DATAKIND_NRV,                      // Nominal Rating Value.
    DATAKIND_AMV,                      // Accumulated Value.
    DATAKIND_CLV,                      // Calculated Value.
    DATAKIND_AGV,                      // Actual Generating Value.
    DATAKIND_WAV,                      // Wind Abandoning Value.
    DATAKIND_PGGTHV,                   // Power generating Threshold value.
    DATAKIND_PGGAMV,                   // Power generating Accumulated value.
    DATAKIND_OVH,                      // Overhaul value.
    DATAKIND_TLN,                      // Value for Transmission Line.
    DATAKIND_LMT,                      // Value for Power Limited.
    // Other.
    DATAKIND_CTF=128,                  // Current Transformer.
    DATAKIND_BLD,                      // Blades.
    DATAKIND_PSA,                      // Phase A.
    DATAKIND_PSB,                      // Phase B.
    DATAKIND_PSC,                      // Phase C.
    DATAKIND_MAXCT,                    // Maximum Case Temperature.
    DATAKIND_MINCT,                    // Minimum Case Temperature.
    DATAKIND_MAXBL,                    // Maximum Battery Level.
    DATAKIND_MINBL,                    // Minimum Battery Level.
    DATAKIND_MAXPP,                    // Maximum Active Power.
    DATAKIND_MINPP,                    // Minimum Active Power.
    DATAKIND_MAXWS,                    // Maximum Wind Speed.
    DATAKIND_MINWS,                    // Minimum Wind Speed.
    DATAKIND_MAXWD,                    // Maximum Wind Direction.
    DATAKIND_MINWD,                    // Minimum Wind Direction.
    DATAKIND_MAXAT,                    // Maximum Air Temperature.
    DATAKIND_MINAT,                    // Minimum Air Temperature.
    DATAKIND_MAXAH,                    // Maximum Air Humidity.
    DATAKIND_MINAH,                    // Minimum Air Humidity.
    DATAKIND_MAXAD,                    // Maximum Air Density.
    DATAKIND_MINAD,                    // Minimum Air Density.
    DATAKIND_MAXAP,                    // Maximum Air Pressure.
    DATAKIND_MINAP,                    // Minimum Air Pressure.
    DATAKIND_3SAVG1,                   // 3s Average 1.
    DATAKIND_3SAVG2,                   // 3s Average 2.
    DATAKIND_3SAVG3,                   // 3s Average 3.
    DATAKIND_3SAVG4,                   // 3s Average 4.
    DATAKIND_3SAVG5,                   // 3s Average 5.
    DATAKIND_3SAVG6,                   // 3s Average 6.
    DATAKIND_3SAVG7,                   // 3s Average 7.
    DATAKIND_3SAVG8,                   // 3s Average 8.
    DATAKIND_3SAVG9,                   // 3s Average 9.
    DATAKIND_3SAVG10,                  // 3s Average 10.
    DATAKIND_3SAVG11,                  // 3s Average 11.
    DATAKIND_3SAVG12,                  // 3s Average 12.
    DATAKIND_3SAVG13,                  // 3s Average 13.
    DATAKIND_3SAVG14,                  // 3s Average 14.
    DATAKIND_3SAVG15,                  // 3s Average 15.
    DATAKIND_3SAVG16,                  // 3s Average 16.
    DATAKIND_3SAVG17,                  // 3s Average 17.
    DATAKIND_3SAVG18,                  // 3s Average 18.
    DATAKIND_3SAVG19,                  // 3s Average 19.
    DATAKIND_3SAVG20,                  // 3s Average 20.
    DATAKIND_HEIGHT,                   // Height.
    DATAKIND_GEARBOX,                  // Gear Box.
    DATAKIND_GENERATOR,                // Generator.
    // Special.
    DATAKIND_LAST,
    DATAKIND_ALL=256,                  // All values with invalid.
    DATAKIND_VALID                     // All values without invalid.
}DATA_KIND;

// Define for source type.
typedef enum _DATA_SOURCETYPE
{
    SOURCETYPE_INVALID=0,
    SOURCETYPE_AMT,                    // Anemometer Tower.
    SOURCETYPE_TSS,                    // Turbine Supervisory System.
    SOURCETYPE_IAS,                    // Integrated Automation System.
    SOURCETYPE_NWP,                    // Numerical Weather Prediction.
    SOURCETYPE_STF,                    // Short-term Forecast.
    SOURCETYPE_UTF,                    // Ultra Short-term Forecast.
    SOURCETYPE_TPS,                    // Theory Power System.
    SOURCETYPE_HND,                    // Manual Input. (Input by hand.)
    SOURCETYPE_EMS,                    // Energy Manage System.
    SOURCETYPE_AGC,                    // AGC.
    SOURCETYPE_PLN,                    // Plan.
    SOURCETYPE_SBI,                    // System Basic Information.
    SOURCETYPE_CFG,                    // Configuration.
    SOURCETYPE_DPH,                    // Dispatch.
    SOURCETYPE_LAST,                   //
    SOURCETYPE_ALL=256,                // All values with invalid
    SOURCETYPE_VALID                   // All values without invalid
}DATA_SOURCETYPE;

// Define for source serial number.
typedef enum _DATA_SOURCESN
{
    SOURCESN_IDINVALID=0,
    SOURCESN_LAST=65535,
    SOURCESN_INVALID,
    SOURCESN_ALL,                      // All values with invalid.
    SOURCESN_VALID                     // All values without invalid.
}DATA_SOURCESN;

// Define for source method.
typedef enum _DATA_SOURCEMETHOD
{
    SOURCEMETHOD_ORIGINAL=0,           //
    SOURCEMETHOD_SAMPLE,               //
    SOURCEMETHOD_AVERAGE,              //
    SOURCEMETHOD_CONFIG                //
}DATA_SOURCEMETHOD;

// Define height field.
typedef enum _HEIGHT_FIELD
{
    HEIGHTFIELD_TYPE=0,                //
    HEIGHTFIELD_NUMBER,                //
    HEIGHTFIELD_LAST                   //
}HEIGHT_FIELD;

// Define for height type.
typedef enum _DATA_HEIGHT
{
    HEIGHTTYPE_HUB=1,         // The height of turbine hub.
    HEIGHTTYPE_SL,            // The height from sea level.
    HEIGHTTYPE_ES,            // The height from earth surface, Unit 1 meter.
    HEIGHTTYPE_INVALID=0xFF,  // The value is invalid.
    HEIGHTTYPE_ALL,           // All values with invalid.
    HEIGHTTYPE_VALID          // All values without invalid.
}DATA_HEIGHTTYPE;

// Define for height value.
typedef enum _DATA_HEIGHTVALUE
{
    HEIGHTVALUE_LAST=65535,
    HEIGHTVALUE_INVALID,
    HEIGHTVALUE_ALL,          // All values with invalid.
    HEIGHTVALUE_VALID         // All values without invalid.
}DATA_HEIGHTVALUE;

// Define interval field.
typedef enum _INTERVAL_FIELD
{
    INTERVALFIELD_UNIT=0,     //
    INTERVALFIELD_NUMBER,     // 
    INTERVALFIELD_LAST        //
}INTERVAL_FIELD;

// Define for interval unit.
typedef enum _DATA_INTERVALUNIT
{
    INTERVALUNIT_INVALID=0,
    INTERVALUNIT_UNDEFINE,    // Undefine.
    INTERVALUNIT_SECOND,      // Second.
    INTERVALUNIT_3SECOND,     // 3-Second.
    INTERVALUNIT_MINUTE,      // Minute.
    INTERVALUNIT_5MINUTE,     // 5-Minute.
    INTERVALUNIT_HOUR,        // Hour.
    INTERVALUNIT_5HOUR,       // 5-Hour.
    INTERVALUNIT_DAY,         // Day.
    //INTERVALUNIT_MONTH,     // Month.
    //INTERVALUNIT_YEAR,      // Year.
    INTERVALUNIT_LAST,
    INTERVALUNIT_ALL=256,     // All values with invalid.
    INTERVALUNIT_VALID        // All values without invalid.
}DATA_INTERVALUNIT;

// Define for interval value.
typedef enum _DATA_INTERVALVALUE
{
    INTERVALVALUE_INVALID=0,  //
    INTERVALVALUE_LAST=65535, // 
    INTERVALVALUE_ALL,        // All values with invalid.
    INTERVALVALUE_VALID       // All values without invalid.
}DATA_INTERVALVALUE;

// Define for position.
typedef enum _DATA_POSITION
{
    POSITIONTYPE_INVALID=0,
    POSITIONTYPE_FARM,         // Farm.
    POSITIONTYPE_MODEL,        // Model.
    POSITIONTYPE_STAGE,        // Stage.
    POSITIONTYPE_TURBINE,      // Turbine.
    POSITIONTYPE_AMT,          // Anemometer Tower.
    POSITIONTYPE_NWP,          // Numerical Weather Prediction.
    POSITIONTYPE_GPS,          // GPS device.
    POSITIONTYPE_LAST,
    POSITIONTYPE_ALL=256,      // All values with invalid.
    POSITIONTYPE_VALID         // All values without invalid.
}DATA_POSITION;

// Define for confidence.
typedef enum _DATA_CONFIDENCE
{
    CONFIDENCEVALUE_DENY=0x00,
    CONFIDENCEVALUE_DOUBT=0x3C,
    CONFIDENCEVALUE_CREDIBLE=0x5A,
    CONFIDENCEVALUE_INVALID=0xFF,
    CONFIDENCEVALUE_ALL,       // All values with invalid
    CONFIDENCEVALUE_VALID      // All values without invalid
}DATA_CONFIDENCE;

// Define for theory power computing method.
typedef enum _TP_CM
{
    TP_CM_UNKNOW=0,
    TP_CM_CWSM,    // Cabin Wind Speed Method.
    TP_CM_ATEM_S,  // Simple Anemometer Tower Extrapolation Method.
    TP_CM_ATEM,    // Anemometer Tower Extrapolation Method.
    TP_CM_MDEM,    // Meteorological Data Extrapolation Method.
    TP_CM_STM,     // Sample Turbine Method.
    TP_CM_WRM,     // Wind Resource Method.
    TP_CM_LAST
}TP_CM;

// Define log level.
typedef enum _LOG_LEVEL
{
    LOG_CLOSE=0,    // Log is closed.
    LOG_RUN,        // Error and key path.
    LOG_DETAIL,     // More information than Run.
    LOG_DEBUG       // All log.
}LOG_LEVEL;

// Define encoding for every sub-system.
typedef enum _SUB_SYSTEM
{
    SUBSYS_DC=0,    // Data Center.
    SUBSYS_SCADA,   // SCADA.
    SUBSYS_FDB,     // Forecast file into Database.
    SUBSYS_TP,      // Theory Power Calculate.
    SUBSYS_DPH,     // Dispatch process.
    SUBSYS_UTF,     // Ultra-short Term Forecast.
    SUBSYS_CLIENT,  // C/S Client.
	SUBSYS_LAST
}SUB_SYSTEM;

// ===============================================================================================
// ==  Data structure used for data process with data ID.
// ===============================================================================================
// The handle of data process.
typedef void * d_handle;

// The data structure for data pair exchange.
typedef struct _d_data
{
    char *id;   // Point to bytes buffer.
    char *val;  // Invalid when NULL.
    int vlen;   // The real size of val.
}d_data;

// The date time structure for common date transmit.
typedef struct _DATAFORMAT_DATE7B1
{
    unsigned short year;
    unsigned char  month;
    unsigned char  day;
    unsigned char  hour;
    unsigned char  minute;
    unsigned char  second;
}DATAFORMAT_DATE7B1;

// ===============================================================================================
// ==  Functions of data ID process.
// ===============================================================================================
// Get the size of data ID in bytes.
DI_API int did_idsize(void);

// Get single element length of data value.
DI_API int did_valuelength(char *pid);

// Get element amount of data value.
DI_API int did_valueamount(char *pid);

// Clear id field content.
DI_API char *did_fieldclear(char *pid);

// Make data ID with id field content.
DI_API int did_fieldget(char *pid, int fieldno);

// Get id field content of data ID.
DI_API char *did_fieldset(char *pid, int fieldno, int fieldvalue);

// Build single field of ID from its each parts.
DI_API char *did_fieldbuild(char *pid, int fieldno, int value[], int len);

// Get single part of id field.
DI_API int  did_fieldfetch(char *pid, int fieldno, int index);

// Get length from data type，but you must decide length when it return 0.
DI_API int did_typesize(int datatype);

// Build data ID for file.
DI_API char *did_file_makeid(char *pid, int catalog, int sn, int vlen=0);

// Get value length of the file ID.
DI_API int did_file_valuelength(int fnlen,int fdlen);

// Fill file data use filename.
DI_API char *did_file_valuefill(char *pval, const char *fname);

// Get value element position from value buffer.
DI_API void did_file_getvalue(char *pval, int vlen
                            , char **fname, int *pnlen
                            , char **fdata, int *pdlen);

// Build data ID for SQL.
DI_API char *did_sql_makeid(char *pid, int catalog, int vlen);

// Get SELECT statement size of SQL.
DI_API int did_sql_selectlength(int table_slen, int fields_slen
                              , int where_slen, int group_slen, int order_slen);

// Make data for SELECT statement for SQL.
DI_API char *did_sql_selectmake(char *pd
             , const char *table, const char *filds
             , const char *where, const char *group, const char *order);

// Get size of SELECT statement result of SQL.
DI_API int did_sql_resultlength(int result_slen);

// Make data for SELECT statement result for SQL.
DI_API char *did_sql_resultmake(char *pd, const char *result);

// Get INSERT INTO statement size of SQL.
DI_API int did_sql_insertlength(int table_slen, int fields_slen, int values_slen);

// Make data for INSERT INTO statment for SQL.
DI_API char *did_sql_insertmake(char *pd
            , const char *table, const char *fields, const char *values);

// Get UPDATE statement size of SQL.
DI_API int did_sql_updatelength(int table_slen, int set_slen, int where_slen);

// Make data for UPDATE statement for SQL.
DI_API char *did_sql_updatemake(char *pd
            , const char *table, const char *set, const char *where);

// ===============================================================================================
// ==  Functions of data process interface in C-language.
// ===============================================================================================
// Initiate the data process interface.
DI_API int d_init(const char *cfgpath=NULL);

// Open the data process interface for operation. Return DS_OK when success.
DI_API int d_open(d_handle &handle
                , const char *host=NULL, const char *sid=NULL
                , const char *user=NULL, const char *passwd=NULL);

// Close the data process interface after use. Return DS_OK when success.
DI_API int d_close(d_handle &h);

// Get data from package and return SQL statement for insert. Return database statement when success or "".
DI_API char *d_set(d_handle h, char *buf, int len);

// Get data from package and return SQL statement for query. Return database statement when success or "".
DI_API char *d_get(d_handle h, char *pd, int dlen
    , int *page_pno= NULL
    , int *page_plen= NULL
    , DATAFORMAT_DATE7B1 *dbegin_pdate=NULL
    , DATAFORMAT_DATE7B1 *dend_pdate  =NULL
    , unsigned int *pinterval=NULL                      //32bit    //
    , DATAFORMAT_DATE7B1 *dforecast_pdate=NULL
	);

// Begin transmit data to the interface.  //Return DS_OK when success.
DI_API int d_begintransmit(d_handle h, int no, int col_num,int row_num);

// Transmit data to the interface.  //Return DS_OK when success.
DI_API int d_transmitdata(d_handle h, int no, const char *table
        , const char *field, int col, int row, int type, char *buf, int len);

// End data transmit to the interface.  // Return DS_OK when success.
DI_API int d_endtransmit(d_handle h, char **ppd, int *pl);

// Get description of error within the interface.
DI_API const char *d_error(int err);

// Get error no within the interface.
DI_API int d_errno(d_handle h=0);

// Set id field map of data ID. This function must only be used when no other function used.
DI_API void d_fieldmap(unsigned short map[IDFIELD_ALL]);

// Set log level.
DI_API int d_log(int level=LOG_RUN);

// ===============================================================================================
// ==  Functions of character set transform.
// ===============================================================================================
// Translate file from GB2312 to UTF-8.  // 0: No process, -1: crlf->lf, 1: lf->crlf
DI_API int d_fgbkutf8(const char *fgbk, const char *futf8, int crlf=0, bool bom=true);

// Translate file from UTF-8 to GB2312.  // 0: No process, -1: crlf->lf, 1: lf->crlf
DI_API int d_futf8gbk(const char *futf8, const char *fgbk, int crlf=0);

// ===============================================================================================
// ==  Class for data process interface in C++-language.
// ===============================================================================================
// Class for data ID.
class DI_API DID
{
public:    // Constructors.
    DID();
    DID(char *pid);
    DID(const DID &o);
    ~DID();
    DID & operator =(DID &o);
public:
    // Import data ID in address 'pid'.
    void Import(char *pid);
    // Export data ID to address 'pid'.
    void Export(char *pid);
    // Import data ID from string.
    int ImportString(const char *sz);
    // Export data ID to string, the buffer 'sz' size must >= (2*DID::IdSize()+1).
    int ExportString(char *buf, int len=0);
    // Clear ID buffer.
    void Clear(void);
    // Valid or not.
    bool IsValid(void);         //
    // Equal or not.  IDFIELD_CATALOG/IDFIELD_KIND/IDFIELD_ENCODING/IDFIELD_HEIGHT/IDFIELD_INTERVAL/IDFIELD_SOURCE
    bool IsEqual(char *pid, int exceptfld=IDFIELD_INVALID);//
    // Filter to judge whether it is a wanted ID.
    bool Filter(int catalog
              , int kind          = DATAKIND_ALL
              , int encodetype    = ENCODETYPE_ALL
              , int encodesn      = ENCODESN_ALL
              , int sourcetype    = SOURCETYPE_ALL
              , int sourcesn      = SOURCESN_ALL
              , int intervalunit  = INTERVALUNIT_ALL
              , int intervalvalue = INTERVALVALUE_ALL
              , int heighttype    = HEIGHTTYPE_ALL
              , int heightvalue   = HEIGHTVALUE_ALL);
    // Get id field by field no.
    int  FieldGet(int fieldno);
    // Get Maximum value of id field by field no.
    int  FieldMax(int fieldno);
    // Set id field value by field no.
    void FieldSet(int fieldno,int fieldvalue);
    // Get pointer of id bytes.
    char *IdData(void);
    // Get size of id.
    int IdSize(void);
    // Get size by data type.
    static int TypeSize(int type); // You must decide length when it return <0 or =0.
    // Get value type.
    int ValueType(void);
    // Get value length.
    int ValueLength(void);
    // Get value length.
    int ValueAmount(void);
    // Get maximum amount.
    int MaxAmount(void);

public:
    // Make each normal data ID by every field.
    char *MakeNormal(int dcatalog    // This parameter is defined in 'DATA_CATALOG'.
                   , int dkind       // This parameter is defined in 'DATA_KIND'.
                   , int dtype       // This parameter is defined in 'DATA_TYPE'.
                   , int dlen        // This parameter is a real value when string or calculated by DID::TypeSize(dtype).
                   , int amount=0    // This parameter is a real value of amount or zero when request.
                   , int ectype=ENCODETYPE_INVALID   // This parameter is defined in 'DATA_ENCODETYPE'.
                   , int ecsn=ENCODESN_INVALID       // This parameter is a value really or defined in 'DATA_ENCODESN', Use from 0.
                   , int srctype=SOURCETYPE_INVALID  // This parameter is defined in 'DATA_SOURCETYPE'.
                   , int srcsn=SOURCESN_INVALID      // This parameter is a value really or defined in 'DATA_SOURCESN', Use from 0.
                   , int itype=INTERVALUNIT_INVALID  // This parameter is defined in 'DATA_INTERVALUNIT'.
                   , int ivalue=INTERVALVALUE_INVALID// This parameter is a value really or defined in 'DATA_INTERVALVALUE'.
                   , int htype=HEIGHTTYPE_INVALID    // This parameter is defined in 'DATA_HEIGHTTYPE'.
                   , float hvalue=HEIGHTVALUE_INVALID// Height value, unit: m.
                   , int confidence=CONFIDENCEVALUE_INVALID  // This parameter is a value really or defined in 'DATA_CONFIDENCE'.
                    );  // Fully realized for normal ID make.
    // Make each computed data ID by every field.
    char *MakeCompute(int dcatalog    // Data Catalog.
                    , int dkind       // This parameter is defined in 'DATA_KIND'.
                    , int dtype       // This parameter is defined in 'DATA_TYPE'.
                    , int dlen        // This parameter is a real value when string or calculated by DID::TypeSize(dtype).
                    , int amount=0    // This parameter is a real value of amount or zero when request.
                    , int ectype=ENCODETYPE_INVALID       // This parameter is defined in 'DATA_ENCODETYPE'.
                    , int ecmethod=ENCODEMETHOD_ORIGINAL  // This parameter is a value really or defined in 'DATA_ENCODESN'.
                    , int ecsn=ENCODESN_INVALID           // This parameter is a value really or defined in 'DATA_ENCODESN', Use from 0.
                    , int ecdno0=0                        // Mask of computing for encoding 0.
                    , int ecdno1=0                        // Mask of computing for encoding 1.
                    , int ecdno2=0                        // Mask of computing for encoding 2.
                    , int ecdno3=0                        // Mask of computing for encoding 3.
                    , int ecdno4=0                        // Mask of computing for encoding 4.
                    , int ecdno5=0                        // Mask of computing for encoding 5.
                    , int ecdno6=0                        // Mask of computing for encoding 6.
                    , int srctype=SOURCETYPE_INVALID      // Source Type.
                    , int itype=INTERVALUNIT_INVALID      // This parameter is defined in 'DATA_INTERVALUNIT'.
                    , int ivalue=INTERVALVALUE_INVALID    // This parameter is a value really or defined in 'DATA_INTERVALVALUE'.
                    , int htype=HEIGHTVALUE_INVALID       // This parameter is defined in 'DATA_HEIGHTTYPE'.
                    , float hvalue=HEIGHTVALUE_INVALID        // Height value, unit: m.
                    , int confidence=CONFIDENCEVALUE_INVALID  // This parameter is a value really or defined in 'DATA_CONFIDENCE'.
                     );  // Fully realized for computed ID make.
    // Make date ID.                           // 1st parameter is defined as DD/SD/PD/FD.
    char *MakeDate(int dcatalog=DATACATALOG_DD, int dkind=DATAKIND_INVALID, int amount=0);
    // Make encoding ID.                       // 1st parameter is defined in "DATA_ENCODETYPE".
    char *MakeEncoding(int ectype, int len=0, int amount=0);
    // Make height ID.                         // 1st parameter is defined in "DATA_HEIGHT" and 2nd in "DATA_TYPE".
    char *MakeHeight(int htype, float hvalue, int dtype=DATATYPE_SINT32, int len=4, int amount=0);
    // Make product model ID.                  // 1st parameter is defined in "DATA_ENCODETYPE".
    char *MakeProductModel(int ectype, int ecsn=0, int len=0, int amount=0);
    // Make data ID by each property of data.  //
    char *MakeProperty(int dcatalog, int dkind, int dtype, int dlen, int amount, int ectype, int ecsn);

    // Caution:
    //   The follow functions in this section is obsolete, Replace with MakeNormal() and MakeCompute().
    // Make meteor data ID for single device.
    char *MakeMeteorSingle(int dcatalog, int dkind, int dtype, int dlen, int amount
                         , int ectype, int ecsn, int srctype, int srcsn
                         , int htype, float hvalue, int itype, int ivalue);
    // Make normal measured data ID for single device.
    char *MakePeriodSingle(int dcatalog, int dkind, int dtype, int dlen, int amount
                         , int ectype, int ecsn
                         , int srctype, int srcsn
                         , int itype, int ivalue);
    // Make meteor data ID for whole farm.  // DATACATALOG_WS/WD/AT/AH/AD/AP; ENCODETYPE_FARM/MODULE/STAGE
    char *MakeMeteorGlobal(int dcatalog, int dkind, int dtype, int dlen, int amount
                         , int ectype, int ecmethod, int ecsn
                         , int ecdno0, int ecdno1, int ecdno2, int ecdno3, int ecdno4, int ecdno5, int ecdno6
                         , int srctype, int htype, float hvalue
                         , int itype, int ivalue);
    // Make normal measured data ID for single device.
    char *MakePeriodGlobal(int dcatalog, int dkind, int dtype, int dlen,int amount
                         , int ectype, int ecmethod, int ecsn
                         , int srctype, int itype, int ivalue);

public:    // Data buffer functions. This section is obsolete and replace with class DBuffer.
    // Reset the data buffer.
    char *BufferReset(void);
    // Get data length in data buffer.
    int BufferLength(void);
    // Get max size of  data buffer.
    int BufferSize(void);
    // Put ID and value into data buffer.
    char *BufferBuild(char *pid, char *pvalue=NULL, int vlen=-1);
    // Put data into data buffer.
    char *BufferBuild(char *pdata, int len);
    // Get the address of data buffer.
    char *BufferPointer(void);
    // Fetch data buffer to d_data array.
    void BufferFetch(char *pbuf, int len, d_data **ppd, int &num, bool bv=true);
    // Diagnose buffer pbuf and return message in sbuf.
    int  DiagnoseBuffer(char *pbuf, int len, bool bv, char *sbuf, int slen);

public:  // Build data to data buffer. This section is obsolete and replace with class DBuffer.
    // Build SELECT statement of SQL to data buffer.
    char *BufferBuildSqlSelect(const char *table, const char *filds
                             , const char *where, const char *group, const char *order);
    // Build result of SELECT statement to data buffer.
    char *BufferBuildSqlResult(char *pd, const char *result);
    // Build INSERT INTO statement of SQL to data buffer.
    char *BufferBuildSqlInsert(const char *table, const char *fields, const char *values);
    // Build UPDATE statement of SQL to data buffer.
    char *BufferBuildSqlUpdate(const char *table, const char *set, const char *where);
    // Build file data to data buffer.
    char *BufferBuildFile(int type, int sn, const char *filename, char *filedata, int filelen);

private:
    char id[64];     // Bytes buffer for ID.
    char *buffer;    // Data buffer for communication.
    //int  offset;   // Data offset in data buffer.
};

// Class for data Buffer.
class DI_API DBuffer
{
public:    // Constructors.
    DBuffer();
    DBuffer(const DBuffer &o);
    ~DBuffer();
    DBuffer & operator =(DBuffer &o);
public:    // Data buffer functions.
    // Reset the data buffer.
    char *Reset(void);
    // Get data length in data buffer.
    int Length(void);
    // Get max size of  data buffer.
    int Size(void);
    // Put ID and value into data buffer.
    char *Build(char *pid, char *pvalue=NULL, int vlen=-1);
    // Put data into data buffer.
    char *Build(char *pdata, int len);
    // Get the address of data buffer.
    char *Pointer(void);
    // Fetch data buffer to d_data array.
    void Fetch(char *pbuf, int len, d_data **ppd, int &num, bool bv=true);
    // Diagnose buffer pbuf and return message in sbuf.
    int  Diagnose(char *pbuf, int len, bool bv, char *sbuf, int slen);

public:    // Build data to data buffer.
    // Build SELECT statement of SQL to data buffer.
    char *BuildSqlSelect(const char *table, const char *filds
                             , const char *where, const char *group, const char *order);
    // Build result of SELECT statement to data buffer.
    char *BuildSqlResult(char *pd, const char *result);
    // Build INSERT INTO statement of SQL to data buffer.
    char *BuildSqlInsert(const char *table, const char *fields, const char *values);
    // Build UPDATE statement of SQL to data buffer.
    char *BuildSqlUpdate(const char *table, const char *set, const char *where);
    // Build file data to data buffer.
    char *BuildFile(int type, int sn, const char *filename, char *filedata, int filelen);

private:
    char *buffer;    // Data buffer for communication.
};

// Class for data value.
class DValue
{
public:
    DValue();
    DValue(const DValue &o);
    DValue & operator = (const DValue &o);
    virtual ~DValue();
    // Import data for value.
    DValue(int type, char *pbuf, int len=0);
    void Import(int type, char *pbuf, int len=0);
public:
    // Get string of value.
    const char *ZeroString(void);
    // Get size by data type.
    static int TypeSize(int type);
    // Convert value to type 't' and return new length.
    int Convert(int t, int len=0);
public:
    // Set Data Date to now;
    static void SetDateNow(DATAFORMAT_DATE7B1 *pd);
    // Set Data Date to empty when calling d_get();
    static void SetDateEmpty(DATAFORMAT_DATE7B1 *pd);
    // Set Data Date to maximum value when calling d_get();
    static void SetDateMax(DATAFORMAT_DATE7B1 *pdbegin, DATAFORMAT_DATE7B1 *pdend=NULL, unsigned int *pinterval=0);
    // Set Data Date to minimum value when calling d_get();
    static void SetDateMin(DATAFORMAT_DATE7B1 *pdbegin, DATAFORMAT_DATE7B1 *pdend=NULL, unsigned int *pinterval=0);

private:
    char *value;
};

// ===============================================================================================
// ==  System functions for data process interface.
// ===============================================================================================
// Display switch define.
#define LOG_WITH_ALL              (~((unsigned long long)0))
#define DEBUG_DISPLAY_ALL         LOG_WITH_ALL
#define LOG_WITH_RUN              ((unsigned long long)0x0000000000000001)
#define LOG_WITH_FUNCLIMITED      ((unsigned long long)0x0000000000000002)
#define LOG_WITH_SYSTEMERROR      ((unsigned long long)0x0000000000000004)
#define LOG_WITH_ALERT            ((unsigned long long)0x0000000000000008)
#define LOG_WITH_DATALESS         ((unsigned long long)0x0000000000000100)
#define LOG_WITH_DATAERROR        ((unsigned long long)0x0000000000000200)
#define LOG_WITH_ENTERCHECK       ((unsigned long long)0x0000000000000400)
#define LOG_WITH_PROCESS          ((unsigned long long)0x0000000000010000)
#define LOG_WITH_DATA             ((unsigned long long)0x0000000000020000)
#define LOG_WITH_PROMPT           ((unsigned long long)0x0000000000040000)
#define LOG_WITH_CHARSET          ((unsigned long long)0x0000000000080000)

// Set display information.
DI_API void d_debug(unsigned long long sw);  //
// Get library version.
DI_API char *d_version(void);                //

// ===============================================================================================
// ==  Debug functions for data process interface.
// ===============================================================================================
DI_API void d_selfcheck(void);       // Self-check.
DI_API void did_debug(char *pid);    // Debug the id.
DI_API char *d_verdate(void);        // Simple version.
// ===============================================================================================
// ==  Error no define for Data Interface function.
// ===============================================================================================
typedef enum _D_STATE
{
    DS_OK=0,                           //
    //
    DS_WRN_FIRST           =-1,
    DS_WRN_INVALIDHANDLE   =-2,          // The DataInterface handle is invalid.
    DS_WRN_INVALIDFORMAT   =-3,          // The Data Format error.
    DS_WRN_NULLPOINTER     =-4,          // The point is NULL.
    DS_WRN_LITTLEBUFFER    =-5,          // The buffer is not enough.
    DS_WRN_INVALIDMEMBER   =-6,          // The member is invalid.
    DS_WRN_LACKFUNC        =-7,          // The function is lacked.
    DS_WRN_NOMATCH         =-8,          // NO match.
    DS_WRN_LACKTABLE       =-9,          // The table is lacked.
    DS_WRN_LACKELEMENT     =-10,         // The data element is lacked.
    DS_WRN_LACKREQUESTID   =-11,         // The ID for request is lacked.
    DS_WRN_LACKQUERYRESULT =-12,         // The Result of query is lacked.
    DS_WRN_LAST          =-10000,
    //
    DS_ERR_BASE          =-10001,
    DS_ERR_DIHANDLE      =-10002,      // The data interface handle is invalid.
    DS_ERR_MEMORYNEW     =-10003,      // Error when 'new' something.
    DS_ERR_MBC2WC        =-10004,      // Error when translate from multibytes to wide char.
    DS_ERR_WC2MBC        =-10005,      // Error when translate from wide char to multibytes.
    DS_ERR_ICONVOPEN     =-10006,      // Error when open iconv.
    DS_ERR_ICONV         =-10007,      // Error when iconv.
    DS_ERR_OPENFILE      =-10008,      // Error when open file.
    DS_ERR_READFILE      =-10009,      // Error when read file.
    DS_ERR_WRITEFILE     =-10010,      // Error when write file.
    DS_ERR_PARAMETER     =-10011,      // The parameter is invalid.
    DS_ERR_IDDLENGTH     =-10012,      // ID data type length error.
    DS_ERR_IDVAMOUNT     =-10013,      // ID data value amount error.
    DS_ERR_BUFFERFORMAT  =-10014,      // ID or pairs buffer format error.
    DS_ERR_IDCATALOG     =-10015,      // Data Catalog in ID in unknown.
    DS_ERR_LAST          =-20000,
    //
    DS_USR_FIRST         =-20001,
    //
    DS_USR_LAST          =-30000,
    //
    DS_LAST
}D_STATE;

// ===============================================================================================
// ==  Limit value define.
// ===============================================================================================
//
#define MAX_SQL_LEN              3600  // The max length of SQL statement.

// ===============================================================================================
// ==  Invalid value define.
// ===============================================================================================
//
#define FLOATVALUE_INVALID          -9999999.000
#define CHARVALUE_INVALID           "NAN"
#define CHARTEXT_INVALID            "NULL"
#define INTVALUE_INVALID            -9999999
#define DATEVALUE_INVALID_YEAR      1900
#define DATEVALUE_INVALID_MONTH     1
#define DATEVALUE_INVALID_DAY       1
#define DATEVALUE_INVALID_HOUR      0
#define DATEVALUE_INVALID_MINUTE    0
#define DATEVALUE_INVALID_SECOND    0

/*************************************************************************************************/
#pragma pack(pop)
/*************************************************************************************************/

/*************************************************************************************************/
#endif /* DATA_H_ */
/*************************************************************************************************/
