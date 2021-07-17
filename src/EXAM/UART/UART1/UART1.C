/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X UART1 DEMO
*                      (1)��CH56X Examples by KEIL3_V3.53;
*                      (2)������1���ͺͽ������ݣ�ʵ��ͨ��FIFO���ͺͽ�������.
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* �������� */
UINT8 SEND_STRING1[ ] = { "IRQ sucess!\n" };
UINT8 buf[ 50 ]; 
UINT8 rcvbuf[ 50 ];

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
    x += 5;                                                                        /* �������� */
    x /= 10;
    R8_UART1_LCR = RB_LCR_DLAB;                                                 /* DLABλ��1 */
    R8_UART1_DIV = 1;                                                           /* Ԥ��Ƶ */
    R8_UART1_DLM = x>>8;
    R8_UART1_DLL = x&0xff;

    R8_UART1_LCR = RB_LCR_WORD_SZ ;                                             /* �����ֽڳ���Ϊ8 */
    R8_UART1_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN ;
                                                                                /* ����FIFO������Ϊ28���巢�ͺͽ���FIFO��FIFOʹ�� */
    R8_UART1_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD1|TXD1;                                                    /* RXD1 schmitt input, TXD1 slow rate */
    R32_PB_PU  |= RXD1;                                                         /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD1;                                                         /* TXD1 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : ͨ��������������Ϣ
* Input          : c  -writes the character specified by c 
*                  *f -the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART1_THR = c;                                                           /* �������� */
    while( ( R8_UART1_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* �ȴ����ݷ��� */
    return( c );
}

/*******************************************************************************
* Function Name  : UART1_SendByte
* Description    : ����1����һ�ֽ��ӳ���
* Input          : dat -����������
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
* Description    : ����1����һ�ַ����ӳ���
* Input          : str--�������ַ�������
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
        
    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ) {
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
* Description    : ����FIFO������1���Ͷ��ֽڳ��� 
* Input          : *Data -������������ָ��
*                  Num   -�������ݳ���
* Output         : None
* Return         : None 
*******************************************************************************/

void  Seril1Send( UINT8 *Data, UINT8 Num )                        
{
    do{
        while( ( R8_UART1_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                    /* �ȴ����ݷ������ */ 
        R8_UART1_THR  =*Data++;  
    }while( --Num );
}

/*******************************************************************************
* Function Name  : Seril1Rcv
* Description    : ����FIFO,����1���ն��ֽ��ӳ��� 
* Input          : *buf -���������ݴ滺����
* Output         : None
* Return         : RcvNum -���յ����ݳ��� 
*******************************************************************************/

UINT8  Seril1Rcv( UINT8 *buf )    
{
    UINT8 RcvNum = 0;

    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ) {
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* �ȴ�����׼���� */
        do{
            *buf++ = R8_UART1_RBR;                                              /* �ӽ��ջ���Ĵ����������� */ 
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
* Description    : ����FIFO,һ�����32�ֽڣ�����1���Ͷ��ֽ��ӳ���
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
        if( Num <= UART1_FIFO_SIZE){                                            /* FIFO����Ϊ32�����ݳ��Ȳ���32�ֽڣ�һ�η������ */
            do{
                R8_UART1_THR = *Data++;
            }while(--Num) ;
            break;
        }
        else{                                                                   /* FIFO����Ϊ32�����ݳ��ȳ���32�ֽڣ��ֶ�η��ͣ�һ��32�ֽ� */
            for(i=0;i<UART1_FIFO_SIZE;i++){
                R8_UART1_THR = *Data++;
            }
            Num -= UART1_FIFO_SIZE;
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
    UINT8 i,RcvNum;

    LED_OUT_INIT( );                                                                 
    LED_OUT_ACT( );                                                             /* ������LED��һ����ʾ���� */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    Uart1_Init( 115200 );                                                       /* ����1��ʼ�� */ 
    for( i = 0; i < 50; i++ ){
        buf[ i ] = i;
    }
    UART1_SendByte(0xAA);                                                       /* ����1����1�ֽ� */
    UART1_SendByte(0xAA);                                                       /* ����1����1�ֽ� */
    UART1_SendByte(0xAA);                                                       /* ����1����1�ֽ� */
    UART1Send_FIFO( buf, 50 );                                                  /* ����FIFO������50�ֽ����� */
    while(1){
        RcvNum  = Seril1Rcv( rcvbuf );                                          /* �ȴ��������ݣ���ͨ������1���ͳ�ȥ */
        Seril1Send( rcvbuf, RcvNum );
    }
}

/*********************************** endfile **********************************/