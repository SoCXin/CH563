/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 KEY Demo Test
*                      (1)��CH563 USB Device Examples by KEIL;
*                      (2)������0��������Ϣ,115200bps;
*                      (3)���ó������жϷ�ʽ����;
*                      (4)���ó�����Ҫ���ܣ�ģ�����.
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "CH563USBSFR.H"                                                        /* оƬUSB�Ĵ������ͷ�ļ� */
#include "USB_DEVICE.H"                                                         /* USB�豸�������ͷ�ļ� */
#include "SYSFREQ.H"                                                            /* ϵͳ�������ͷ�ļ� */
#include "KEYBOARD.H"                                                          
#include "PRINTF.H"                                                             /* ���ڴ�ӡ�������ͷ�ļ� */

volatile UINT32 MyTimes = 0;
/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void IRQ_Handler( void )
{
    if( R8_INT_FLAG_1 & RB_IF_USB )                                             /* USB�ж� */
    {
        if( USB_GL_INT_STATUS & RB_DEV_INT )                                    /* USB�豸�ж� */
        {
            USBDev_IRQHandler( );                                               /* USB�豸�жϴ������� */
        }
    }
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
    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */
#if  MY_DEBUG_PRINTF
    printf("CH563 USB Device Test\n");
#endif
    USBDev_ModeSet( );                                                          /* USB����Ϊ�豸ģʽ */
    USBDev_Init( );                                                             /* USB�豸��ʼ�� */
    USBDev_UsbInt_Enable( );                                                    /* USB�豸������ж�ʹ�� */
    while(KEY_SendFlag==0);
    Delay_ms(3000);
    while( 1 )
    {
        if(KEY_SendFlag)
        {
            if(MyTimes<26)
            {
                KEY_SendKey(MyTimes);
#if  MY_DEBUG_PRINTF
                printf("%d",MyTimes++);
#endif
                KEY_SendFlag = 0;
                while(KEY_SendFlag==0);
                KEY_SendBlank(0x28);
                while(KEY_SendFlag==0);
#if  MY_DEBUG_PRINTF
                printf("delay over!\n");
#endif
            }
        }
    }
}

/*********************************** endfile **********************************/