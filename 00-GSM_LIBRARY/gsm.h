/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website https://majerle.eu/projects/gsm-at-commands-parser-for-embedded-systems
 * \license MIT
 * \version 0.4.0
 * \brief   GSM Library
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GSM_H
#define GSM_H 040

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup GSM
 * \brief    Platform independent, written in ANSII C, GSM AT parser library for SIMcom modules   
 * \{
 */
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"

/* Proto threads */
#include "pt/pt.h"
/* GSM configuration */
#include "gsm_config.h"
/* Buffer implementation */
#include "buffer.h"

/* GSM Low-Level drivers */
#include "gsm_ll.h"

#if GSM_RTOS
/* RTOS support */
#include "gsm_sys.h"
#endif

/**
 * @defgroup GSM_Macros
 * @brief    Library defines
 * @{
 */

#define gvol          volatile
#define gstatic       static

/* Configuration defines */

/**
 * \}
 */
 
/**
 * \defgroup GSM_Typedefs
 * \brief    Library Typedefs
 * \{
 */

/**
 * \brief  GSM library result enumeration
 */
typedef enum _GSM_Result_t {
    gsmOK = 0x00,                                           /*!< OK status */
    gsmERROR,                                               /*!< Error status */
    gsmBUSY,                                                /*!< GSM is busy right now */
    gsmPARERROR,                                            /*!< Parameter errors */
    gsmTIMEOUT,                                             /*!< Timeout occurred */
    gsmSENDFAIL,                                            /*!< Send data failed on connection */
    
    gsmSYSERROR,                                            /*!< Syscall error */
    gsmLLERROR,                                             /*!< Error in Low-Level processing */
    gsmSIMNOTREADYERROR,                                    /*!< SIM is not ready to operate */
    gsmENTERTEXTMODEERROR,                                  /*!< Error trying to enter text mode for SMS */
    gsmSIMMEMORYERROR,                                      /*!< Error trying to select SMS mode */
    gsmNETWORKERROR
} GSM_Result_t;

/**
 * \brief  Call direction enumeration
 */
typedef enum _GSM_CallDir_t {
    GSM_CallDir_MO = 0x00,                                  /*!< Call direction is Mobile Originated */
    GSM_CallDir_MT = 0x01                                   /*<! Call direction is Mobile Terminated */
} GSM_CallDir_t;

/**
 * \brief  Call type enumeration
 */
typedef enum _GSM_CallType_t {
    GSM_CallType_Voice = 0x00,                              /*!< Call type is voice */
    GSM_CallType_Data = 0x01,                               /*!< Call type is data */
    GSM_CallType_Fax = 0x02                                 /*!< Call type is fax */
} GSM_CallType_t;

/**
 * \brief  Call status enumeration
 */
typedef enum _GSM_CallState_t {
    GSM_CallState_Active = 0x00,                            /*!< Call is active */
    GSM_CallState_Held = 0x01,                              /*<! Call is held */
    GSM_CallState_Dialing = 0x02,                           /*<! Call is dialing */
    GSM_CallState_Alerting = 0x03,                          /*<! Call is alerting */
    GSM_CallState_Incoming = 0x04,                          /*<! Call is incoming */
    GSM_CallState_Waiting = 0x05,                           /*<! Call is waiting */
    GSM_CallState_Disconnect = 0x06                         /*<! Call is disconnected */
} GSM_CallState_t;

/**
 * \brief  SMS memory enumeration
 * \note   This is currently not used and is for future purpose
 */
typedef enum _GSM_SMS_Memory_t {
    GSM_SMS_Memory_SM,
    GSM_SMS_Memory_BM,
    GSM_SMS_Memory_SE,
    GSM_SMS_Memory_ME
} GSM_SMS_Memory_t;

/**
 * \brief  SMS mass delete enumeration
 */
typedef enum _GSM_SMS_MassDelete_t {
    GSM_SMS_MassDelete_Read = 0x00,                         /*!< Delete all read SMS messages */
    GSM_SMS_MassDelete_Unread,                              /*!< Delete all unread SMS messages */
    GSM_SMS_MassDelete_Sent,                                /*!< Delete all sent SMS messages */
    GSM_SMS_MassDelete_Unsent,                              /*!< Delete all unset SMS messages */
    GSM_SMS_MassDelete_Inbox,                               /*!< Delete all received SMS messages */
    GSM_SMS_MassDelete_All                                  /*!< Delete all SMS messages */
} GSM_SMS_MassDelete_t;

/**
 * \brief  SMS read type enumeration
 */
typedef enum _GSM_SMS_ReadType_t {
    GSM_SMS_ReadType_ALL,                                   /*!< List all SMS entries */
    GSM_SMS_ReadType_READ,                                  /*!< List read entries only */
    GSM_SMS_ReadType_UNREAD                                 /*!< List unread entries only */
} GSM_SMS_ReadType_t;

/**
 * \brief  SIM card informations
 */
typedef enum _GSM_CPIN_t {
    GSM_CPIN_Unknown = 0x00,                                /*!< Default value */
    GSM_CPIN_Ready,                                         /*!< SIM is ready to use */
    GSM_CPIN_SIM_PIN,                                       /*!< SIM is waiting for PIN number */
    GSM_CPIN_SIM_PUK,                                       /*!< SIM is blocked and is waiting for PUK number */
    GSM_CPIN_PH_SIM_PIN,
    GSM_CPIN_PH_SIM_PUK,
    GSM_CPIN_SIM_PIN2,
    GSM_CPIN_SIM_PUK2
} GSM_CPIN_t;

/**
 * \brief  Time structure
 */
typedef struct _GSM_Date_t {
    uint8_t Day;                                            /*!< Day in month, from 1 to 31 */
    uint8_t Month;                                          /*!< Month in a year, from 1 to 12 */
    uint16_t Year;                                          /*!< Year with included thousands */
} GSM_Date_t;

/**
 * \brief  Time structure
 */
typedef struct _GSM_Time_t {
    uint8_t Hours;                                          /*!< Hours in format 00-24 */
    uint8_t Minutes;                                        /*!< Minutes */
    uint8_t Seconds;                                        /*!< Seconds */
} GSM_Time_t;

/**
 * \brief  Date and time structure
 */
typedef struct _GSM_DateTime_t {
    GSM_Date_t Date;                                        /*!< Date data */
    GSM_Time_t Time;                                        /*!< Time data */
} GSM_DateTime_t;

/**
 * \brief  SMS structure for send message
 */
typedef struct _GSM_SMS_t {
    const char* Number;                                     /*!< Pointer to constant string for number */
    const char* Data;                                       /*!< Pointer to constant data array */
    uint16_t SentSMSMemNum;                                 /*!< Memory number for sent SMS */
} GSM_SMS_t;

/**
 * \brief  Call information structure
 * \note   Used when call status changes to notify user
 */
typedef struct _GSM_CallInfo_t {
    uint8_t ID;                                             /*!< Call identification number, 0 - 7 */
    GSM_CallDir_t Dir;                                      /*!< Call direction */
    GSM_CallState_t State;                                  /*!< Call state */
    GSM_CallType_t Type;                                    /*!< Call type */
    char Number[16];                                        /*!< Call phone number */
    uint8_t IsMultiparty;                                   /*!< Call multiparty status */
    uint8_t AddressType;                                    /*!< Call address type */
    char Name[20];                                          /*!< Name in phonebook, if exists for selected number */
} GSM_CallInfo_t;

/**
 * \brief  SMS information structure
 * \note   Used when new SMS is received to notify user
 */
typedef struct _GSM_SmsInfo_t {
    GSM_SMS_Memory_t Memory;                                /*!< Memory where received SMS was saved */
    uint16_t Position;                                      /*!< Position where SMS was stored in memory */
    
    union {
        struct {
            uint8_t Received:1;                             /*!< Do we have new SMS info on this structure and was not yet checked */
            uint8_t Used:1;                                 /*!< Memory used indication */
        } F;
        uint8_t Value;
    } Flags;                                                /*!< Structure flags management */
} GSM_SmsInfo_t;

/**
 * \brief  SMS item
 */
typedef struct _GSM_SMS_Entry_t {
    uint16_t Position;                                      /*!< Position number in memory */
    GSM_SMS_Memory_t Memory;                                /*!< Memory type used for SMS */
    char Data[GSM_SMS_MAX_LENGTH];                          /*!< Actual SMS data in text mode */
    uint8_t DataLen;                                        /*!< Length of SMS data */
    char Name[20];                                          /*!< Name of sender if exists in phonebook */
    char Number[20];                                        /*!< Sender phone number */
    GSM_DateTime_t DateTime;                                /*!< SMS date and time sent/received */
} GSM_SMS_Entry_t;

/**
 * \brief  Phonebook entry item
 */
typedef struct _GSM_PB_Entry_t {
    uint16_t Index;                                         /*!< Phonebook index number */
    char Name[20];                                          /*!< Phonebook saved display name */
    char Number[20];                                        /*!< Actual phone number */
} GSM_PB_Entry_t;

/**
 * \brief  Connection type enumeration
 */
typedef enum _GSM_CONN_Type_t {
    GSM_CONN_Type_TCP,                                      /*!< Create TCP connection  */
    GSM_CONN_Type_UDP                                       /*!< Create UDP connection */
} GSM_CONN_Type_t;

/**
 * \brief  Connection over SSL selection
 */
typedef enum _GSM_CONN_SSL_t {
    GSM_CONN_SSL_Disable = 0x00,                            /*!< Disable TCP over SSL */
    GSM_CONN_SSL_Enable = 0x01                              /*!< Enable TCP over SSL */
} GSM_CONN_SSL_t;

/**
 * \brief  Connection structure for GSM
 */
typedef struct _GSM_CONN_t {
    uint8_t ID;                                             /*!< Connection identification */
    uint8_t* ReceiveData;                                   /*!< Pointer to data array for receive or send */
    uint16_t BytesToRead;                                   /*!< Bytes to read from receive at a time, selected by user on function call */
    uint16_t BytesRead;                                     /*!< Bytes actually read from device, number of bytes we actually read in packet */
    uint32_t BytesReadTotal;                                /*!< Total number of bytes we already read from response */
    uint16_t BytesReadRemaining;                            /*!< Number of bytes we have to read in current packet */
    uint16_t BytesRemaining;                                /*!< Number of bytes remaining to read in module buffer */
    uint16_t ReadTimeout;                                   /*!< Timeout before checking for new data when reading */
    union {
        struct {
            uint8_t Active:1;                               /*!< Connection active flag */
            uint8_t RxGetReceived:1;                        /*!< RXGET was received waiting to read data */
            uint8_t CallGetReceived:1;                      /*!< RXGET was received, notify user about new data */
            uint8_t CallConnClosed:1;                       /*!< Connection was closed by remote server */
        } F;
        uint8_t Value;                                      /*!< Value containing all the flags in single memory */
    } Flags;                                                /*!< Union with all the listed flags */
} GSM_CONN_t;

/**
 * \brief  HTTP suppoerted request methods
 */
typedef enum _GSM_HTTP_Method_t {
    GSM_HTTP_Method_GET = 0x00,                             /*!< HTTP method GET */
    GSM_HTTP_Method_POST = 0x01,                            /*!< HTTP method POST */
    GSM_HTTP_Method_HEAD = 0x02                             /*!< HTTP method HEAD */
} GSM_HTTP_Method_t;

/**
 * \brief  HTTP structure for GSM
 */
typedef struct _GSM_HTTP_t {
    GSM_HTTP_Method_t Method;                               /*!< HTTP method */
    const char* TMP;                                        /*!< Pointer to URL to use/CONTENT to send */
    uint16_t Code;                                          /*!< HTTP response code */
    uint8_t* Data;                                          /*!< Pointer to read/write data array */
    uint32_t DataLength;                                    /*!< Length of data array in units of bytes */
    uint32_t BytesReceived;                                 /*!< Number of total bytes received by HTTP response from server to GSM */
    uint32_t BytesReadTotal;                                /*!< Total number of bytes we already read from GSM module HTTP response */
    uint16_t BytesToRead;                                   /*!< Bytes to read from receive at a time, selected by user on function call */
    uint32_t BytesRead;                                     /*!< Actual number of bytes read in last read procedure */
    uint32_t BytesReadRemaining;                            /*!< Number of bytes remaining to read in current read procedure */
} GSM_HTTP_t;

/**
 * \brief  HTTP over SSL selection
 */
typedef enum _GSM_HTTP_SSL_t {
    GSM_HTTP_SSL_Disable = 0x00,                            /*!< Disable SSL usage for HTTP */
    GSM_HTTP_SSL_Enable = 0x01                              /*!< Enable SSL usage for HTTP */
} GSM_HTTP_SSL_t;

/**
 * \brief  FTP structure for GSM
 */
typedef struct _GSM_FTP_t {
    uint8_t Mode;                                           /*!< FTP usage mode (execute, read) */
    uint8_t ErrorCode;                                      /*!< Error code number */
    uint8_t* Data;                                          /*!< Pointer to read/write data array */
    uint32_t BytesToProcess;                                /*!< Bytes to process (read/write) at a time, selected by user on function call */
    uint32_t BytesRead;                                     /*!< Bytes actually read from device, number of bytes we actually read in packet */
    uint32_t BytesReadRemaining;                            /*!< Number of bytes remaining to read in current read procedure */
    uint32_t BytesProcessedTotal;                           /*!< Total number of bytes read in FTP session */
    uint32_t MaxBytesToPut;                                 /*!< Maximal number of bytes we can put on FTPPUT command */
    union {
        struct {
            uint8_t DataAvailable:1;                        /*!< Set to 1 when data available to read */
            uint8_t DownloadActive:1;                       /*!< Set to 1 when download session is active */
        } F;
        uint8_t Value;
    } Flags;                                                /*!< FTP Flags */
} GSM_FTP_t;

/**
 * \brief  FTP connection mode
 */
typedef enum _GSM_FTP_Mode_t {
    GSM_FTP_Mode_Active = 0x00,                             /*!< Active FTP mode */
    GSM_FTP_Mode_Passive = 0x01                             /*!< Passive FTP mode */
} GSM_FTP_Mode_t;

/**
 * \brief  FTP over SSL selection
 */
typedef enum _GSM_FTP_SSL_t {
    GSM_FTP_SSL_Disable = 0x00,                             /*!< Do not use FTP over SSL */
    GSM_FTP_SSL_Implicit = 0x01,                            /*!< Use FTP over SSL in implicit mode */
    GSM_FTP_SSL_Explicit = 0x02                             /*!< Use FTP over SSL in explicit mode */
} GSM_FTP_SSL_t;

/**
 * \brief  FTP upload mode
 */
typedef enum _GSM_FTP_UploadMode_t {
    GSM_FTP_UploadMode_Append = 0x00,                       /*!< Append file already existing on server */
    GSM_FTP_UploadMode_StoreUnique = 0x01,
    GSM_FTP_UploadMode_Store = 0x02
} GSM_FTP_UploadMode_t;

/**
 * \brief  GSM network status
 */
typedef enum _GSM_NetworkStatus_t {
    GSM_NetworkStatus_Searching = 0x00,                     /*!< Searching for network */
    GSM_NetworkStatus_RegisteredHome = 0x01,                /*!< Registered in home network */
    GSM_NetworkStatus_NotRegistered,                        /*!< Not registered and not searching */
    GSM_NetworkStatus_RegistrationDenied,                   /*!< Registration has been denied */
    GSM_NetworkStatus_Unknown,                              /*!< Unknown status */
    GSM_NetworkStatus_RegisteredRoaming                     /*!< Registered and roaming */
} GSM_NetworkStatus_t;

/**
 * \brief  GSM GPS location over GPRS
 */
typedef struct _GSM_GPS_t {
    uint16_t Error;                                         /*!< Error number if exists */
    float Latitude;                                         /*!< GPS latitude location */
    float Longitude;                                        /*!< GPS longitude location */
    GSM_Date_t Date;                                        /*!< Date of GPS, UTC */
    GSM_Time_t Time;                                        /*!< Time of GPS, UTC */
} GSM_GPS_t;

/**
 * \brief  Battery informations 
 */
typedef struct _GSM_Battery_t {
    uint8_t Charging;                                       /*!< Status indicating battery is charging */
    uint8_t Percentage;                                     /*!< Battery percentage */
    uint16_t Voltage;                                       /*!< Battery voltage in units of millivolts */
} GSM_Battery_t;

/**
 * \brief  Event enumeration for callback
 */
typedef enum {
    gsmEventIdle = 0x00,                                    /*!< Stack went idle */
    gsmEventDataReceived,                                   /*!< A new data received on connection */
    gsmEventDataSent,                                       /*!< Data were sent on connection */
    gsmEventDataSentError,                                  /*!< Data sent error */
    gsmEventCallCLCC,                                       /*!< CLCC Call info was received with call data */
    gsmEventCallRING,                                       /*!< RING was received on call */
    gsmEventSMSCMTI,                                        /*!< SMS info was received */
    gsmEventGPRSAttached,                                   /*!< GPRS has been attached */
    gsmEventGPRSAttachError,                                /*!< Error while trying to attach GPRS */
    gsmEventGPRSDetached,                                   /*!< GPRS has been detached */
    gsmEventUVWarning,                                      /*!< Under voltage warning received */
    gsmEventUVPowerDown,                                    /*!< Under voltage power down */
} GSM_Event_t;

/**
 * \brief  Parameters for callback processing
 */
typedef struct _GSM_EventParams_t {
    const void* CP1;                                        /*!< Constant void pointer number 1 */
    const void* CP2;                                        /*!< Constant void pointer number 2 */
    uint32_t UI;                                            /*!< Unsigned integer value */
} GSM_EventParams_t;

/**
 * \brief  Callback function prototype
 */
typedef int (*GSM_EventCallback_t)(GSM_Event_t, GSM_EventParams_t*);

/**
 * \brief  GSM structure
 */
typedef struct _GSM_t {
    gvol uint32_t Time;                                     /*!< Current time in units of milliseconds */
    gvol GSM_Result_t RetVal;                               /*!< Return value */
    
    /*! Low-Level management */
    GSM_LL_t LL;                                            /*!< Structure for Low-Level communication */
    
    /*!< Active command informations */
    gvol uint16_t ActiveCmd;                                /*!< Current active command for execution */
    gvol uint16_t ActiveCmdSaved;                           /*!< Value of saved active CMD when necessary to change active command while processing one. */
    const char* gvol ActiveCmdResp;                         /*!< Pointer to active command response we are waiting for */
    gvol uint32_t ActiveCmdStart;                           /*!< Time when new command started with execution */
    gvol GSM_Result_t ActiveResult;                         /*!< Result to return from function */
    gvol uint32_t ActiveCmdTimeout;                         /*!< Timeout in units of MS for active command to finish */
    
    gvol GSM_NetworkStatus_t NetworkStatus;                 /*!< Network status enumeration */
    
    /*!< SMS management */
    GSM_SMS_t SMS;                                          /*!< SMS Send object */
    GSM_SmsInfo_t SmsInfos[GSM_MAX_RECEIVED_SMS_INFO];      /*!< Received SMS info object */
    
    /*!< Call management */
    GSM_CallInfo_t CallInfo;                                /*!< Call info object */
    
    /*!< Pin checks */
    GSM_CPIN_t CPIN;                                        /*!< SIM status */
    
    /*!< Network options */
    uint8_t IP[4];                                          /*!< Device IP address when connected to network */
    
    /*!< Plain connections check */
    GSM_CONN_t* Conns[6];                                   /*!< Array of pointers to connections */
    
    /*!< HTTP */
    GSM_HTTP_t HTTP;                                        /*!< HTTP related structure */
    
    /*!< FTP */
    GSM_FTP_t FTP;                                          /*!< FTP related structure */
    
#if GSM_RTOS
    /*!< RTOS support */
    GSM_RTOS_SYNC_t Sync;                                     /*!< RTOS Synchronization object */
#endif
    
    /*!< Flags management */
    union {
        struct {
            uint8_t IsBlocking:1;                           /*!< Set to 1 when function is in blocking execution */
            uint8_t Call_Idle:1;                            /*!< Set to 1 when we have to call IDLE callback */
            
            uint8_t PIN_Ok:1;                               /*!< Set to 1 when PIN is entered correct */
            uint8_t PIN_Error:1;                            /*!< Set to 1 when PIN is not connect */
            uint8_t PUK_Ok:1;                               /*!< Set to 1 when PUK is entered correct */
            uint8_t PUK_Error:1;                            /*!< Set to 1 when PUK is not correct */
            
            uint8_t SMS_SendOk:1;                           /*!< SMS was successfully sent */
            uint8_t SMS_SendError:1;                        /*!< We got an error trying to send SMS */
            uint8_t SMS_Read_Data:1;                        /*!< Set to 1 when we are reading actual SMS data */
            uint8_t SMS_CMTI_Received:1;                    /*!< Set to 1 when CMTI SMS info is received and callback should be called */
            
            uint8_t CALL_CLCC_Received:1;                   /*!< Set to 1 when CLCC call info is received and callback should be called */
            uint8_t CALL_RING_Received:1;                   /*!< Set to 1 when RING is received and callback should be called */
            
            uint8_t ReadSingleLineDataRespond:1;            /*!< Set to 1 when we have to read response from command like AT+CGMI. Data is returned as plain text without any special command before */
            
            uint8_t CLIENT_Read_Data:1;                     /*!< Set to 1 when we are reading raw data from client response */
            
            uint8_t HTTP_Read_Data:1;                       /*!< Set to 1 when reading data from HTTP response */
            
            uint8_t FTP_Read_Data:1;                        /*!< Set to 1 when reading data from FTP response */
            
            uint8_t Call_GPRS_Attached:1;                   /*!< Set to 1 when GPRS is attached sucessfully */
            uint8_t Call_GPRS_Attach_Error:1;               /*!< Set to 1 when GPRS was not attached */
            uint8_t Call_GPRS_Detached:1;                   /*!< Set to 1 when GPRS was detached */
            
            uint8_t Call_UV_Warn:1;                         /*!< Set to 1 when undervoltage warning received */
            uint8_t Call_UV_PD:1;                           /*!< Set to 1 when undervoltage power down received */
            
            uint8_t LastOperationStatus:1;
        } F;
        uint32_t Value;                                     /*!< Value containing all the flags in single memory */
    } Flags;                                                /*!< Union holding all the required flags */
    GSM_EventCallback_t Callback;                           /*!< Callback function */
    GSM_EventParams_t CallbackParams;                       /*!< Callback parameters */
    
    union {
        struct {
            uint8_t RespOk:1;                               /*!< OK message response */
            uint8_t RespError:1;                            /*!< Error message response */
            uint8_t RespTimeout:1;                          /*!< Timeout detected */
            uint8_t RespBracket:1;                          /*!< Bracket received (SMS messages) */
            
            uint8_t RespConnectOk:1;                        /*!< n, CONNECT OK was returned from device */
            uint8_t RespConnectFail:1;                      /*!< n, CONNECT FAIL was returned from device */
            uint8_t RespConnectAlready:1;                   /*!< n, ALREADY CONNECTED was returned from device */
            uint8_t RespCloseOk:1;                          /*!< n, CLOSE OK was returned from device */
            uint8_t RespSendOk:1;                           /*!< n, SEND OK was returned from device */
            uint8_t RespSendFail:1;                         /*!< n, SEND FAIL was returned from device */
            
            uint8_t RespHttpAction:1;                       /*!< +HTTPACTION received and already parsed */
            uint8_t RespDownload:1;                         /*!< DOWNLOAD received for download HTTP data */
            
            uint8_t RespCallReady:1;                        /*!< Set to 1 when call is ready */
            uint8_t RespSMSReady:1;                         /*!< Set to 1 when SNS is ready */
            
            uint8_t RespFtpGet:1;                           /*!< Response for FTPGET was received */
            uint8_t RespFtpPut:1;                           /*!< Response for FTPPUT was received */
            uint8_t RespFtpUploadReady:1;                   /*!< Response for FTPPUT was received for uploading data available */
        } F;
        uint32_t Value;                                     /*!< Value containing all the flags in single memory */
    } Events;                                               /*!< Union holding all the required events for library internal processing */
} GSM_t;

/**
 * @}
 */

/**
 * \defgroup GSM_Functions
 * \brief    Library Functions
 * \{
 */

/**
 * \brief  Initializes GSM stack and prepares low-level layer and system calls when necessary
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *pin: Pointer to pin in string format to be used if SIM is inserted and not ready
 * \param  baudrate: Baudrate for UART communication
 * \param  callback: Pointer to callback function to be called on different events. 
 *          Use NULL if you don't need callback
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_Init(gvol GSM_t* GSM, const char* pin, uint32_t baudrate, GSM_EventCallback_t callback);

/**
 * \brief  Checks received data and makes action according to selected action
 * \note   When in <b>RTOS</b> mode, it is recommended to use this function in separate thread only for GSM processing
 *
 * \note   When in <b>ASYNC</b> mode only (without <b>RTOS</b>), it is recommended to use this function in 1ms interrupt handler for processing incoming data
 *
 * \note   When not in <b>RTOS</b> and <b>ASYNC</b> mode, you have to call this function in main while loop as faster as possible.
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_Update(gvol GSM_t* GSM);

/**
 * \brief  Checks for flags and calls callback functions for user.
 * \note   When in <b>RTOS</b> or <b>ASYNC</b> mode, user has to call this function manually. 
 
 * \note   WHen not in <b>RTOS</b> AND <b>ASYNC</b> mode, this function is called from GSM library itself and can be left by user.
 *
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_ProcessCallbacks(gvol GSM_t* GSM);

/**
 * \brief  Update time for GSM stack
 * \note   This function must be called periodically with fixed frequency
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  millis: Number of milliseconds time is increased from last function call
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_UpdateTime(gvol GSM_t* GSM, uint32_t millis);

/**
 * \brief  Gets last return status from stack
 * \note   Use this function in callback function to detect returned status of last operation
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_GetLastReturnStatus(gvol GSM_t* GSM);

/**
 * \brief  Waits for stack to be ready inside desired milliseconds
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  timeout: Timeout to wait before returning in units of milliseconds
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_WaitReady(gvol GSM_t* GSM, uint32_t timeout);

/**
 * \brief  Delay for specific amount of time
 * \note   When not in ASYNC or RTOS mode, this function will also update stack for amount of timeout time
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  timeout: Timeout to delay in units of milliseconds
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_Delay(gvol GSM_t* GSM, uint32_t timeout);

/**
 * \brief  Checks if stack is ready
 * \note   This checks only flag in library. When working with RTOS, this function may not return actual value
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_IsReady(gvol GSM_t* GSM);

/**
 * \brief  Data were received from UART and should be transfered to GSM stack
 * \note   This function should be called from UART RX interrupt for further processing
 * \param  *ch: Pointer to character or array of characters
 * \param  count: Number of characters received from UART 
 * \retval Number of characters added to GSM stack
 */
uint32_t GSM_DataReceived(uint8_t* ch, uint32_t count);

/**
 * \defgroup INFO_API
 * \brief    Informations based functions
 * \{
 */

/**
 * \brief  Gets manufacturer from SIM device
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *str: Pointer to string array to save menufacturer name
 * \param  length: Length of string array
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_INFO_GetManufacturer(gvol GSM_t* GSM, char* str, uint32_t length, uint32_t blocking);

/**
 * \brief  Gets model name from SIM device
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *str: Pointer to string array to save model name
 * \param  length: Length of string array
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_INFO_GetModel(gvol GSM_t* GSM, char* str, uint32_t length, uint32_t blocking);

/**
 * \brief  Gets revision from SIM device
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *str: Pointer to string array to save revision name
 * \param  length: Length of string array
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_INFO_GetRevision(gvol GSM_t* GSM, char* str, uint32_t length, uint32_t blocking);

/**
 * \brief  Gets serial number from SIM device
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *str: Pointer to string array to save serial number
 * \param  length: Length of string array
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_INFO_GetSerialNumber(gvol GSM_t* GSM, char* str, uint32_t length, uint32_t blocking);

/**
 * \brief  Get AT software info on GSM device
 * \param  *ESP: Pointer to working \ref GSM_t structure
 * \param  *rev: Pointer to array of string to save revision version.
 *               Length of array should be at least 30 bytes but mostly depends on actual ESP8266 AT software.
 *               Use NULL if you don't need revision version
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref ESP_Result_t enumeration
 */
GSM_Result_t GSM_INFO_GetSoftwareInfo(gvol GSM_t* GSM, char* rev, uint32_t blocking);

/**
 * \brief  Get battery status informations
 * \param  *ESP: Pointer to working \ref GSM_t structure
 * \parma  *bat: Pointer to empty \ref GSM_Battery_t structure to save response
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref ESP_Result_t enumeration
 */
GSM_Result_t GSM_INFO_GetBatteryInfo(gvol GSM_t* GSM, GSM_Battery_t* bat, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup PIN_API
 * \brief    PIN/PUK based functions
 * \{
 */
 
/**
 * \brief  Enter PIN code for SIM card
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *pin: Pointer to PIN in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PIN_Enter(gvol GSM_t* GSM, const char* pin, uint32_t blocking);

/**
 * \brief  Remove PIN code for SIM card
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *current_pin: Pointer to current PIN in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PIN_Remove(gvol GSM_t* GSM, const char* current_pin, uint32_t blocking);

/**
 * \brief  Set new PIN for SIM
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *new_pin: Pointer to new PIN in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PIN_Add(gvol GSM_t* GSM, const char* new_pin, uint32_t blocking);

/**
 * \brief  Unlock SIM by entering PUK code and new PIN
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *puk: Pointer to SIM PUK code
 * \param  *new_pin: Pointer to new PIN in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PUK_Enter(gvol GSM_t* GSM, const char* puk, const char* new_pin, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup CALL_API
 * \brief    CALL based functions
 * \{
 */

/**
 * \brief  Start voice call to specific number
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *number: Pointer to number in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_Voice(gvol GSM_t* GSM, const char* number, uint32_t blocking);

/**
 * \brief  Start data call to specific number
 * \note   This options has not been actually tested yet
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *number: Pointer to number in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_Data(gvol GSM_t* GSM, const char* number, uint32_t blocking);

/**
 * \brief  Start voice call to specific number selected from sim phonebook
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  pos: Phonebook position on SIM card
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_VoiceFromSIMPosition(gvol GSM_t* GSM, uint16_t pos, uint32_t blocking);

/**
 * \brief  Start data call to specific number selected from sim phonebook
 * \note   This options has not been actually tested yet
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  pos: Phonebook position on SIM card
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_DataFromSIMPosition(gvol GSM_t* GSM, uint16_t pos, uint32_t blocking);

/**
 * \brief  Answer to received call
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_Answer(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Hang up a receiving call or finish with call
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_HangUp(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Get informations about current call, receive, outgoing or active
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Pointer to call info.
 */
GSM_CallInfo_t* GSM_CALL_GetInfo(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Clear informations about call once you are done and you don't need info anymore
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *info: Pointer to \ref GSM_CallInfo_t structure previously returned with \ref GSM_CALL_GetInfo function
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CALL_ClearInfo(gvol GSM_t* GSM, GSM_CallInfo_t* info, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup SMS_API
 * \brief    SMS based functions
 * \{
 */

/**
 * \brief  Send new SMS to specific number
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *number: Phone number to send SMS to
 * \param  *data: SMS data in ASCII format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_SMS_Send(gvol GSM_t* GSM, const char* number, const char* data, uint32_t blocking);

/**
 * \brief  Read SMS from specific SIM memory position
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  position: SMS position in memory.
 * \param  *SMS: Pointer to empty \ref GSM_SMS_Entry_t structure to save data to
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_SMS_Read(gvol GSM_t* GSM, uint16_t position, GSM_SMS_Entry_t* SMS, uint32_t blocking);

/**
 * \brief  List all SMS entries
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  type: SMS type to read. This parameter can be a value of \ref GSM_SMS_ReadType_t enumeration
 * \param  *entries: Pointer to array of \ref GSM_SMS_Entry_t structures
 * \param  btr: Number of entries to read. This parameter can't be greater than number of elements in entries array
 * \param  *br: Pointer to save actual SMS entries read from module
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_SMS_List(gvol GSM_t* GSM, GSM_SMS_ReadType_t type, GSM_SMS_Entry_t* entries, uint16_t btr, uint16_t* br, uint32_t blocking);

/**
 * \brief  Delete specific SMS at desired position
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  position: Position in memory to delete
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_SMS_Delete(gvol GSM_t* GSM, uint16_t position, uint32_t blocking);

/**
 * \brief  Process mass delete SMS entries
 * \note   This function may take a while so take care of using it as blocking.
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  type: Mass delete type. This parameter can be a value of \ref GSM_SMS_MassDelete_t enumeration
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_SMS_MassDelete(gvol GSM_t* GSM, GSM_SMS_MassDelete_t type, uint32_t blocking);

/**
 * \brief  Get informations about received new SMS if possible
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Pointer to \ref GSM_SmsInfo_t structure with informations about received SMS or NULL if nothing received
 */
GSM_SmsInfo_t* GSM_SMS_GetReceivedInfo(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Clear info about received SMS
 * \note   You must call this function after you process everything from \ref GSM_SMS_GetReceivedInfo function call
 *           to allow new SMS received infos to be saved again into free memory.
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *info: Pointer to previously returned pointer from \ref GSM_SMS_GetReceivedInfo function to clear and free memory
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_SMS_ClearReceivedInfo(gvol GSM_t* GSM, GSM_SmsInfo_t* info, uint32_t blocking);

/**
 * \}
 */
 
/**
 * \defgroup PHONEBOOK_API
 * \brief    PHONEBOOK based functions
 * \{
 */
 
/**
 * \brief  Add new entry to phonebook
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *name: Pointer to new phonebook entry name
 * \param  *number: Pointer to entry call number in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PB_Add(gvol GSM_t* GSM, const char* name, const char* number, uint32_t blocking);

/**
 * \brief  Edit phonebook entry
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  index: Index position for entry in phonebook
 * \param  *name: Pointer to new phonebook entry name
 * \param  *number: Pointer to entry call number in string format
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PB_Edit(gvol GSM_t* GSM, uint32_t index, const char* name, const char* number, uint32_t blocking);

/**
 * \brief  Delete phonebook entry
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  index: Index position for entry in phonebook to delete
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PB_Delete(gvol GSM_t* GSM, uint32_t index, uint32_t blocking);

/**
 * \brief  Read phonebook entry
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *entry: Pointer to empty \ref GSM_PB_Entry_t structure to save entry informations
 * \param  index: Index in phonebook to read entry
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PB_Get(gvol GSM_t* GSM, uint32_t index, GSM_PB_Entry_t* entry, uint32_t blocking);

/**
 * \brief  List entries from phonebook
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *entries: Pointer to array of empty entry structures to save data to
 * \param  start_index: Start index number in phonebook to read from
 * \param  btr: Number of entries to read. This number should not be greater than number of elements in entries array
 * \param  *br: Pointer to number to save actual number of entries read from phonebook
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PB_List(gvol GSM_t* GSM, GSM_PB_Entry_t* entries, uint16_t start_index, uint16_t btr, uint16_t* br, uint32_t blocking);

/**
 * \brief  Search for entries in phonebook
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *search: Pointer to string to use in search operation
 * \param  *entries: Pointer to array of empty entry structures to save data to
 * \param  btr: Number of entries to read. This number should not be greater than number of elements in entries array
 * \param  *br: Pointer to number to save actual number of entries read from phonebook
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_PB_Search(gvol GSM_t* GSM, const char* search, GSM_PB_Entry_t* entries, uint16_t btr, uint16_t* br, uint32_t blocking);

/**
 * \}
 */
 
/**
 * \defgroup DATETIME_API
 * \brief    DATETIME based functions
 * \{
 */
 
/**
 * \brief  Get current date and time from network
 * \note   Some network operators does not support this function. In this case, time when module was last reset will be returned.
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *datetime: Pointer to empty \ref GSM_DateTime_t structure to save date and time
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_DATETIME_Get(gvol GSM_t* GSM, GSM_DateTime_t* datetime, uint32_t blocking);

/**
 * \}
 */
 
/**
 * \defgroup GPRS_API
 * \brief    GPRS based functions
 * \{
 */
/**
 * \brief  Connects module to GPRS network to access to internet
 * \note   This operation may take up to 3 minutes so take care when in blocking use. When in <b>RTOS</b> mode, you may use it in blocking mode easily.
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *apn: Pointer to APN string for internet
 * \param  *user: Pointer to user name. Use "" when no user is required
 * \param  *pwd: Pointer to password. Use "" when no password is required
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_GPRS_Attach(gvol GSM_t* GSM, const char* apn, const char* user, const char* pwd, uint32_t blocking);

/**
 * \brief  Disconnects module from GPRS network
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_GPRS_Detach(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Gets GPS location from GPRS and current UTC time
 * \note   Connection with GRPS must be active in order to get information
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *GPS: Pointer to empty ref \GSM_GPS_t to save GPS information and date/time information
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_GPRS_GetLocationAndTime(gvol GSM_t* GSM, GSM_GPS_t* GPS, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup CONN_API
 * \brief    TCP/UDP connection based functions
 * \{
 *
 * \note   Concept of connections is done to support multiple connections at the same time.
 *         However, currently only single connection at a time can be used.
 */

/**
 * \brief  Start a new connection
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *conn: Pointer to empty \ref GSM_CONN_t structure for connection identifier
 * \param  type: Connection type. This parameter can be a value of \ref GSM_CONN_Type_t enumeration
 * \param  ssl: Connection over SSL. This parameter can be a value of \ref GSM_CONN_SSL_t enumeration. Parameter is valid only with TCP connection
 * \param  *host: Pointer to host where to connect. Can be a domain name or IP address, both in sting format
 * \param  port: Port to connect to
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CONN_Start(gvol GSM_t* GSM, gvol GSM_CONN_t* conn, GSM_CONN_Type_t type, GSM_CONN_SSL_t ssl, const char* host, uint16_t port, uint32_t blocking);

/**
 * \brief  Send data on active connection
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *conn: Pointer to working \ref GSM_CONN_t structure for connection
 * \param  *data: Pointer to data to send
 * \param  btw: Number of bytes to send
 * \param  *bw: Pointer to save number of bytes actually sent in connection
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CONN_Send(gvol GSM_t* GSM, gvol GSM_CONN_t* conn, const void* data, uint16_t btw, uint32_t* bw, uint32_t blocking);

/**
 * \brief  Read received data on connection
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *conn: Pointer to working \ref GSM_CONN_t structure for connection
 * \param  *data: Pointer to data array to save receive data to
 * \param  btr: Length of data array in units of bytes
 * \param  *br: Pointer to number of actually read bytes from response
 * \param  timeBeforeRead: Wait time before actual trying to read. This enables you to wait for actual data to receive from response.
 *            Set to 0 if you don't want to wait.
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CONN_Receive(gvol GSM_t* GSM, gvol GSM_CONN_t* conn, void* data, uint16_t btr, uint32_t* br, uint16_t timeBeforeRead, uint32_t blocking);

/**
 * \brief  Close active connection
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *conn: Pointer to working \ref GSM_CONN_t structure for connection
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_CONN_Close(gvol GSM_t* GSM, gvol GSM_CONN_t* conn, uint32_t blocking);

/**
 * \brief  Checks if any data to read from connection response
 * \note   This functions only checks flags and does not inquiry GSM.
 *              It means that when you are not in RTOS or ASYNC mode, 
 *              you have to manually call \ref GSM_Update function to parse any incoming data in a loop for specific timeout
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *conn: Pointer to active \ref GSM_CONN_t connection structure
 * \param  blocking: Status whether this function should be blocking to check for response.
 *           Currently this param has no effect and is reserved for future use
 * \retval Status indicating if data available:
 *           - 0: Data not available
 *           - > 0: Data available for read operation
 */
uint32_t GSM_CONN_DataAvailable(gvol GSM_t* GSM, const gvol GSM_CONN_t* conn, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup HTTP_API
 * \brief    HTTP based functions
 * \{
 */

/**
 * \brief  Begin with HTTP support on SIM module
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_HTTP_Begin(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Stop GSM HTTP SIM module
 * \note   To use it again, use \ref GSM_HTTP_Begin function
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_HTTP_End(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Set content for HTTP request
 * \note   This function does not set actual data for HTTP, but only sets content type, e.g. "application/json".
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *content: Pointer to content type string
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_HTTP_SetContent(gvol GSM_t* GSM, const char* content, uint32_t blocking);

/**
 * \brief  Set data to be sent on HTTP body for POST or PUT methods
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *data: Data to be sent as body
 * \param  btw: Number of bytes to send
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_HTTP_SetData(gvol GSM_t* GSM, const void* data, uint32_t btw, uint32_t blocking);

/**
 * \brief  Execute HTTP request to server with given URL and method
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *url: Remote URL to use starting with "http://" or "https://"
 * \param  method: HTTP method to use. This parameter can be a value of \ref GSM_HTTP_Method_t enumeration
 * \param  ssl: Enable SSL for HTTP. This parameter can be a value of \ref GSM_HTTP_SSL_t enumeration
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
 GSM_Result_t GSM_HTTP_Execute(gvol GSM_t* GSM, const char* url, GSM_HTTP_Method_t method, GSM_HTTP_SSL_t ssl, uint32_t blocking);

/**
 * \brief  Read data from HTTP response
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *data: Pointer to array to save data to
 * \param  btr: Number of bytes to read from response
 * \param  *br: Pointer to save number of actual data read from response
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_HTTP_Read(gvol GSM_t* GSM, void* data, uint32_t btr, uint32_t* br, uint32_t blocking);

/**
 * \brief  Checks if any data to read from HTTP response
 * \note   This functions only checks flags and does not inquiry GSM.
 *              It means that when you are not in <b>RTOS</b> or <b>ASYNC</b> mode, 
 *              you have to manually call \ref GSM_Update function to parse any incoming data in a loop for specific timeout
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response.
 *           Currently this param has no effect and is reserved for future use
 * \retval Status indicating if data available:
 *           - 0: Data not available
 *           - > 0: Data available for read operation
 */
uint32_t GSM_HTTP_DataAvailable(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \}
 */
 
/**
 * \defgroup FTP_API
 * \brief    FTP based functions
 * \since    0.4
 * \{
 */

/**
 * \brief  Enable FTP procedure
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  mode: FTP mode either active or passive. This parameter can be a value of \ref GSM_FTP_Mode_t enumeration
 * \param  ssl: FTP over SSL. This parameter can be a value of \ref GSM_FTP_SSL_t enumeration
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_Begin(gvol GSM_t* GSM, GSM_FTP_Mode_t mode, GSM_FTP_SSL_t ssl, uint32_t blocking);

/**
 * \brief  Disable FTP procedure
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_End(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Authenticate to server with server name, port, username and password
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *server: Pointer to server address
 * \param  port: FTP port to connect. In most cases, port 21 is used
 * \param  *user: FTP username
 * \param  *pass: FTP password
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_Authenticate(gvol GSM_t* GSM, const char* server, uint16_t port, const char* user, const char* pass, uint32_t blocking);

/**
 * \brief  Begin with file download session
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *folder: Folder to use for file download
 * \param  *file: File to download in specific folder. For some FTP servers, this parameter must start with "/" character
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_DownloadBegin(gvol GSM_t* GSM, const char* folder, const char* file, uint32_t blocking);

/**
 * \brief  Checks if FTP download session is active
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_DownloadActive(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Checks if there are bytes to read in current session
 * \note   If may happen that session is active (\ref GSM_FTP_DownloadActive) but there are no data to read.
 *           This means that you will have to wait for incoming data a little first.
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_DownloadAvailable(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Read downloaded data from GSM device
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *data: Data to save data read from GSM
 * \param  btr: Bytes to read from device
 * \param  *br: Pointer to number of bytes actually read from device
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_Download(gvol GSM_t* GSM, void* data, uint32_t btr, uint32_t* br, uint32_t blocking);

/**
 * \brief  Finish with FTP download session
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_DownloadEnd(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \brief  Begin with file upload session
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *folder: Folder to use for file download
 * \param  *file: File to download in specific folder. For some FTP servers, this parameter must start with "/" character
 * \param  mode: Upload mode. This parameter can be a value of \ref GSM_FTP_UploadMode_t enumeration
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_UploadBegin(gvol GSM_t* GSM, const char* folder, const char* file, GSM_FTP_UploadMode_t mode, uint32_t blocking);

/**
 * \brief  Upload data to FTP server
 * \note   This function can be called multiple times when more data needs to be sent
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  *data: Pointer to data to upload to FTP server
 * \param  btw: Number of bytes to write over FTP
 * \param  *bw: Pointer to save number of bytes written over FTP
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_Upload(gvol GSM_t* GSM, const void* data, uint32_t btw, uint32_t* bw, uint32_t blocking);

/**
 * \brief  Finish with FTP upload session
 * \param  *GSM: Pointer to working \ref GSM_t structure
 * \param  blocking: Status whether this function should be blocking to check for response
 * \retval Member of \ref GSM_Result_t enumeration
 */
GSM_Result_t GSM_FTP_UploadEnd(gvol GSM_t* GSM, uint32_t blocking);

/**
 * \}
 */

/**
 * \}
 */
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
