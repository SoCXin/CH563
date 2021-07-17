/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Examples
*                      (1)��CH563 USB Examples by KEIL��    
*                      (2)������0��������Ϣ,115200bps��
*                      (3)��USB HOST�Բ�ѯ��ʽ������
*                      (4)����������:����HID���������豸�� 
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "absacc.h"
#include "SYSFREQ.H"                                                            /* ϵͳ�������ͷ�ļ� */
#include "CH563SFR.H"
#include "EHCI_HCD.H"                                                           /* �����������Ͳ�������ͷ�ļ� */
#include "CH563USBSFR.H"                                                        /* оƬUSB�Ĵ������ͷ�ļ� */
#include "HID.H"                                                                /* HID�������ͷ�ļ� */
#include "PRINTF.H"                                                             /* ���ڴ�ӡ�������ͷ�ļ� */

/******************************************************************************/
/* ������������ */
UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                               /* �ڴ���仺����(����QH��QTD�Ƚṹ����Ҫ32�ֽڶ���, ����������QH��QTD����ڴ�ʹ��) */
__align( 4096 ) UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                       /* �ڴ����ݻ�����(��Ҫ4K����) */                                
__align( 4096 ) UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];          /* ������֡�б�������(��Ҫ4K����) */                                
/* ����HID����ؽṹ�� */
HID_INFO        Hid;                                                            /* HID��ؽṹ�嶨�� */    
Periodic_Frame_List_Structure  *pHID_FramList;                                  /* ������֡�б��ṹ�� */

/******************************************************************************/
/* �������� */
extern UINT8 Hid_Test( HID_INFO *Hidinfo );                                     /* HID���������� */        

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    while( 1 );
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ�жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{
    while( 1 );
}

/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : Ϊprintf��getkey���������ʼ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void mInitSTDIO( void )
{
    UINT32    x, x2;

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        /* 115200bps */
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        /* �������� */
    R8_UART0_LCR = 0x80;                                                        /* DLABλ��1 */
    R8_UART0_DIV = 1;                                                           /* Ԥ��Ƶ */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* �����ֽڳ���Ϊ8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* ����FIFO������Ϊ14���巢�ͺͽ���FIFO��FIFOʹ�� */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : ͨ��������������Ϣ
* Input          : c-- writes the character specified by c 
*                  *f--the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* �������� */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* �ȴ����ݷ��� */
    return( c );
}

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
    UINT8  status;

    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */

#ifdef  MY_DEBUG_PRINTF    
    printf( "CH563_HUB_Example_Text\n" );
#endif

    USBHOST_ModeSet( );                                                         /* ����USB����ģʽ */
    USBHOST_EHCI_Init( );                                                       /* USB����EHCI��λ����ʼ�� */
    USBHOST_Structure_DeInit( );                                                /* USB�������ݽṹ��ʼ�� */
    USBHOST_DevicePara_Init( &Hid.Device );                                     /* USB������ز�����ʼ�� */

    while( 1 )
    {                
        /***********************************************************************/
        /* ��ѯ�ȴ��豸���� */
#ifdef  MY_DEBUG_PRINTF 
        printf("Wait USB Device In:\n" );
#endif
HOST_WAIT_DEVICE_IN:
        while( 1 )
        {
            if( USBHOST_EHCI_Detect_Connect( &Hid.Device ) == USB_INT_CONNECT ) /* ����豸���� */
            {
                break;
            }
               Delay_ms( 50 );
        }
        Delay_ms( 200 );                                                        /* ��ʱ */
        if( USBHOST_EHCI_Detect_Connect( &Hid.Device ) == USB_INT_DISCONNECT )  /* ����豸���� */
        {
            goto HOST_WAIT_DEVICE_IN;                                           /* �豸�Ͽ���ת��ǰ������ȴ� */
        }

#ifdef  MY_DEBUG_PRINTF            
        printf("USB Device In:\n" ); 
        printf("Device Speed is: %02x\n",(UINT16)Hid.Device.bSpeed );           /* 0x01:�����豸, 0x00:ȫ���豸, 0x02:�����豸, 0xFF:δ֪ */            
#endif

        /**********************************************************************/
        /* ��USB�豸����ö�� */    
        status = USBHOST_EHCI_EP0_Init( &Hid.Device );                          /* ��ʼ���˵�0 */
        if( status != USB_OPERATE_SUCCESS )
        {
#ifdef  MY_DEBUG_PRINTF            
            printf("USBHOST_EP0_Init Error\n");
#endif                
            return( status );
        }

        status = USBHOST_EHCI_Enumerate( &Hid.Device, EHCI_Data_Buf );          /* USB�������豸����ö�� */
        if( status == USB_OPERATE_SUCCESS )
        {
            if( gDeviceClassType == USB_DEV_CLASS_HUMAN_IF )                    /* ��ǰ�豸ΪHID���豸 */
            {
#ifdef  MY_DEBUG_PRINTF    
                printf("Current USB Device is HID Device\n");                
#endif
                   status = Hid_Test( &Hid );                                   /* HID���������������� */    
            }    
        }

        /**********************************************************************/
        /* ���Խ���,��ѯ�ȴ��豸�γ� */
        while( 1 )
        {
            if( USBHOST_EHCI_Detect_Connect( &Hid.Device ) == USB_INT_DISCONNECT )
            {
                printf("USB Device Out\n");
                USBHOST_EHCI_Init( );
                USBHOST_DevicePara_Init( &Hid.Device );                         /* USB������ز�����ʼ�� */
                USBHOST_Structure_DeInit( );                                    /* USB�������ݽṹ��ʼ�� */
#ifdef  MY_DEBUG_PRINTF    
                printf("USB Device Out\n");
#endif
                break;
            }
        }    
    }
}

