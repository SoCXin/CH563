/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1_INT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X UART1_INT DEMO
*                      (1)��CH56X Examples by KEIL;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)������1�жϵ�ʹ�ü��жϵĴ������Լ��ֽ��շ��ӳ���FIFO�շ��ӳ���.
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* �������� */
void UART1_SendStr( UINT8 *str );  
void  Seril1Send( UINT8 *Data, UINT8 Num );
UINT8  Seril1Rcv( UINT8 *buf );
void UART1_SendByte( UINT8 dat ) ;

UINT8 SEND_STRING[ ] = { "l am uart1! \n" };
UINT8 SEND_STRING1[ ] = { "Modem Change!\n" };
UINT8  MyBuf[ 5000 ]; 

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
      
    UINT8 RcvNum = 0;
    UINT8 ReadDat = 0;

     
    if (R8_INT_FLAG_0&RB_IF_UART1){
        switch( R8_UART1_IIR & RB_IIR_INT_MASK ){
            case UART_II_MODEM_CHG :                                            /* Modem �źű仯 */
                UART1_SendStr(SEND_STRING1);
                if ( R8_UART1_MSR == 0x30 ){                                    /* Modem �źŷ������ݱ仯 */ 
                    UART1_SendStr( SEND_STRING1 );
                }
                else if ( R8_UART1_MSR == 0x11 ){                               /* Modem �źŽ������ݱ仯 */ 
                    R8_UART1_THR= 0xAA;
                }
                else if ( R8_UART1_MSR == 0x22 ){                               /* Modem �źŽ������ݱ仯 */ 
                    RcvNum = Seril1Rcv( MyBuf );
                    Seril1Send( MyBuf, RcvNum );
                }
                break;
            case UART_II_NO_INTER :                                             /* û���ж� */ 
                break;
            case UART_II_THR_EMPTY:                                             /* ���ͱ��ּĴ������ж� */
                break;
            case UART_II_RECV_RDY:                                              /* ���ڽ��տ��������ж� */
                RcvNum = Seril1Rcv( MyBuf );
                Seril1Send( MyBuf, RcvNum );
                break;
            case UART_II_LINE_STAT:                                             /* ������·״̬�ж� */
                ReadDat = R8_UART1_LSR;
                PRINT("ReadDat = %x\n",ReadDat);
                break;
            case UART_II_RECV_TOUT:                                             /* �������ݳ�ʱ�ж� */
                RcvNum = Seril1Rcv( MyBuf );
                Seril1Send( MyBuf, RcvNum );
                break;
            default:                                                            /* �����ܷ������ж� */ 
                break;
        }
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
* Function Name  : Uart1_Init
* Description    : ����1��ʼ��
* Input          : baud-���ڲ����ʣ����Ϊ��Ƶ1/8
* Output         : None
* Return         : None
*******************************************************************************/

void Uart1_Init( UINT32 baud )
{
    UINT32 x;

    x = 10 * FREQ_SYS/ 8 / baud;                                                /* 115200bps */
    x += 5;                                                                     /* �������� */
    x /= 10;
    R8_UART1_LCR = RB_LCR_DLAB;                                                 /* DLABλ��1 */
    R8_UART1_DIV = 1;                                                           /* Ԥ��Ƶ */
    R8_UART1_DLM = x>>8;
    R8_UART1_DLL = x&0xff;

    R8_UART1_LCR = RB_LCR_WORD_SZ ;                                             /* �����ֽڳ���Ϊ8    */
    R8_UART1_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* ����FIFO������Ϊ28���巢�ͺͽ���FIFO��FIFOʹ�� */
    R8_UART1_IER = RB_IER_TXD_EN | RB_IER_LINE_STAT |RB_IER_THR_EMPTY | 
                   RB_IER_RECV_RDY  ;                                           /* TXD enable */
    R8_UART1_MCR = RB_MCR_OUT2;                                                
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_UART1;                                         /* �����ж����ʹ�� */
    R32_PB_SMT |= RXD1|TXD1;                                                    /* RXD1 schmitt input, TXD1 slow rate */
    R32_PB_PD  &= ~ RXD1;                                                       /* disable pulldown for RXD1, keep pullup */
    R32_PB_DIR |= TXD1;                                                         /* TXD1 output enable */
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
* Input          : c  -writes the character specified by c 
*                  *f -the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int    fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* �������� */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* �ȴ����ݷ��� */
    return( c );
}

/*******************************************************************************
* Function Name  : UART1_SendByte
* Description    : ����1����һ�ֽ��ӳ���
* Input          : dat -Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/

void UART1_SendByte( UINT8 dat )   
{        
    R8_UART1_THR  = dat;
    while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                         /* �ȴ����ݷ��� */       
}

