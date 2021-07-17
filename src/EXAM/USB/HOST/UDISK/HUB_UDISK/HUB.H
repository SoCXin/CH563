/********************************** (C) COPYRIGHT *******************************
* File Name          : HUB.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : HUB�������ͷ�ļ�
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include "CH563BAS.H"

#ifndef __HUB_H__
#define __HUB_H__

#ifdef __cplusplus
extern "C" {
#endif

/* USB HUB Class Request Types */
#ifndef USB_CLEAR_HUB_FEATURE
#define USB_CLEAR_HUB_FEATURE           0x20
#define USB_CLEAR_PORT_FEATURE          0x23
#define USB_CLEAR_TT_BUFFER             0x23
#define USB_GET_HUB_DESCR               0xA0
#define USB_GET_HUB_STATUS              0xA0
#define USB_GET_PORT_STATUS             0xA3
#define USB_RESET_TT                    0x23
#define USB_SET_HUB_DESCR               0x20
#define USB_SET_HUB_FEATURE             0x20
#define USB_SET_PORT_FEATURE            0x23
#define USB_GET_TT_STATUS               0xA3
#define USB_STOP_TT                     0x23
#endif

/* Hub Class Feature Selectors */
#define    USB_C_HUB_LOCAL_POWER        0
#define USB_C_HUB_OVER_CURRENT          1
#define USB_PORT_CONNECTION             0
#define USB_PORT_ENABLE                 1
#define USB_PORT_SUSPEND                2
#define USB_PORT_OVER_CURRENT           3
#define USB_PORT_RESET                  4
#define USB_PORT_POWER                  8
#define    USB_PORT_LOW_SPEED           9
#define USB_C_PORT_CONNECTION           16
#define USB_C_PORT_ENABLE               17
#define USB_C_PORT_SUSPEND              18
#define USB_C_PORT_OVER_CURRENT         19
#define USB_C_PORT_RESET                20
#define USB_PORT_TEST                   21
#define USB_PORT_INDICATOR              22

#ifndef ROOT_HUB_DEVICE
typedef struct _ROOT_HUB_DEVICE
{
    UINT8  bHubStatus;                                                          /* ��ǰHUB״̬: 0-���豸,1-���豸����δ��ʼ��,2-���豸����ʼ��ö��ʧ��,3-���豸�ҳ�ʼ��ö�ٳɹ� */
    UINT8  bHubSpeed;                                                           /* ��ǰHUB�ٶȣ�0---δ֪��1---���٣�2---ȫ�٣�3---���� */
    UINT8  bEp0MaxPks;                                                          /* ��ǰHUB�˵�0������С */    
    UINT8  bHubAddress;                                                         /* ��ǰHUB�������USB��ַ */
    UINT8  bHubPortNum;                                                         /* ��ǰHUB�˿���Ŀ */    
}ROOT_HUB_DEVICE, *PROOT_HUB_DEVICE;
#endif

typedef struct _HubInfo 
{ 
    ATTACH_DEVICE_INFO  Device;                                                    
    UINT8  IntInEp;                                                             /* HUB�豸��IN�˵��ַ */
    UINT16 IntInEpSize;                                                         /* HUB�豸��IN�˵�������С */    
    UINT8  InterfNumber;                                                        /* USB�������洢�豸�Ľӿں� */  
}HUB_INFO;                                                                      /* HUB��Ϣ�ṹ��,�洢��ǰU�̱�Ҫ���� */

/******************************************************************************/
/* �������� */
extern UINT8 HUB_CofDescrAnalyse( HUB_INFO *Hubinfo, UINT8 *pbuf );                          /* USB��������HUB Device���������� */
extern UINT8 HUB_GetClassDevDescr( HUB_INFO *Hubinfo, UINT8 *buf, UINT16 *len );             /* USB������ȡHUB�������� */
extern UINT8 HUB_ClearHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector );           /* ���HUB���� */
extern UINT8 HUB_ClearPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 select );/* ���HUB�˿����� */ 
extern UINT8 HUB_GetHubStatus( HUB_INFO *Hubinfo, UINT8 *buf );                              /* ��ѯHUB״̬ */ 
extern UINT8 HUB_GetPortStatus( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port );                 /* ��ѯHUB�˿�״̬ */
extern UINT8 HUB_SetHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector );             /* ����HUB���� */
extern UINT8 HUB_SetPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 select );  /* ����HUB�˿����� */ 
extern UINT8 HUB_CheckPortConnect( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port );             /* ���HUB���ζ˿�����״̬ */
extern UINT8 HUB_CheckPortSpeed( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port );               /* ���HUB���ζ˿������豸���ٶ� */
                        
#ifdef __cplusplus
}
#endif

#endif