/********************************** (C) COPYRIGHT *******************************
* File Name          : EHCI_HCD.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller EHCI test pattern file
*******************************************************************************/


#ifndef __EHCI_HCD_H__
#define __EHCI_HCD_H__

/******************************************************************************/
/* ͷ�ļ����� */
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  "CH563BAS.H"
#include  "CH563USB.H"                                                          /* USB���ö������ͷ�ļ� */    

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* EHCI�ڴ�����˵��: 
   EHCI�ײ�ʹ�������ڴ�ռ���Ҫ���ⲿ���建����EHCI_Allocate_Buf,Ȼ���ɳ�������
   ����ʹ��,�û�������С���û�������Ҫ���ж���,��ͨ���޸���������ݽṹ���Ӧ��
   ����������޸�.
*/

/* EHCI�ڴ������غ궨�� */
#define EHCI_QHD_SIZE                        0x40                               /* EHCI���ݽṹQHD��С */
#define EHCI_QHD_MAX                         14                                 /* EHCI���ݽṹQHD������� */    

#define EHCI_QTD_SIZE                        0x20                               /* EHCI���ݽṹQTD��С */
#define EHCI_QTD_MAX                         14                                 /* EHCI���ݽṹQTD������� */

#define EHCI_ITD_SIZE                        0x40                               /* EHCI���ݽṹITD��С */
#define EHCI_ITD_MAX                         10                                 /* EHCI���ݽṹITD������� */

#define EHCI_P_FRAME_SIZE                    0x04                               /* EHCI����֡�б���С */
#define EHCI_P_FRAME_LIST_MAX                1024                               /* EHCI����֡�б�������� */    

#define EHCI_DATAPAGE_SIZE                   4096                               /* EHCI���ݻ�����ҳ��С */
#define EHCI_DATAPAGE_MAX                    5                                  /* EHCI���ݻ�����ҳ������� */

/* ����EHCI�ڴ���仺������С */
#define EHCI_ALLOCATE_BUF_LEN                 ( ( EHCI_QHD_SIZE * EHCI_QHD_MAX ) + \
                                               ( EHCI_QTD_SIZE * EHCI_QTD_MAX ) + \
                                               ( EHCI_ITD_SIZE * EHCI_ITD_MAX ) + \
                                               ( EHCI_P_FRAME_SIZE * EHCI_P_FRAME_LIST_MAX ) + 64 + 8 )

/* ����EHCI���ݷ��仺������С */
#define EHCI_DATA_BUF_LEN                      ( EHCI_DATAPAGE_SIZE * EHCI_DATAPAGE_MAX )

/* ����EHCI������֡�б���������С */
#define EHCI_PER_LISE_BUF_LEN                 ( EHCI_P_FRAME_SIZE * EHCI_P_FRAME_LIST_MAX )

/*******************************************************************************/                                                                                       
/* EHCI�ڴ������غ궨�� */
#define EHCI_MEM_TYPE_QHD                    0x00
#define EHCI_MEM_TYPE_QTD                    0x01
#define EHCI_MEM_TYPE_ITD                    0x02
#define EHCI_MEM_TYPE_SITD                     0x03
#define EHCI_MEM_TYPE_4K_BUFFER              0x04

#define EHCI_MEM_FREE                           0x01
#define EHCI_MEM_USED                           0x02

/*******************************************************************************/
/* EHCI���ʹ�ܡ���ֹ���� */
#define USBHOST_OP_ENABLE                      0x01
#define USBHOST_OP_DISABLE                     0x00

