/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC_Query.C
* Author             : WCH
* Version            : V1.0
* Date               : 2015/08/25
* Description        : CH56X ADC_QUERY DEMO
*                      (1)��CH56X Examples by KEIL;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)��ʵ��ADC��������.AD���̲���DMA����,Ĭ��ѡ��ͨ��2����ADCת��.
*******************************************************************************/

#define           DEBUG            1

/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* �������� */
UINT16 ADC_buf[1024]; 

/* ����һ��LED���ڼ����ʾ����Ľ���,�͵�ƽLED�� */
#define LED                     1<<3

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
    R32_PB_SMT  |= RXD0|TXD0;                                                   /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD   &= ~ RXD0;                                                      /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR  |= TXD0;                                                        /* TXD0 output enable */
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
* Function Name  : ADC_INIT
* Description    : ADC��ʼ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_INIT( void )
{
    R8_ADC_CTRL_MOD = 0;
    R8_ADC_CTRL_MOD |= RB_ADC_POWER_ON;                                         /* ����ADCʹ�� */
    R8_ADC_CLOCK_DIV = 0x40;                                                    /* ADC����ʱ���ź� */
    R8_ADC_CTRL_MOD |= (RB_ADC_SAMPLE_WID | RB_ADC_CYCLE_CLK );                 /* ���������Լ������Զ�ѭ������ */
    R8_ADC_CTRL_MOD |= 0x20;                                                    /* ѡ��ͨ��2 */    
    Delay_us(100);
    /* ����ΪDMA���� */
    R16_ADC_DMA_BEG = (UINT32)&ADC_buf[0];
    R16_ADC_DMA_END = (UINT32)&ADC_buf[1024];
    R8_ADC_CTRL_DMA |= (RB_ADC_DMA_ENABLE|RB_ADC_DMA_BURST|RB_ADC_MAN_SAMPLE ); /* ����DMA,��ʼ���� */
    R8_ADC_CTRL_DMA &= ~RB_ADC_CHAN_OE;                                         /* �����ֹADCͨ������ADCS������������λ */
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
    UINT32 i,sum;

    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* ������LED��һ����ʾ���� */ 
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */ 
    PRINT( "ADC Demo...\n" );
    ADC_INIT(  );
    R32_PB_DIR &=  ~(1<<7);                                                        /* PB7�������� */                                
    R32_PB_PU  |=  (1<<7);                   
    while(1){
        if( R16_ADC_DMA_NOW == R16_ADC_DMA_END ){                               /* �ȴ�ADC��DMA���� */
            sum = 0;
            for( i=0;i!=1024;i++ ){                                             /* ��ʾADC�ɼ�����,ADC�ɼ�����λ��10λ */
                sum +=ADC_buf[i];
                if(i%16 == 0) printf("\n");
                PRINT("%04x ",(UINT16)ADC_buf[i]);
            } 
            PRINT("\naverage:%-08x\n",(UINT32)(sum/1024));
            R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_ENABLE ;                             /* ֹͣ���� */
            R16_ADC_DMA_BEG  = (UINT32)ADC_buf;
        }
        if( (R32_PB_PIN & (1<<7) )== 0 ){                                                /* �л�ͨ�� */
            R8_ADC_CTRL_MOD &= ~RB_ADC_CHAN_MOD;                                        /* ѡ��ͨ��1 */    
            R8_ADC_CTRL_MOD |= 0x10;                                                       
            while( R8_ADC_FIFO_COUNT ){                                                    /* ��Ϊ���FIFO�ڵ����� */
                R16_ADC_FIFO;
            }
            R8_ADC_CTRL_DMA |= (RB_ADC_DMA_ENABLE|RB_ADC_DMA_BURST|RB_ADC_MAN_SAMPLE ); /* ����DMA,��ʼ���� */
            R8_ADC_CTRL_DMA &= ~RB_ADC_CHAN_OE;                                         /* �����ֹADCͨ������ADCS������������λ */
        }
    }                                                                   
}

/*********************************** endfile **********************************/ 