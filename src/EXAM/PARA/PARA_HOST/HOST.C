/********************************** (C) COPYRIGHT *******************************
* File Name          : HOST.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : CH563 PARA_HOST DEMO
*                      (1)��CH563 Examples by KEIL3_V3.53;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)������������ʾ����,��Ҫʵ�ֶ��ⲿRAM�Ĳ���, ���������ڵ�HOLDλ�Լ����ݽ��������޸ĵ��� 
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
 
/******************************************************************************/
/* �������� */
UINT16 my_buffer[ 0x2000 ];
PUINT8V pXbusPt;

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
* Function Name  : XbusTest
* Description    : ���ڲ���ram
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void XbusTest( void )
{
    UINT8   mFstDat, mTstDat;
    UINT8   mHoldClk;                                                           /* hold clk */
    UINT32    i, j;

    PRINT( "Xbus Test\n" );
    R32_PA_DIR = (1<<21)|( 1<<20 )|0x7FFF;                                      /* �õ�ַ��WR��RD�ź���� */ 
    mFstDat = 0x55;
    PRINT( "setup clk=1:\n" );
    for( mHoldClk = 0; mHoldClk <= RB_XBUS_HOLD; mHoldClk ++ ){
        R8_SAFE_ACCESS_SIG = 0x57;                                              /* unlock step 1 */
        R8_SAFE_ACCESS_SIG = 0xA8;                                              /* unlock step 2 */ 

        R8_XBUS_CONFIG = RB_XBUS_ENABLE | RB_XBUS_ADDR_OE;                      /* �ⲿ����ʹ�� */ 
        R8_XBUS_SETUP_HOLD = mHoldClk;                                          /* 1 setup clocks */ 

        R8_SAFE_ACCESS_SIG = 0x00;                                              /* lock, to prevent unexpected writing */

        j = 1024 * 32;                                                          /* �ⲿ32K RAM */ 
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* �ⲿ���߻�ַ */ 
        mTstDat = mFstDat;

        PRINT( "    mHoldClk=0x%x, FstDat=0x%x\n", ( UINT16 )mHoldClk, ( UINT16 )mFstDat );
        PRINT( "        write: " );
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x ",( UINT16 )mTstDat );                      /* ��ʾǰ16������ */ 
            *pXbusPt = mTstDat;
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n" );
        PRINT( "        read:  " );
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* �ⲿ���߻�ַ */ 
        mTstDat = mFstDat;                                                      /* ���������� */
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x  ", ( UINT16 )*pXbusPt );
            if( *pXbusPt != mTstDat ){
                PRINT( "        err: i=0x%lX, mTstDat=%x\n", ( UINT32 )i, ( UINT16 )mTstDat );
                break;
            }
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n        over\n" );
        mFstDat ^= 0xFF;
    }
    PRINT( "setup clk=2:\n" );
    for( mHoldClk = 0; mHoldClk <= RB_XBUS_HOLD; mHoldClk ++ ){
        R8_SAFE_ACCESS_SIG = 0x57;                                              /* unlock step 1 */ 
        R8_SAFE_ACCESS_SIG = 0xA8;                                              /* unlock step 2 */

        R8_XBUS_CONFIG = RB_XBUS_ENABLE | RB_XBUS_ADDR_OE;                      /* �ⲿ����ʹ�� */
        R8_XBUS_SETUP_HOLD = mHoldClk;                                          /* 1 setup clocks */ 

        R8_SAFE_ACCESS_SIG = 0x00;                                              /* lock, to prevent unexpected writing */

        j = 1024 * 32;                                                          /* �ⲿ32K RAM */
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* �ⲿ���߻�ַ */ 
        mTstDat = mFstDat;                                                      /* ���������� */ 
        PRINT( "    mHoldClk=0x%x, FstDat=0x%x\n", ( UINT16 )mHoldClk, ( UINT16 )mFstDat );
        PRINT( "        write: " );
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x ",( UINT16 )mTstDat );                      /* ��ʾǰ16������ */ 
            *pXbusPt = mTstDat;
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n" );
        PRINT( "        read:  " );
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* �ⲿ���߻�ַ */ 
        mTstDat = mFstDat;                                                      /* ���������� */ 
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x  ", ( UINT16 )*pXbusPt );
            if( *pXbusPt != mTstDat ){
                PRINT( "        err: i=0x%lX, mTstDat=%x\n", ( UINT32 )i, ( UINT16 )mTstDat );
                break;
            }
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n        over\n" );
        mFstDat ^= 0xFF;
    }
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
    PRINT( "Start Para Host Test\n" );
    XbusTest( );                                                                /* ����������ʾ���� */
    while( 1 );
}

/*********************************** endfile **********************************/