/********************************** (C) COPYRIGHT *******************************
* File Name          : UDISK.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : SCSIЭ�����U�����ͷ�ļ�
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include "CH563BAS.H"

#ifndef __UDISK_H__
#define __UDISK_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
#ifdef BIG_ENDIAN
#define USB_BO_CBW_SIG              0x55534243                                  /* �����CBWʶ���־'USBC' */
#define USB_BO_CSW_SIG              0x55534253                                  /* ����״̬��CSWʶ���־'USBS' */
#else
#define USB_BO_CBW_SIG              0x43425355                                  /* �����CBWʶ���־'USBC' */
#define USB_BO_CSW_SIG              0x53425355                                  /* ����״̬��CSWʶ���־'USBS' */
#endif

#define USB_BO_CBW_SIZE             0x1F                                        /* �����CBW���ܳ��� */
#define USB_BO_CSW_SIZE             0x0D                                        /* ����״̬��CSW���ܳ��� */
#define USB_BO_DATA_IN              0x80
#define USB_BO_DATA_OUT             0x00

/* USB������ʽ�Ĳ���״̬���� */
#define USB_INT_SUCCESS             0x14                                        /* USB������ߴ�������ɹ� */
#define USB_INT_CONNECT             0x15                                        /* ��⵽USB�豸�����¼� */
#define USB_INT_DISCONNECT          0x16                                        /* ��⵽USB�豸�Ͽ��¼� */
#define USB_INT_BUF_OVER           0x17                                         /* USB���ƴ��������̫��, ��������� */
#define USB_INT_DISK_ERR           0x1F                                         /* USB�洢������ʧ�� */

typedef union _BULK_ONLY_CMD 
{
    struct 
    {
        UINT32 mCBW_Sig;
        UINT32 mCBW_Tag;
        UINT32 mCBW_DataLen;                                                    /* ����: ���ݴ��䳤�� */
        UINT8  mCBW_Flag;                                                       /* ����: ���䷽��ȱ�־ */
        UINT8  mCBW_LUN;
        UINT8  mCBW_CB_Len;                                                     /* ����: �����ĳ���,��Чֵ��1��16 */
        UINT8  mCBW_CB_Buf[16];                                                 /* ����: �����,�û��������Ϊ16���ֽ� */
    } mCBW;                                                                     /* BulkOnlyЭ��������, ����CBW�ṹ */
    struct 
    {
        UINT32 mCSW_Sig;
        UINT32 mCSW_Tag;
        UINT32 mCSW_Residue;                                                    /* ����: ʣ�����ݳ��� */
        UINT8  mCSW_Status;                                                     /* ����: ����ִ�н��״̬ */
    } mCSW;                                                                     /* BulkOnlyЭ�������״̬��, ���CSW�ṹ */
} BULK_ONLY_CMD;

typedef struct _DiskInfo 
{ 
    ATTACH_DEVICE_INFO  Device;                                                    
    qHD_Structure *qHD_Bulk_In;                                                 /* �����ϴ��˵�EHCI��ؽṹ��ָ�붨�� */
    qHD_Structure *qHD_Bulk_Out;                                                /* �����´��˵�EHCI��ؽṹ��ָ�붨�� */
    UINT8  MaxLun;                                                              /* ��������߼���Ԫ�� */
    UINT8  CurLun;                                                              /* ���̵�ǰ�����߼���Ԫ�� */
    UINT32 Capability;                                                          /* ���������� */
    UINT32 PerSecSize;                                                          /* ����������С */         
    UINT8  BulkInEp;                                                            /* USB�������洢�豸��IN�˵��ַ */
    UINT8  BulkOutEp;                                                           /* USB�������洢�豸��OUT�˵��ַ */
    UINT16 BulkInEpSize;                                                        /* USB�������洢�豸��IN�˵�������С */    
    UINT16 BulkOutEpSize;                                                       /* USB�������洢�豸��OUT�˵�������С */    
    UINT8  InterfNumber;                                                        /* USB�������洢�豸�Ľӿں� */   
}DISK_INFO;                                                                     /* ������Ϣ�ṹ��,�洢��ǰU�̱�Ҫ���� */

/******************************************************************************/
/* �������� */
extern BULK_ONLY_CMD    mBOC;                                                    /* BulkOnly����ṹ */                                                        

/******************************************************************************/
/* �������� */
extern UINT8 MS_CofDescrAnalyse( DISK_INFO *Diskinfo, UINT8 *pbuf );            /* �������������� */                         
extern UINT8 MS_Bluk_QH_Allocate( DISK_INFO *Diskinfo );                        /* USB���������������QH��TD���� */
extern UINT8 MS_GetMaxLun_EX( DISK_INFO *Diskinfo );                            /* USB������ȡ����߼���Ԫ�� */
extern UINT8 MS_ResetErrorBOC_EX( DISK_INFO *Diskinfo );                        /* USB������λU�� */    
extern UINT8 MS_ScsiCmd_Process_EX( DISK_INFO *Diskinfo, UINT8 *DataBuf );      /* ִ�л���BulkOnlyЭ�������� */
extern UINT8 MS_RequestSense_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );            /* USB���������̴���״̬ */
extern UINT8 MS_DiskInquiry_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );             /* USB������ȡ�������� */ 
extern UINT8 MS_DiskCapacity_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );            /* USB������ȡ�������� */
extern UINT8 MS_DiskTestReady_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );           /* USB�������Դ����Ƿ���� */
extern UINT8 MS_Init_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );                    /* USB�������洢�豸��ʼ�� */    
extern UINT8 MS_ReadSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf );/* ������Ϊ��λ�Ӵ��̶�ȡ���� */
extern UINT8 MS_WriteSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );/* ������Ϊ��λ������д����� */

#ifdef __cplusplus
}
#endif

#endif