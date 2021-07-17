/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 AUDIO Demo Test
*                      (1)��CH563 USB Device Examples by KEIL;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)���ó������жϷ�ʽ����;
*                      (4)��ģ��USB����.���CH563��������Բ�������
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "CH563USBSFR.H"                                                        /* оƬUSB�Ĵ������ͷ�ļ� */
#include "USB_DEVICE.H"                                                         /* USB�豸�������ͷ�ļ� */
#include "SYSFREQ.H"                                                            /* ϵͳ�������ͷ�ļ� */    
#include "PRINTF.H"                                                             /* ���ڴ�ӡ�������ͷ�ļ� */
#include "AUDIO.H"

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void IRQ_Handler( void )
{
    UINT8 i;

    if( R8_INT_FLAG_1 & RB_IF_USB )                                             /* USB�ж� */
    {
        if( USB_GL_INT_STATUS & RB_DEV_INT )
        {
            USBDev_IRQHandler( );                                               /* USB�豸�жϴ������� */
        }
    }
    if(R8_INT_FLAG_0 & RB_IF_TMR0)                                              /* �ж��ǲ��Ƕ�ʱ��0��ص��ж� */
    {
        i = R8_TMR0_INT_FLAG;
           if( i&RB_TMR_IF_DMA_END ){
            R8_TMR0_CTRL_MOD &= ~(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);               /* �رն�ʱ�� */
            R16_TMR0_DMA_BEG = (UINT32)&pwm.DatBuf[0];                          /* ������׵�ַ��DMA��ʼ��������ַ */
            R8_TMR0_INT_FLAG = RB_TMR_IF_DMA_END;                               /* �����Ӧ�жϱ�־ */
            R8_INT_FLAG_0 |= RB_IF_TMR0;                                        /* �����Ӧ�жϱ�־ */
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
    while(1);
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
* Function Name  : Tim0_IRQ_Init
* Description    : PWM��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Tim0_IRQ_Init( )
{
    pwm.EndCount = 100000000/8000/8;
    printf("end count=%8d\n",pwm.EndCount);
    R8_TMR0_INT_FLAG=0xff;                                                      /* ������е��жϱ�־ */                                           
    R8_INT_EN_IRQ_0=RB_IE_IRQ_TMR0;                                             /* ֻ������ʱ��0����жϲ��� */
    R8_TMR0_INTER_EN |= RB_TMR_IE_DMA_END;                                      /* ����DMA�����ж� */
    R8_INT_EN_IRQ_GLOB= RB_IE_IRQ_GLOB;                                         /* ֻ����IRQȫ���ж� */
    R8_TMR0_CTRL_DMA= (RB_TMR_DMA_ENABLE)|(RB_TMR_DMA_LOOP&0);                  /* ����DMA����ֹDMAѭ�� */
    R32_TMR0_CNT_END = pwm.EndCount;                                            /* ����PWM���� */
    R8_TMR0_CTRL_MOD = 0x00;                                                    /* ��0 */
/* 16 */
//    R8_TMR0_CTRL_MOD |= (1<<7)|(1<<6);                                        /* timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16 */
/* 8 */
    R8_TMR0_CTRL_MOD |= (1<<7);                                                 /* timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16 */
/* 4 */
//    R8_TMR0_CTRL_MOD |= (1<<6);                                               /* timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16 */
    R32_PB_DIR |= PWM0;                                                         /* PB0Ϊ��ʱ��0������ţ�����Ϊ��� */
}
void Star_PWM_DMA( )
{
    R16_TMR0_DMA_BEG = (UINT32)&pwm.DatBuf[0];                                  /* ������׵�ַ��DMA��ʼ��������ַ */
    R16_TMR0_DMA_END = (UINT32)&pwm.DatBuf[pwm.DatLen+2];                       /* ����Ľ�����ַ��DMA������������ַ */
    R8_TMR0_CTRL_MOD|=(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);                          /* ������ʱ�� */
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
    Tim0_IRQ_Init( );
    USBDev_ModeSet( );                                                          /* USB����Ϊ�豸ģʽ */
    USBDev_Init( );                                                             /* USB�豸��ʼ�� */
    USBDev_UsbInt_Enable( );                                                    /* USB�豸������ж�ʹ�� */
    while( 1 )
    {
        /* ���в��������ж��д��� */
    }
}

/*********************************** endfile **********************************/