/*******************************************************************************
* Function Name  : Hid_Test
* Description    : HID����������
* Input          : *Hidinfo----��ǰ������HID���豸��ؽṹ��
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 Hid_Test( HID_INFO *Hidinfo )
{
    UINT8  status;
    UINT16 i, len;
    UINT8  buf1[ 8 ];
    UINT8  buf2[ 8 ];
        
#ifdef  MY_DEBUG_PRINTF            
    printf("Hid_Test......\n");        
#endif
    
    /* ��U�����������з��� */
    HID_CofDescrAnalyse( Hidinfo, EHCI_Data_Buf );                              /* ��U�̵��������������з���,��ȡ������Ϣ */

    /* ��HID���豸������������� */
    status = HID_ClassRequest_Deal( Hidinfo, EHCI_Data_Buf );
    if( status )
    {
        return( status );
    }

    /* ��ֹ�����Դ��� */    
    USBHOST_Periodic_Setting( USBHOST_OP_DISABLE );

    /* ʹ��������֡����ȫ����Ч */
       pHID_FramList = ( Periodic_Frame_List_Structure *)EHCI_PERIODIC_LIST_Buf;
    for( i = 0; i < EHCI_P_FRAME_LIST_MAX; i = i + 1 ) 
    {
        pHID_FramList->sCell[ i ].bTerminal = 0x01;
    }
    
    /* ���ж϶˵�1���QHD���ӵ�������֡�����У�����ִ������λ8mS */
    for( i = 0; i < EHCI_P_FRAME_LIST_MAX; i = i + 8 ) 
    {
        pHID_FramList->sCell[ i ].bLinkPointer = ((UINT32)( Hidinfo->qHD_INT_In1 ) ) >> 5;  
        pHID_FramList->sCell[ i ].bTerminal = 0;
        pHID_FramList->sCell[ i ].bType = EHCI_HD_TYPE_QH;      
    }

    /* ���ж϶˵�2���QHD���ӵ�������֡�����У�����ִ������λ8mS */
    if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
    {
        for( i = 1; i < EHCI_P_FRAME_LIST_MAX; i = i + 8 ) 
        {
            pHID_FramList->sCell[ i ].bLinkPointer = ((UINT32)( Hidinfo->qHD_INT_In2 ) ) >> 5;  
            pHID_FramList->sCell[ i ].bTerminal = 0;
            pHID_FramList->sCell[ i ].bType = EHCI_HD_TYPE_QH;      
        }
    }

    /* ʹ�������Դ��� */    
    USBHOST_Periodic_Setting( USBHOST_OP_ENABLE );

    /* ��ȡ����������� */
#ifdef  MY_DEBUG_PRINTF    
    printf("Start Get HID Device Data\n ");
#endif

    /* ��ȡ��һ���ж϶˵����ݰ� */        
    HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In1, buf1, Hidinfo->IntInEp1Size );

    /* ��ȡ�ڶ����ж϶˵����ݰ� */    
    if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
    {
           HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In2, buf2, Hidinfo->IntInEp2Size );
    }

    while( 1 )
    {
        /* ��ѯ��һ���ж϶˵��Ƿ������ݰ� */
        len = Hidinfo->IntInEp1Size;
        status = HID_Check_Data( Hidinfo, Hidinfo->qHD_INT_In1, &len );
        if( status == USB_OPERATE_SUCCESS )
        {
            /* ������ȡ���ݰ� */
            for( i = 0x00; i < len; i++ )
            {
                printf("%02x ",(UINT16)buf1[ i ]);
            }
            printf("\n");
        
            /* ���»�ȡ��һ���ж϶˵����ݰ� */
               HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In1, buf1, Hidinfo->IntInEp1Size );
        }

        /* ��ѯ�ڶ����ж϶˵��Ƿ������ݰ� */
        if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
        {                
            len = Hidinfo->IntInEp2Size;
            status = HID_Check_Data( Hidinfo, Hidinfo->qHD_INT_In2, &len );
            if( status == USB_OPERATE_SUCCESS )
            {
                /* ������ȡ���ݰ� */
                for( i = 0x00; i < len; i++ )
                {
                    printf("%02x ",(UINT16)buf2[ i ]);
                }
                printf("\n");
            
                /* ���»�ȡ�ڶ����ж϶˵����ݰ� */
                   HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In2, buf2, Hidinfo->IntInEp2Size );
            }
        }

        /* ��ѯ�豸���� */
        if( USBHOST_EHCI_Detect_Connect( &Hidinfo->Device ) == USB_INT_DISCONNECT )
        {
            /* ����ǿ�Ƴ����ö˵��ȡ�������� */
            HID_Stop_GetData( Hidinfo, Hidinfo->qHD_INT_In1 );        
            if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
            {
                HID_Stop_GetData( Hidinfo, Hidinfo->qHD_INT_In2 );    
            }
            return( USB_INT_DISCONNECT );
        }                    
    }
}

/*********************************** endfile **********************************/
