/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Examples
*                      (1)��CH563 USB Examples by KEIL��    
*                      (2)������0��������Ϣ,115200bps��
*                      (3)��USB HOST�Բ�ѯ��ʽ������
*                      (4)����������:����U��,��������������д�� 
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
#include "Udisk.H"                                                              /* Udisk�������ͷ�ļ� */
#include "PRINTF.H"                                                             /* ���ڴ�ӡ�������ͷ�ļ� */

/******************************************************************************/
/* ������������ */
UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                               /* �ڴ���仺����(����QH��QTD�Ƚṹ����Ҫ32�ֽڶ���, ����������QH��QTD����ڴ�ʹ��) */
__align( 4096 ) UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                       /* �ڴ����ݻ�����(��Ҫ4K����) */                                
__align( 4096 ) UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];          /* ������֡�б�������(��Ҫ4K����) */                                
/* ����U����ؽṹ�� */
BULK_ONLY_CMD    mBOC;                                                          /* BulkOnly����ṹ���� */
DISK_INFO        UDisk;                                                         /* U����ؽṹ�嶨�� */                                                        
DISK_INFO        HUB_UDisk[ 4 ];                                                /* HUB��U����ؽṹ�嶨�� */
/******************************************************************************/
/* �������� */
extern UINT8 Disk_Test( DISK_INFO *Diskinfo );                                  /* U�̲��������� */

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
* Function Name  : Disk_Test
* Description    : U�̲���������
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
* Output         : None
* Return         : None
*******************************************************************************/

UINT8 Disk_Test( DISK_INFO *Diskinfo )
{
    UINT8  status;
    UINT32 sector;
    
#ifdef  MY_DEBUG_PRINTF            
    printf("Disk_Test......\n");        
#endif
    /* ��U�����������з��� */
    MS_CofDescrAnalyse( Diskinfo, EHCI_Data_Buf );                              /* ��U�̵��������������з���,��ȡ������Ϣ */

    /* ��U�̽��г�ʼ������ */
    status = MS_Init_EX( Diskinfo, EHCI_Data_Buf );                             /* �������洢�豸��ʼ�� */
    if( status == USB_OPERATE_SUCCESS )
    {        
        sector = 0x00;
        while( 1 )  
        {
            /* ����һ�ζ�ȡ20K���� */
            status = MS_ReadSector_EX( Diskinfo, sector, EHCI_DATA_BUF_LEN / 
                Diskinfo->PerSecSize, EHCI_Data_Buf );                          /* ��ȡN���������� */                        
            if( status )
            {
                return( status );
            }
            /* �ж��Ƿ����ȫ������,�����ͷ�ٶ� */
            if( sector <= ( Diskinfo->Capability - 40 ) )
            {

                sector = sector + EHCI_DATA_BUF_LEN / Diskinfo->PerSecSize;                        
            }
            else
            {
                sector = 0x00;    
            }
        }
    }
    return( status );
}    

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int    main( void ) 
{
    UINT8  status;

//    SysFreq( );                                                               /* ������Ҫ����PLL�����Ӧ��ʱ��Ƶ�� */
    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */

#ifdef  MY_DEBUG_PRINTF    
    printf( "CH563_HUB_Example_Test\xd\xa" );
#endif

    USBHOST_ModeSet( );                                                         /* ����USB����ģʽ */
    USBHOST_EHCI_Init( );                                                       /* USB����EHCI��λ����ʼ�� */
    USBHOST_Structure_DeInit( );                                                /* USB�������ݽṹ��ʼ�� */
    USBHOST_DevicePara_Init( &UDisk.Device );                                   /* USB������ز�����ʼ�� */

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
            if( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) == USB_INT_CONNECT ) /* ����豸���� */
            {
                break;
            }
               Delay_ms( 50 );
        }
        Delay_ms( 200 );                                                        /* ��ʱ */
        if( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) == USB_INT_DISCONNECT )/* ����豸���� */
        {
            goto HOST_WAIT_DEVICE_IN;                                           /* �豸�Ͽ���ת��ǰ������ȴ� */
        }

#ifdef  MY_DEBUG_PRINTF            
        printf("USB Device In:\n" ); 
        printf("Device Speed is: %02x\n",(UINT16)UDisk.Device.bSpeed );         /* 0x01:�����豸, 0x00:ȫ���豸, 0x02:�����豸, 0xFF:δ֪ */            
#endif

        /***********************************************************************/
        /* ��USB�豸����ö�� */    
        status = USBHOST_EHCI_EP0_Init( &UDisk.Device );                        /* ��ʼ���˵�0 */
        if( status != USB_OPERATE_SUCCESS )
        {
#ifdef  MY_DEBUG_PRINTF            
            printf("USBHOST_EP0_Init Error\n");
#endif                
            return( status );
        }

        status = USBHOST_EHCI_Enumerate( &UDisk.Device, EHCI_Data_Buf );        /* USB�������豸����ö�� */
        if( status == USB_OPERATE_SUCCESS )
        {
            if( gDeviceClassType == USB_DEV_CLASS_STORAGE )                     /* ��ǰ�豸Ϊ�������洢�豸 */
            {
#ifdef  MY_DEBUG_PRINTF            
                printf("Current USB Device is Mass Storage Device\n");
#endif
                   status = Disk_Test( &UDisk );                                /* U�̲��������� */    
            }
        }
        /***********************************************************************/
        /* ���Խ���,��ѯ�ȴ��豸�γ� */
        while( 1 )
        {
            if( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) == USB_INT_DISCONNECT )
            {
                printf("USB Device Out\n");
                USBHOST_EHCI_Init( );
                USBHOST_DevicePara_Init( &UDisk.Device );                       /* USB������ز�����ʼ�� */
                USBHOST_Structure_DeInit( );                                    /* USB�������ݽṹ��ʼ�� */
#ifdef  MY_DEBUG_PRINTF    
                printf("USB Device Out\n");
#endif
                break;
            }
        }    
    }
}

/*********************************** endfile **********************************/