/*******************************************************************************/
/* Queue Head Structure Definition */
typedef struct _qHD 
{
    /* <1>.Next_qHD_Pointer Word */
    UINT32 bTerminate: 1;                 
    UINT32 bType: 2;                                
    UINT32 bReserve_1: 2;                   
    UINT32 bNextQHDPointer: 27;              
                
    /* <2>.qHD_2 Word */
    UINT32 bDeviceAddress: 7;             
    UINT32 bInactiveOnNextTransaction: 1;       
    UINT32 bEdNumber: 4;                   
    UINT32 bEdSpeed: 2;                    
    UINT32 bDataToggleControl: 1;         
    UINT32 bHeadOfReclamationListFlag: 1; 
    UINT32 bMaxPacketSize: 11;            
    UINT32 bControlEdFlag: 1;             
    UINT32 bNakCounter: 4;                

    /* <3>.qHD_3 Word */    
    UINT32 bInterruptScheduleMask: 8;     
    UINT32 bSplitTransactionMask: 8;      
    UINT32 bHubAddr: 7;                   
    UINT32 bPortNumber: 7;                
    UINT32 bHighBandwidth: 2;             

    /* <4>.Overlay_CurrentqTD */    
    UINT32 bOverlay_CurrentqTD;       

    /* <5>.Overlay_NextqTD */       
    UINT32 bOverlay_NextTerminate: 1;     
    UINT32 bOverlay_Reserve2: 4;               
    UINT32 bOverlay_NextqTD: 27;          

    /* <6>.Overlay_AlternateNextqTD */       
    UINT32 bOverlay_AlternateNextTerminate: 1; 
    UINT32 bOverlay_NanCnt: 4;                  
    UINT32 bOverlay_AlternateqTD: 27;          

    /* <7>.Overlay_TotalBytes */       
    UINT32 bOverlay_Status: 8;              
    UINT32 bOverlay_PID: 2;                   
    UINT32 bOverlay_ErrorCounter: 2;        
    UINT32 bOverlay_C_Page: 3;              
    UINT32 bOverlay_InterruptOnComplete: 1; 
    UINT32 bOverlay_TotalBytes: 15;         
    UINT32 bOverlay_Direction: 1;           

    /* <8>.Overlay_BufferPointer0 */       
    UINT32 bOverlay_CurrentOffset: 12;     
    UINT32 bOverlay_BufferPointer_0: 20;   

    /* <9>.Overlay_BufferPointer1 */       
    UINT32 bOverlay_C_Prog_Mask: 8;         
    UINT32 bOverlay_Reserve3: 4;            
    UINT32 bOverlay_BufferPointer_1: 20;    
      
    /* <10>.Overlay_BufferPointer2 */       
    UINT32 bOverlay_FrameTag: 5;            
    UINT32 bOverlay_S_Bytes: 7;             
    UINT32 bOverlay_BufferPointer_2: 20;    
      
    /* <11>.Overlay_BufferPointer3 */       
    UINT32 bOverlay_Reserve4: 12;           
    UINT32 bOverlay_BufferPointer_3: 20;    
      
    /* <12>.Overlay_BufferPointer4 */       
    UINT32 bOverlay_Reserve5: 12;           
    UINT32 bOverlay_BufferPointer_4: 20;              
}qHD_Structure;

/*******************************************************************************/
/* qTD Structure Definition */ 
typedef struct _qTD 
{ 
    /* <1>.Next_qTD_Pointer Word */
    UINT32 bTerminate: 1;        
    UINT32 bReserve_1: 4;        
    UINT32 bNextQTDPointer: 27;   
                
    /* <2>.Alternate Next qTD Word */
    UINT32 bAlternateTerminate: 1;    
    UINT32 bReserve_2: 4;             
    UINT32 bAlternateQTDPointer: 27;  
     
    /* <3>.Status Word */     
    UINT32 bStatus_PingState: 1;         
    UINT32 bStatus_SplitState: 1;        
    UINT32 bStatus_MissMicroFrame: 1;    
    UINT32 bStatus_Transaction_Err: 1;   
    UINT32 bStatus_Babble: 1;            
    UINT32 bStatus_Buffer_Err: 1;        
    UINT32 bStatus_Halted: 1;            
    UINT32 bStatus_Active: 1;            
       
    UINT32 bPID: 2;                  
    UINT32 bErrorCounter: 2;         
    UINT32 CurrentPage: 3;           
    UINT32 bInterruptOnComplete: 1;  
    UINT32 bTotalBytes: 15;          
    UINT32 bDataToggle: 1;           

    /* <4>.Buffer Pointer Word Array */    
    UINT32 ArrayBufferPointer_Word[ 5 ];       
}qTD_Structure;

