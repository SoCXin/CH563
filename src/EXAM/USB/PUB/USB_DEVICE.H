/********************************** (C) COPYRIGHT *******************************
* File Name          : USB_DEVICE.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB Device Test Define File
*******************************************************************************/


#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__




#ifdef __cplusplus
extern "C" {
#endif

/* Endpoints Max Packet Size Set */
#define USB_EP0_MAX_PKT                 64
#define USB_EP1_IN_MAX_PKT              8
#define USB_EP2_IN_MAX_PKT              64
#define USB_EP2_OUT_MAX_PKT             64

/* endpoints enumeration */
#define ENDP0                           0x00
#define ENDP1                           0x01    
#define ENDP2                           0x02
#define ENDP3                           0x03    
#define ENDP4                           0x04    
#define ENDP5                           0x05
#define ENDP6                           0x06    
#define ENDP7                           0x07
#define ENDP8                           0x08

/* Block Number of FIFO x */
#define FIFO_BK_NUM_SINGLE              0x00                                    /* Single Block */
#define FIFO_BK_NUM_DOUBLE              0x01                                    /* Double Block */
#define FIFO_BK_NUM_TRIPLE              0x02                                    /* Triple Block */
#define FIFO_BK_NUM_RESERVED            0x03                                    /* Reserved */

/* FIFO x Data Transfer Direction */
#define FIFO_DIR_OUT                    0x00                                    /* OUT */
#define FIFO_DIR_IN                     0x01                                    /* IN */
#define FIFO_DIR_DOUBLE                 0x02                                    /* Bidirectional */
#define FIFO_DIR_RESERVED               0x03                                    /* Not allowed */

/* FIFO x Transfer Type */
#define FIFO_TYPE_RESERVED              0x00                                    /* Reserved */
#define FIFO_TYPE_ISO                   0x01                                    /* Isochronous Type */
#define FIFO_TYPE_BULK                  0x02                                    /* Bulk Type */
#define FIFO_TYPE_INTERRUPT             0x03                                    /* Interrupt Type */

/* FIFO Number of Endpoint x */
#define FIFO_NUM_0                      0x00                                    /* FIFO Number 0 */
#define FIFO_NUM_1                      0x01                                    /* FIFO Number 1 */
#define FIFO_NUM_2                      0x02                                    /* FIFO Number 2 */
#define FIFO_NUM_3                      0x03                                    /* FIFO Number 3 */

/* FIFO Enable/Disable */
#define FIFO_ENABLE                     0x01                                    /* Enable FIFO x */
#define FIFO_DISABLE                    0x00                                    /* Disable FIFO x */

#define FIFO_BK_SIZE_0                  0x00                                    /* Block Size of FIFO x is 0 */
#define FIFO_BK_SIZE_1                  0x01                                    /* Block Size of FIFO x is 1 */

/* ENDP x Type */
#define ENDP_TYPE_IN                    0x00                                    /* ENDP is IN Type */
#define ENDP_TYPE_OUT                   0x01                                    /* ENDP is OUT Type */

/* DMA Target FIFO */    
#define USB_DMA_ACC_NO                  0x00000000                              /* NO DMA Target FIFO */
#define USB_DMA_ACC_F0                  (1 << 0)                                /* DMA Target FIFO is FIFO 0 */
#define USB_DMA_ACC_F1                  (1 << 1)                                /* DMA Target FIFO is FIFO 1 */
#define USB_DMA_ACC_F2                  (1 << 2)                                /* DMA Target FIFO is FIFO 2 */
#define USB_DMA_ACC_F3                  (1 << 3)                                /* DMA Target FIFO is FIFO 3 */
#define USB_DMA_ACC_CXF                 (1 << 4)                                /* DMA Target FIFO is FIFO 4(Control Transfer FIFO) */

/* USB�豸���ֱ�־λ���� */
#define DEF_BIT_USB_RESET               0x01                                    /* ���߸�λ��־ */
#define DEF_BIT_USB_DEV_DESC            0x02                                    /* ��ȡ���豸��������־ */
#define DEF_BIT_USB_ADDRESS             0x04                                    /* ���ù���ַ��־ */
#define DEF_BIT_USB_CFG_DESC            0x08                                    /* ��ȡ��������������־ */
#define DEF_BIT_USB_SET_CFG             0x10                                    /* ���ù�����ֵ��־ */
#define DEF_BIT_USB_WAKE                0x20                                    /* USB���ѱ�־ */
#define DEF_BIT_USB_SUPD                0x40                                    /* USB���߹����־ */
#define DEF_BIT_USB_HS                  0x80                                    /* USB���١�ȫ�ٱ�־ */

/******************************************************************************/
/* �������� */
extern void Delay_ms( int n );
extern void USBDev_ModeSet( void );                                             /* ���ó�USB�豸ģʽ */
extern void USBDev_SpeedSet( UINT8 speed );                                     /* USB�豸�ٶ����� */    
extern UINT8 USBDev_SpeedCheck( void );                                         /* USB�豸�ٶȼ�� */
extern void USBDev_UsbInt_Enable( void );                                       /* USB�豸���USB�ж�ʹ�� */
extern void USBDev_Init( void );                                                /* USB�豸��ʼ�� */
extern void USBDev_SetAddress( UINT8 addre );                                   /* ����USB�豸��ַ */
extern void USBDev_SetConfig( void );                                           /* ����USB�豸���ñ�־λ */
extern void USBDev_EP0_Stall( void );                                           /* USB�豸��������STALL */
extern void USBDev_EPx_IN_Stall( UINT8 epx_num, UINT8 mode );                   /* USB�豸EPx��IN�˵�����STALL */
extern void USBDev_EPx_OUT_Stall( UINT8 epx_num, UINT8 mode );                  /* USB�豸EPx��OUT�˵�����STALL */
extern void USBDev_EPx_IN_ResetToggle( UINT8 epx_num );                         /* USB�豸EPx��IN�˵㸴λͬ��λ */
extern void USBDev_EPx_OUT_ResetToggle( UINT8 epx_num );                        /* USB�豸EPx��OUT�˵㸴λͬ��λ */
extern void USBDev_EPx_IN_SetMaxPKS( UINT8 epx_num, UINT16 maxpks_len );        /* ����USB�豸EPx��IN�˵��������� */
extern void USBDev_EPx_OUT_SetMaxPKS( UINT8 epx_num, UINT16 maxpks_len );       /* ����USB�豸EPx��OUT�˵��������� */
extern void USBDev_EPx_Init( UINT8 epx_num, 
                               UINT8 epx_type,
                             UINT8 fifo_type, 
                             UINT8 fifo_dir, 
                             UINT8 fifo_bk_num, 
                             UINT8 fifo_bk_size, 
                             UINT8 fifo_num, 
                             UINT8 fifo_en,
                             UINT16 maxpks_len );                               /* USB�豸�˵����� */
extern UINT32 USBDev_RD_FIFOx( UINT8 fifonum, UINT32 len, UINT8 *pbuf );        /* ��FIFO�ж�ȡUSB���� */
extern UINT32 USBDev_WR_FIFOx( UINT8 fifonum, UINT32 len, UINT8 *pbuf );        /* ��FIFO��д��USB���� */
extern void USBDev_EP0_Setup_Deal( void );                                      /* USB���ƶ˵�Setup������ */
extern void USBDev_EP0_IN_Deal( void );                                         /* USB���ƶ˵�IN������ */
extern void USBDev_EP0_OUT_Deal( void );                                        /* USB���ƶ˵�OUT������ */
extern void USBDev_EP2_OUT_Deal( void );                                        /* USB�˵�2 OUT������ */
extern void USBDev_EPx_Up_0BytePack( UINT8 epx_num );                           /* USB�˵�EPx�ϴ�0���Ȱ� */
extern void USBDev_IRQHandler( void );                                          /* USB�жϴ��� */
extern void USBDev_Test( void );                                                /* USB�豸ģʽ���������� */
#ifdef __cplusplus
}
#endif

#endif

/********************************************************************************/