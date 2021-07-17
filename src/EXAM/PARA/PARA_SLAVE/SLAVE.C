/********************************** (C) COPYRIGHT *******************************
* File Name          : SLAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 PARA_SLAVE DEMO
*                      (1)��CH563 Examples by KEIL;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)������������ʾ���ӳ���.
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* �������� */
UINT16    my_buffer[ 0x2000 ];

/* ����һ��LED���ڼ����ʾ����Ľ���,�͵�ƽLED�� */
#define LED                     1<<3

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }         /* LED �ߵ�ƽΪ������� */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                            /* LED �͵�ƽ����LED��ʾ */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                            /* LED �ߵ�ƽ�ر�LED��ʾ */

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
* Function Name  : ParaSlvQryTest
* Description    : ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/* 
    Ӳ����������:
    D0-D7    PD0-PD7
    INT        PA9        
    A0        PA10
    CS        PA11
    RD        PA20
    WR        PA21
    */

void ParaSlvQryTest( void )
{
    UINT8 dat;
    
    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */
    R8_SLV_CONFIG = (RB_SLV_ENABLE | RB_SLV_IE_CMD);                            /* ʹ�ܴ�ģʽ���ڣ���ֹ�����д�ж� */ 
    R8_SAFE_ACCESS_SIG = 0x00 ;                                                 /* lock, to prevent unexpected writing */

    R32_PA_DIR &= ~SLVA ;                                                       /* A0���� */
    R32_PA_DIR &= ~SLVCS ;                                                      /* CS���� */
    R32_PA_DIR &= ~PRD ;                                                        /* RD���� */
    R32_PA_DIR &= ~PWR ;                                                        /* WR���� */

    R32_PA_DIR |= SLVI;                                                         /* INT��� */
    R32_PA_OUT |= SLVI;                                                         /* INTĬ��Ϊ�� */

    while( (R8_INT_FLAG_SLV& RB_IF_SLV_CMD) == 0 );                             /* �ȴ����� */
    R8_INT_FLAG_SLV = 0xff;
    PRINT("$$");
    dat = R8_INT_SLV_DIN;
    PRINT("%02x\n",(UINT16)dat);
    if( dat == 0xaa ){
        R8_SLV_STATUS =  0x01;                                                  /* �ж�״̬ */
        R32_PA_CLR |= SLVI;                                                     /* ��INT������ */

        while( ( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) ==0 );
        R32_PA_OUT |= SLVI;                                                     /* ��INT������ */
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;
        R8_SLV_DOUT = 0xaa;
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* �ȴ����ж� */ 
        PRINT("R8_INT_FLAG_SLV=%02x\n",(UINT16)R8_INT_FLAG_SLV);
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* ���жϱ�־ */
        PRINT("!!");
    }
    if( dat == 0x55 ){
        R8_SLV_STATUS = 0x80 | 0x02;                                            /* �ж�״̬ */
        R32_PA_CLR |= SLVI;                                                     /* ��INT������ */
        R8_INT_FLAG_SLV = 0xff;                                                 /* ���ж� */
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );
        PRINT("@@");
        R8_SLV_DOUT = 0xaa;
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* �ȴ����ж� */
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* ���жϱ�־ */ 
    }
#if 0
    while( !( R8_INT_FLAG_SLV & RB_IF_SLV_CMD ) );                              /* �ȴ������ж� */
    PRINT( "IF CMD: 0x%x\n", ( UINT16 )R8_INT_SLV_DIN );
    if( R8_INT_SLV_DIN == 0x55 ){
        R8_SLV_STATUS = 0x80 | 0x01;                                            /* �ж�״̬ */
        PRINT( "    Out Int Sts: 0x%x\n", ( UINT16 )R8_SLV_STATUS );
        PRINT( "    Wait Master Get Int Sts\n" ); 
        R32_PA_OUT &= ~( 1<<9 );                                                /* �����ж��ź� */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* �ȴ��ж�״̬��ȡ�� */
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* ���жϱ�־ */ 
        R32_PA_OUT |= 1<<9;                                                     /* �����ж��ź� */ 

        R8_SLV_DOUT = 0xAA;                                                     /* ������ݣ�0xAA */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* �ȴ����ж� */ 
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* ���жϱ�־ */ 
        PRINT( "    Ok\n" );
    }
    while( !( R8_INT_FLAG_SLV & RB_IF_SLV_CMD ) );                              /* �ȴ������ж� */ 
    PRINT( "IF CMD: 0x%x\n", ( UINT16 )R8_INT_SLV_DIN );
    if( R8_INT_SLV_DIN == 0xAA ){
        R8_SLV_STATUS = 0x80 | 0x02;                                            /* �ж�״̬ */
        PRINT( "    Out Int Sts: 0x%x\n", ( UINT16 )R8_SLV_STATUS );
        PRINT( "    Wait Master Get Int Sts\n" );

        R32_PA_OUT &= ~( 1<<9 );                                                /* �����ж��ź� */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* �ȴ��ж�״̬��ȡ�� */ 
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* ���жϱ�־ */ 
        R32_PA_OUT |= 1<<9;                                                     /* �����ж��ź� */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_WR ) );                           /* �ȴ�д�ж� */ 
        dat = R8_INT_SLV_DIN;
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* ���жϱ�־ */ 
        PRINT( "    Ok, dat=0x%x\n", ( UINT16 )dat );
    }
#endif
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
    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */
    PRINT( "Start Para Slave Test\xd\xa" );
    ParaSlvQryTest( );                                                          /* �������ڲ��� */
    while( 1 );
}

/*********************************** endfile **********************************/