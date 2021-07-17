/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO_INT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X GPIO_INT DEMO
*                      (1)��CH56X Examples by KEIL;
*                      (2)������0��������Ϣ,115200bps;
*                      (3)����������Ҫ��GPIO��Ϊ�ⲿ�жϵ����ã������жϷ����ӳ����ж�ȡ�ж�״̬�����жϱ�־.
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/* ����һ��LED���ڼ����ʾ����Ľ���,�͵�ƽLED�� */
#define LED                  1<<3

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }          /* LED �ߵ�ƽΪ������� */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                             /* LED �͵�ƽ����LED��ʾ */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                             /* LED �ߵ�ƽ�ر�LED��ʾ */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϺ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void IRQ_Handler( void )
{
    PRINT("R32_INT_FLAG=%8lx\n",R32_INT_FLAG);
    if((R32_INT_FLAG>>8) & RB_IF_PB){                                           /* PB���ж� */
        PRINT("%8lX \n", R32_INT_STATUS_PB&0x000000ff );                        /* �鿴�жϱ�־ */
        R32_INT_STATUS_PB = 0xfffff;                                            /* �жϱ�־д1���� */
    }
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ�жϺ���
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
* Function Name  : IRQ_InitPB
* Description    : �ж����ó�ʼ�� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void IRQ_InitPB( void )
{
    R32_PB_PU  |= 0x000000ff;                                                   /* GPIO B�������ã���1��ʾ���� */ 
    R32_PB_DIR &= ~0x000000ff;                                                  /* GPIO B��������Ϊ���� , direction: 0=in, 1=out */

    R32_INT_ENABLE_PB |= 0x000000ff;                                            /* GPIO B���λ�ж�ʹ�� �� 1-ʹ�ܣ�0-��ֹ */
    R32_INT_MODE_PB   |= 0x000000ff;                                            /* GPIO B���жϷ�ʽ ��1-�����ж�,0-��ƽ�ж� */      
    R32_INT_POLAR_PB  &= ~0x000000ff;                                           /* GPIO B���жϼ��� ��1-�������ж�/�ߵ�ƽ��0-�½����ж�/�͵�ƽ */      
    
    R32_INT_STATUS_PB  = 0xfffff;                                               /* �жϱ�־д1���� */
    R8_INT_EN_IRQ_1   |= RB_IE_IRQ_PB;                                          /* GPIO B���ж�ʹ�� */ 
    R8_INT_EN_IRQ_GLOB|= RB_IE_IRQ_GLOB;                                        /* ȫ���ж�ʹ�� */     
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
    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* ������LED��һ����ʾ���� */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* ����0��ʼ�� */
    PRINT("GPIO INT DEMO.....\n");
    IRQ_InitPB( );                                                              /* �ж����ó�ʼ�� */
    while(1);
}

/*********************************** endfile **********************************/