#define EHCI_QTD_PID_OUT                     0x00
#define EHCI_QTD_PID_IN                      0x01
#define EHCI_QTD_PID_SETUP                   0x02
    
#define EHCI_QTD_STATUS_ACTIVE                  0x80
#define EHCI_QTD_STATUS_HALTED               0x40
#define EHCI_QTD_STATUS_BUF_ERROR            0x20
#define EHCI_QTD_STATUS_BABBLE               0x10
#define EHCI_QTD_STATUS_TRANS_ERROR          0x08
#define EHCI_QTD_STATUS_MISS_FRAME           0x04
#define EHCI_QTD_STATUS_SPLIT                0x02
#define EHCI_QTD_STATUS_PING                 0x01

/*******************************************************************************/
/* iTD Structure Definition */ 
typedef struct _iTD_Status 
{     
    UINT32 bOffset: 12;             
    UINT32 bPageSelect: 3;             
    UINT32 bInterruptOnComplete: 1; 
    UINT32 bLength: 12;             
    UINT32 bStatus: 4;                             
}iTD_Status_Structure;

typedef struct _iTD_BufferPointer 
{
    UINT32 bMultiFunction: 12;     
    UINT32 bBufferPointer: 20;           
}iTD_BufferPointer_Structure;
 
typedef struct _iTD 
{
    /* <1>.Next_Link_Pointer Word */
    UINT32 bTerminate: 1;      
    UINT32 bType: 2;           
    UINT32 bReserve_1: 2;      
    UINT32 bNextLinkPointer: 27; 
                
    /* <2>.Status Word */
    iTD_Status_Structure ArrayStatus_Word[ 8 ];    
     
    /* <3>.Buffer_Pointer Word */     
    iTD_BufferPointer_Structure ArrayBufferPointer_Word[ 7 ];                
}iTD_Structure;

#define HOST20_iTD_Status_Active             0x08
#define HOST20_iTD_Status_DataBufferError    0x04
#define HOST20_iTD_Status_BabbleDetect       0x02
#define HOST20_iTD_Status_TransctionError    0x01

/*******************************************************************************/
/* siTD Structure Definition */
typedef struct _siTD 
{     
    /* <1>.1 Word */ 
    UINT32 bTerminate: 1;           
    UINT32 bType: 2;                     
    UINT32 bReserve_1: 2;           
    UINT32 bNextQHDPointer: 27;     
                
    /* <2>.2 Word */ 
    UINT32 bDeviceAddress: 7;       
    UINT32 bReserve_21: 1;              
    UINT32 bEdNumber: 4;            
    UINT32 bReserve_22: 4;          
    UINT32 bHubAddr: 7;             
    UINT32 bReserve_23: 1;               
    UINT32 bPortNumber: 7;          
    UINT32 bInOut: 1;               

    /* <3>.3 Word */      
    UINT32 bS_Mask: 8;                    
    UINT32 bC_Mask: 8;                   
    UINT32 bReserve_31: 16;               

    /* <4>.4 Word */      
    UINT32 bStatus: 8;     
    UINT32 bC_Prog_Mask: 8;          
    UINT32 bTotalBytesToTransfer: 10;          
    UINT32 bReserve_41: 4;    
    UINT32 bPageSelect: 1;         
    UINT32 bInterruptOnComplete: 1;         

    /* <5>.5 Word */      
    UINT32 bCurrentOffset: 12;   
    UINT32 bBufferPointer_Page0: 20;          

    /* <6>.6 Word */      
    UINT32 bT_Count: 3;   
    UINT32 bTransactionPosition: 2;
    UINT32 bReserve_61: 7;         
    UINT32 bBufferPointer_Page1: 20; 
      
    /* <7>.7 Word */     
    UINT32 bBP_Terminate: 1;   
    UINT32 bReserve_71: 4;  
    UINT32 bBackPointer: 27;            
}siTD_Structure;

typedef struct
{
    /* <1>.Next_Link_Pointer Word */
    UINT32   bTerminal: 1;                     
    UINT32   bType: 2;                             
    UINT32   bReserved: 2;                    
    UINT32   bLinkPointer: 27;                     
}Periodic_Frame_List_Cell_Structure;