/*******************************************************************************
* Function Name  : UART1_SendStr
* Description    : ����1�����ַ����ӳ���
* Input          : *str -Ҫ�����ַ�����ָ��
* Output         : None
* Return         : None
*******************************************************************************/

void UART1_SendStr( UINT8 *str )   
{
    while( 1 ){
        if( *str == '\0' ) break;
        UART1_SendByte( *str++ );
    }
}

/*******************************************************************************
* Function Name  : UART1_RcvByte
* Description    : ����1����һ�ֽ��ӳ���
* Input          : None
* Output         : None
* Return         : Rcvdat -���յ�������
*******************************************************************************/

UINT8 UART1_RcvByte( void )    
{
    UINT8 Rcvdat = 0;
    
    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ){
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* �ȴ�����׼���� */ 
        Rcvdat = R8_UART1_RBR;                                                  /* �ӽ��ջ���Ĵ����������� */ 
    }
    else{
        R8_UART1_RBR;                                                           /* �д������ */
    }
    return( Rcvdat );
}

/*******************************************************************************
* Function Name  : Seril1Send
* Description    : ����1���Ͷ��ֽ��ӳ���
* Input          : *Data -������������ָ��
*                  Num   -�������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/

void  Seril1Send( UINT8 *Data, UINT8 Num )                        
{
    do{
        while( ( R8_UART1_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                    /* �ȴ����ݷ������ */ 
        R8_UART1_THR  = *Data++;  
    }while( --Num );
}

/*******************************************************************************
* Function Name  : Seril1Rcv
* Description    : ����FIFO,����1���ն��ֽ��ӳ���
* Input          : *pbuf -���ջ�����ָ��
* Output         : None
* Return         : RcvNum -���յ����ݵ����ݳ���
*******************************************************************************/

UINT8  Seril1Rcv( UINT8 *pbuf )    
{
    UINT8 RcvNum = 0;

    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ){
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* �ȴ�����׼���� */ 
        do{
            *pbuf++ = R8_UART1_RBR;                                             /* �ӽ��ջ���Ĵ����������� */ 
            RcvNum++;
        }while( ( R8_UART1_LSR & RB_LSR_DATA_RDY   ) == 0x01 );
    }
    else{
        R8_UART1_RBR;
    }
    return( RcvNum );
}

/*******************************************************************************
* Function Name  : UART1Send_FIFO
* Description    : ����FIFO,һ�����32�ֽڣ�CH432����1���Ͷ��ֽ��ӳ���
* Input          : *Data -������������ָ��
*                  Num   -�������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/

void UART1Send_FIFO( UINT8 *Data, UINT8 Num ) 
{
    int i;

    while( 1 ){
        while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                     /* �ȴ����ݷ�����ϣ�THR,TSRȫ�� */
        if( Num <= 32){                                                         /* FIFO����Ϊ32�����ݳ��Ȳ���32�ֽڣ�һ�η������ */
            do{
                R8_UART1_THR=*Data++;
            }while(--Num) ;
            break;
        }
        else{                                                                   /* FIFO����Ϊ32�����ݳ��ȳ���32�ֽڣ��ֶ�η��ͣ�һ��32�ֽ� */
            for(i=0;i<32;i++){
                R8_UART1_THR=*Data++;
            }
            Num -= 32;
        }
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
    Uart1_Init( 115200 );
    memset(MyBuf,0,sizeof(MyBuf));
    PRINT("start:\n");
    R8_INT_EN_IRQ_GLOB  = RB_IE_IRQ_GLOB;                                       /* ȫ���ж�ʹ�� */
    while ( 1 ) ;
}

/*********************************** endfile **********************************/