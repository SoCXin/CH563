/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X UART1 DEMO
*                      (1)、CH56X Examples by KEIL3_V3.53;
*                      (2)、串口1发送和接收数据，实现通过FIFO发送和接收数据.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
UINT8 SEND_STRING1[ ] = { "IRQ sucess!\n" };
UINT8 buf[ 50 ]; 
UINT8 rcvbuf[ 50 ];

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED                     1<<3

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }         /* LED 高电平为输出方向 */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                            /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                            /* LED 高电平关闭LED显示 */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断函数
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
* Description    : FIQ中断函数
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
* Description    : 串口1初始化
* Input          : baud-串口波特率，最高为主频1/8
* Output         : None
* Return         : None
*******************************************************************************/

void Uart1_Init( UINT32 baud )
{
    UINT32 x;

    x = 10 * FREQ_SYS/ 8 / baud;                                                /* 115200bps */
    x += 5;                                                                        /* 四舍五入 */
    x /= 10;
    R8_UART1_LCR = RB_LCR_DLAB;                                                 /* DLAB位置1 */
    R8_UART1_DIV = 1;                                                           /* 预分频 */
    R8_UART1_DLM = x>>8;
    R8_UART1_DLL = x&0xff;

    R8_UART1_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART1_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN ;
                                                                                /* 设置FIFO触发点为28，清发送和接收FIFO，FIFO使能 */
    R8_UART1_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD1|TXD1;                                                    /* RXD1 schmitt input, TXD1 slow rate */
    R32_PB_PU  |= RXD1;                                                         /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD1;                                                         /* TXD1 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : 通过串口输出监控信息
* Input          : c  -writes the character specified by c 
*                  *f -the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART1_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART1_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}

/*******************************************************************************
* Function Name  : UART1_SendByte
* Description    : 串口1发送一字节子程序
* Input          : dat -待发送数据
* Output         : None
* Return         : None
*******************************************************************************/

void UART1_SendByte( UINT8 dat )   
{        
    R8_UART1_THR  = dat;
    while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                         /* 等待数据发送 */       
}

/*******************************************************************************
* Function Name  : UART1_SendStr
* Description    : 串口1发送一字符串子程序
* Input          : str--待发送字符串数据
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
* Description    : 串口1接收一字节子程序
* Input          : None
* Output         : None
* Return         : Rcvdat -接收到的数据 
*******************************************************************************/

UINT8 UART1_RcvByte( void )    
{
    UINT8 Rcvdat = 0;
        
    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ) {
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* 等待数据准备好 */ 
        Rcvdat = R8_UART1_RBR;                                                  /* 从接收缓冲寄存器读出数据 */ 
    }
    else{
        R8_UART1_RBR;                                                           /* 有错误清除 */
    }
    return( Rcvdat );
}

/*******************************************************************************
* Function Name  : Seril1Send
* Description    : 禁用FIFO，串口1发送多字节程序 
* Input          : *Data -待发送数据区指针
*                  Num   -发送数据长度
* Output         : None
* Return         : None 
*******************************************************************************/

void  Seril1Send( UINT8 *Data, UINT8 Num )                        
{
    do{
        while( ( R8_UART1_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                    /* 等待数据发送完毕 */ 
        R8_UART1_THR  =*Data++;  
    }while( --Num );
}

/*******************************************************************************
* Function Name  : Seril1Rcv
* Description    : 禁用FIFO,串口1接收多字节子程序 
* Input          : *buf -接收数据暂存缓冲区
* Output         : None
* Return         : RcvNum -接收到数据长度 
*******************************************************************************/

UINT8  Seril1Rcv( UINT8 *buf )    
{
    UINT8 RcvNum = 0;

    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ) {
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* 等待数据准备好 */
        do{
            *buf++ = R8_UART1_RBR;                                              /* 从接收缓冲寄存器读出数据 */ 
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
* Description    : 启用FIFO,一次最多32字节，串口1发送多字节子程序
* Input          : *Data -待发送数据区指针
*                  Num   -发送数据长度
* Output         : None
* Return         : None
*******************************************************************************/

void UART1Send_FIFO( UINT8 *Data, UINT8 Num ) 
{
    int i;

    while( 1 ){
        while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                     /* 等待数据发送完毕，THR,TSR全空 */ 
        if( Num <= UART1_FIFO_SIZE){                                            /* FIFO长度为32，数据长度不满32字节，一次发送完成 */
            do{
                R8_UART1_THR = *Data++;
            }while(--Num) ;
            break;
        }
        else{                                                                   /* FIFO长度为32，数据长度超过32字节，分多次发送，一次32字节 */
            for(i=0;i<UART1_FIFO_SIZE;i++){
                R8_UART1_THR = *Data++;
            }
            Num -= UART1_FIFO_SIZE;
        }
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int main( void ) 
{
    UINT8 i,RcvNum;

    LED_OUT_INIT( );                                                                 
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    Uart1_Init( 115200 );                                                       /* 串口1初始化 */ 
    for( i = 0; i < 50; i++ ){
        buf[ i ] = i;
    }
    UART1_SendByte(0xAA);                                                       /* 串口1发送1字节 */
    UART1_SendByte(0xAA);                                                       /* 串口1发送1字节 */
    UART1_SendByte(0xAA);                                                       /* 串口1发送1字节 */
    UART1Send_FIFO( buf, 50 );                                                  /* 启用FIFO，发送50字节数据 */
    while(1){
        RcvNum  = Seril1Rcv( rcvbuf );                                          /* 等待接收数据，并通过串口1发送出去 */
        Seril1Send( rcvbuf, RcvNum );
    }
}

/*********************************** endfile **********************************/