typedef struct  
{     
    Periodic_Frame_List_Cell_Structure   sCell[ EHCI_P_FRAME_LIST_MAX ]; 

}Periodic_Frame_List_Structure;

#define HOST20_siTD_Status_Active            0x80

#define HOST20_siTD_TP_All                   0x00
#define HOST20_siTD_TP_Begin                 0x01
#define HOST20_siTD_TP_Mid                   0x02
#define HOST20_siTD_TP_End                   0x03

#define EHCI_HD_TYPE_ITD                     0x00
#define EHCI_HD_TYPE_QH                      0x01
#define EHCI_HD_TYPE_SITD                    0x02
#define EHCI_HD_TYPE_FSTN                    0x03


/* �����豸�ٶȶ��� */
#define EHCI_DEVICE_SPEED_LS                 0x01                               /* ��ǰΪ�����豸 */
#define EHCI_DEVICE_SPEED_FS                 0x00                               /* ��ǰΪȫ���豸 */
#define EHCI_DEVICE_SPEED_HS                 0x02                               /* ��ǰΪ�����豸 */

/* �˵��Ƿ�Ϊ���ƶ˵㶨�� */
#define EHCI_EP_TYPE_CONTROL                 0x01
#define EHCI_EP_TYPE_NO_CONTROL              0x00


#define EHCI_PORT_INIT                       0
#define DEFAULT_USB_EP0_SIZE                 64                                 /* Ĭ��USB�豸�˵�0������С */                                   
#define DEFAULT_USB_OP_ADDR                  2                                  /* Ĭ�ϲ���USB�豸�ĵ�ַ */
#define DEFAULT_MAX_OPERATE_SIZE             20480                              /* Ĭ�ϵ�ǰ����������С */                                   

#define DEF_USB_TIMEOUT                      50000                              /* USB�жϳ�ʱʱ�䣬��λuS */
    
/* ����״̬����1XH����USB������ʽ�Ĳ���״̬����,�������ж�������ֵ,����Ķ����Ϊ�˼���CH37x */
#define    ERR_USB_UNKNOWN                   0xFA                               /* δ֪����,��Ӧ�÷��������,����Ӳ�����߳������ */
#define    ERR_USB_TIMEOUT                   0xFB                               /* �жϳ�ʱ/NAK���Գ�ʱ */

#ifndef    USB_INT_SUCCESS
#define    USB_OPERATE_SUCCESS               0x00                               /* USB�����ɹ� */
#define    USB_OPERATE_ERROR                 0x01                               /* USB����ʧ�� */
#define    USB_HC_ERROR                      0x02                               /* USB�������������ڴ��� */
#define    USB_PARAMETER_ERROR               0x03                               /* ��������, ��Χ��� */
#define    USB_MEM_ALLOCATE_ERROR            0x04                               /* �ڴ����ʧ�� */
#define    USB_INT_SUCCESS                   0x14                               /* USB������ߴ�������ɹ� */
#define    USB_INT_CONNECT                   0x15                               /* ��⵽USB�豸�����¼� */
#define    USB_INT_DISCONNECT                0x16                               /* ��⵽USB�豸�Ͽ��¼� */
#define    USB_INT_BUF_OVER                  0x17                               /* USB���ƴ��������̫��, ��������� */
#define    USB_HUBDEVICE_CONNECT             0x18                               /* ��⵽HUB�µ�USB�豸�����¼� */
#define    USB_HUBDEVICE_DISCONNECT          0x19                               /* ��⵽HUB�µ�USB�豸�Ͽ��¼� */
#define    USB_INT_ERROR                     0x1F                               /* USB�豸����ʧ�� */
#endif
#ifndef    USB_INT_CONNECT_LS
#define    USB_INT_CONNECT_LS                0x13                               /* ��⵽����USB�豸�����¼� */
#endif

