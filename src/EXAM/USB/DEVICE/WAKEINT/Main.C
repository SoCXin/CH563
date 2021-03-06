/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB WAKE_INT DEMO
*                      (1)、CH563 USB Device Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、该程序以中断方式处理,PB3输入一个信号(低高电平信号)即可产生唤醒信号;
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "CH563USBSFR.H"                                                        /* 芯片USB寄存器相关头文件 */
#include "USB_DEVICE.H"                                                         /* USB设备定义相关头文件 */
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */
/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED                     1<<4

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }          /* LED 高电平为输出方向 */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                             /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                             /* LED 高电平关闭LED显示 */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if( !(R32_PB_PIN & (1<<3)) ) {
        Delay_ms( 100 );
        if( !(R32_PB_PIN & (1<<3)) ) {
            R32_PA_DIR |= 1<<8;
            R32_PA_OUT &= ~( 1<<8 );
            while( !(R32_PB_PIN & (1<<3)) );
            R32_PA_OUT |= ( 1<<8 );
#if  MY_DEBUG_PRINTF    
            printf("GPIO PB input = %8x\n",(UINT32)R32_PB_PIN&(1<<3));
#endif
            R8_SAFE_ACCESS_SIG = 0x57;                                          /* unlock step 1 */ 
            R8_SAFE_ACCESS_SIG = 0xA8;                                          /* unlock step 2 */ 
//            R8_SLP_WAKE_CTRL |= RB_SLP_USB_WAKE;                              /* USB唤醒使能 */ 
            R8_SLP_WAKE_CTRL |= RB_SLP_AP_WAK_USB;                              /* 应用程序唤醒USB请求 */ 
            R8_SAFE_ACCESS_SIG = 0;
            Delay_ms( 20 );

            R8_SAFE_ACCESS_SIG = 0x57;        
            R8_SAFE_ACCESS_SIG = 0xA8;         
//            R8_SLP_WAKE_CTRL &= ~RB_SLP_USB_WAKE;                             /* 唤醒禁止 */
            R8_SLP_WAKE_CTRL &= ~RB_SLP_AP_WAK_USB;                             /* 关闭唤醒信号 */ 
            R8_SAFE_ACCESS_SIG = 0;    
        }
    }
    if( R8_INT_FLAG_1 & RB_IF_USB ){                                            /* USB中断 */
        if( USB_GL_INT_STATUS & RB_HC_INT ){
//            USBHOST_EHCI_IRQHandler( );                                       /* USB主机中断处理函数 */
        }
        else if( USB_GL_INT_STATUS & RB_DEV_INT ){
            USBDev_IRQHandler( );                                               /* USB设备中断处理函数 */
        }
    }
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断处理函数
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
* Description    : 为printf和getkey输入输出初始化串口
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
    if ( x2 >= 5 ) x ++;                                                        /* 四舍五入 */
    R8_UART0_LCR = 0x80;                                                        /* DLAB位置1 */
    R8_UART0_DIV = 1;                                                           /* 预分频 */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为14，清发送和接收FIFO，FIFO使能 */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : 通过串口输出监控信息
* Input          : c-- writes the character specified by c 
*                  *f--the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int main( void ) 
{
    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    R32_PB_DIR &= ~(1<<3);
    R32_PB_PU  |= (1<<3);
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */

#if  MY_DEBUG_PRINTF    
    printf( "CH563 Test\n" );
#endif
    /************************** 设备模式测试 **************************/       
    USBDev_ModeSet( );                                                          /* USB设置为设备模式 */
    USBDev_Init( );                                                             /* USB设备初始化 */
    USBDev_UsbInt_Enable( );                                                    /* USB设备各相关中断使能 */
    while( 1 );
}

/*********************************** endfile **********************************/
