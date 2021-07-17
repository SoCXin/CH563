/********************************** (C) COPYRIGHT *******************************
* File Name          : ISP_DEMO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2016/12/15
* Description        : CH56X ISP DEMO
*                      (1)��CH56X Examples by KEIL;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)����������ʾ����ISP�������أ�д��ɹ���оƬ�����ϵ缴��ͨ��ISP��������
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>

#ifndef    DEBUG
#define    DEBUG  1
#endif

#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"
                              

/* ����һ��LED���ڼ����ʾ����Ľ���,�͵�ƽLED�� */
#define LED_OUT_INIT( LED )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }       /* LED �ߵ�ƽΪ������� */
#define LED_OUT_ACT( LED )      { R32_PB_CLR |= LED; }                          /* LED �͵�ƽ����LED��ʾ */
#define LED_OUT_INACT( LED )    { R32_PB_OUT |= LED; }                          /* LED �ߵ�ƽ�ر�LED��ʾ */

__align(4) UINT8  TestBuf[1024*4];                                              //�û��������ڵ���ISP���õ���غ���ʱ����һ����ʱ������

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϺ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    while(1);
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

    x = 10 * FREQ_SYS/ 8 / 115200;                                              /* 115200bps */
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
    UINT8 i,my_buffer[8];

    R32_PB_DIR &=  ~(1<<7);                                                     // PB7��������                                
    R32_PB_PU  |=  (1<<7);                   
    mInitSTDIO();                                                               
    PRINT("Edit Date and Time is: "__DATE__" , " __TIME__"\n");
    GET_UNIQUE_ID( my_buffer );
    PRINT("chip id: ");
    for(i=0;i<8;i++) PRINT("%-2x ",my_buffer[i]);
    PRINT("\n");

    while(1){
        if( (R32_PB_PIN & (1<<7) )== 0  ){                                      // �����������õĸ�������
            Delay_ms(50);
            if( (R32_PB_PIN & (1<<7) )== 0  ){
                while( (R32_PB_PIN & (1<<7) )== 0  );
                PRINT("ISP TEST\n");
                i = CH56X_EnterIspSet( TestBuf );
                if(i == 0)
                {
                    printf("entry isp success\n");
                                                                                //���ڿ��Խ�оƬ���磬�������ϵ缰����ISP
                }
            }
        }
    }
}

/*********************************** endfile **********************************/