/* ���ӵ�USB�豸�����Ϣ */
typedef struct _ATTACH_DEVICE_INFO
{
    UINT8  bDeviceOnHub;                                                        /* ��ǰUSB�豸�Ƿ�������HUB��: 0---��, 1---�� */
    UINT8  bOnHubPortNumber;                                                    /* ��ǰUSB�豸��HUB�ϵĶ˿ں� */
    UINT8  bAddress;                                                            /* ��ǰUSB�豸��ַ��Ĭ��Ϊ0 */
    UINT8  bHubAddress;                                                         /* ��ǰUSB���ص�HUB��ַ��Ĭ��Ϊ0 */    
    UINT8  bConnectStatus;                                                      /* ��ǰUSB�豸����״̬��0---δ����,1---����δö�٣�2---ö�ٳɹ� */
    UINT8  bPortEnableDisableStatus;
    UINT8  bSpeed;                                                              /* ��ǰUSB�豸�ٶȣ�0xFF---δ֪��0x01---���٣�0x00---ȫ�٣�0x02---���� */
    UINT8  bEp0MaxPks;                                                          /* ��ǰUSB�豸�˵�0������С */    
    UINT8  bPortReset;  
    UINT8  bSuspend;  
    UINT8  bRemoteWakeUpDetected;  
    UINT8  bSendOK;
    UINT8  bSendStatusError;
    qHD_Structure *qHD_Control;                                                 /* ���ƶ˵�qHD���� */
    qTD_Structure *psSendLastqTD;    
    UINT8  *bDataBuffer;
}ATTACH_DEVICE_INFO, *pATTACH_DEVICE_INFO;

/*******************************************************************************/
/* ������������ */
/* EHCI�Ĵ�����ؽṹ��ָ�붨�� */
extern EHCI_CAPABILITY *EHCI_Caps;
extern EHCI_REGISTERS  *EHCI_Regs;

/* USB�������ƴ���SETUP���ṹ��ָ�붨�� */
extern USB_SETUP_REQ   *Ctl_Setup;
extern UINT8  *Ctl_Data;

/* ���������������� */
extern UINT8   gDeviceClassType;                                                /* ��ǰ�豸���� */                                            
extern UINT8V  gEHCILastStatus;                                                 /* EHCI���һ�β���״̬ */
extern UINT32  gEHCIFreeRamAddr;                                                /* EHCI�ڴ���������ʼ��ַ */    

extern UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                        /* �ڴ���仺����(����������QH��QTD����ڴ�ʹ��) */
extern UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                                /* �ڴ����ݻ�����(������Ҫ4K����,ǿ�Ʒ���ָ���ڴ�ռ�) */                                
extern UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];                   /* ������֡�б�������(��Ҫ4K����) */

/*******************************************************************************/
/* �������� */
extern void Delay_us( int n );                                                  /* ΢�뼶��ʱ,����ȷ */
extern void Delay_ms( int n );                                                  /* ���뼶��ʱ,����ȷ */
extern void USBHOST_ModeSet( void );                                            /* ����USB����ģʽ */ 
extern void USBHOST_INT_Enable( void );                                         /* USB�������USB�ж�ʹ�� */
extern void USBHOST_INT_Disable( void );                                        /* USB�������USB�жϽ�ֹ */
extern UINT8 USBHOST_HC_StopRun_Setting( UINT8 option );                        /* USB����������ֹͣ���������� */
extern UINT8 USBHOST_Asynchronous_Setting( UINT8 option );                      /* USB�������á�ֹͣAsynchronous���� */
extern UINT8 USBHOST_Periodic_Setting( UINT8 option );                          /* USB�������á�ֹͣPeriodic���� */
extern UINT8 USBHOST_EHCI_Init( void );                                         /* USB����EHCI��ʼ�� */
extern UINT8 USBHOST_Structure_DeInit( void );                                  /* USB�����������������ݽṹ�����ڴ���估��ʼ�� */
extern UINT32 USBHOST_GetStructure( UINT8 Type );                               /* �������ݽṹ�����ڴ� */
extern UINT8 USBHOST_ReleaseStructure( UINT8 Type, UINT32 pwAddress );          /* �������ݽṹ��������ڴ� */
extern UINT8 USBHOST_Allocate_QHD( ATTACH_DEVICE_INFO *Device, qHD_Structure *psQHTemp, UINT8 bQHType, UINT8 bType, UINT8 bHead, UINT8 bEndPt, UINT32 wMaxPacketSize ); /* ��ʼ�������QH */
extern UINT8 USBHOST_Resume( ATTACH_DEVICE_INFO *Device );
extern UINT8 USBHOST_Suspend( ATTACH_DEVICE_INFO *Device );
extern void DEBUG_Print_Memory( UINT32 startaddr, UINT16 size );                /* ��ӡָ���ڴ�ռ����� */
extern void USBHOST_DevicePara_Init( ATTACH_DEVICE_INFO *Device );              /* ��ʼ���豸��ز��� */
extern UINT8 USBHOST_EHCI_BusReset( ATTACH_DEVICE_INFO *Device );               /* USB�����������߸�λ */
extern UINT8 USBHOST_EHCI_Detect_Connect( ATTACH_DEVICE_INFO *Device );         /* USB��������豸��� */
extern UINT8 USBHOST_EHCI_EP0_Init( ATTACH_DEVICE_INFO *Device );               /* USB�������ƶ˵�0��ʼ�� */
extern UINT8 USBHOST_Send_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD, UINT16 *pRetLen, UINT32 wTimeOutSec ); /* USB��������һ��QTD */
extern UINT8 USBHOST_Issue_Control( ATTACH_DEVICE_INFO *Device, USB_SETUP_REQ *pCmdPack, UINT8 *pDatBuf, UINT16 *pRetLen );/* USB��������һ�����ƴ��� */
extern UINT8 USBHOST_Issue_Bulk( ATTACH_DEVICE_INFO *Device, qHD_Structure *pBlukqHD, UINT8 *pDatBuf, UINT16 *pSize, UINT8 Type ); /* USB����ִ���������� */
extern UINT8 USBHOST_Issue_Interrupt( ATTACH_DEVICE_INFO *Device, qHD_Structure *pIntqHD, UINT8 *pDatBuf, UINT16 *pSize, UINT8 Type );    /* USB����ִ���жϴ��� */
extern UINT8 USBHOST_EHCI_GetDeviceDescr( ATTACH_DEVICE_INFO *Device, UINT8 *buf, UINT16 *len );    /* USB������ȡ�豸������ */
extern UINT8 USBHOST_EHCI_GetConfigDescr( ATTACH_DEVICE_INFO *Device, UINT8 *buf, UINT16 *len );    /* USB������ȡ���������� */ 
extern UINT8 USBHOST_EHCI_SetAddress( ATTACH_DEVICE_INFO *Device, UINT8 addr ); /* USB�������õ�ַ */
extern UINT8 USBHOST_EHCI_SetConfig( ATTACH_DEVICE_INFO *Device, UINT8 cfg );   /* USB������������ֵ */
extern UINT8 USBHOST_EHCI_ClearEndpStall( ATTACH_DEVICE_INFO *Device, UINT8 endp );    /* USB��������˵� */
extern UINT8 USBHOST_CofDescrAnalyse( UINT8 *pbuf );                            /* �������������� */
extern UINT8 USBHOST_EHCI_Enumerate( ATTACH_DEVICE_INFO *Device, UINT8 *pbuf ); /* USB�������豸����ö�� */
extern UINT8 USBHOST_qHD_Link_Insert( qHD_Structure *CurrentqHD );              /* ����ǰqHD���ӵ������� */    
extern UINT8 USBHOST_QH_Link_BackOut( qHD_Structure *CurrentQH );               /* ����ǰQH��������ժ�� */
extern UINT8 USBHOST_Release_QHD( qHD_Structure *CurrentQH );                   /* �ͷŵ�ǰQHD(����������ժ��������QH�¹��ص�TD���Լ�����QH) */
extern UINT32 USBHOST_Issue_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD );
extern UINT8 USBHOST_Check_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD, UINT16 *pRetLen );
extern UINT8 USBHOST_Remove_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD );

#ifdef __cplusplus
}
#endif

#endif

/* ********************************************************************************************************************* */

