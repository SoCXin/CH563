/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0_FALSH.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SPI0_FALSH DEMO
*                      (1)��CH563 Examples by KEIL;    
*                      (2)������0��������Ϣ,115200bps;
*                      (3)����������Ҫ��ʹ��SPI0�ڷ�ʽ0 �¶�дFLASH.
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

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
* Function Name  : SPI_MASTER_INIT
* Description    : SPI����ģʽ��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI_MASTER_INIT ( void )
{
  R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE|RB_SPI_SCK_OE;                              /* MOSI,SCK���ʹ�ܣ�����ģʽ����ʽ0 */
  R8_SPI0_CLOCK_DIV = 0x0a;                                                     /* 10��Ƶ��100/10=10M */
  R32_PB_DIR |= (MOSI | SCK0 | SCS);                                            /* MOSI(PB14),SCK0(PA13),SCS(PB12)Ϊ��� */
  R32_PB_PU  |=  SCS ; 
  R8_SPI0_CTRL_DMA = 0;
}

/*******************************************************************************
* Function Name  : SPI0_Trans
* Description    : ����һ�ֽ�����
* Input          : data -Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/

void SPI0_Trans( UINT8 data )
{
    R32_SPI0_FIFO = data;
    R16_SPI0_TOTAL_CNT = 0x01;
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* �ȴ����ݷ������ */
}

/*******************************************************************************
* Function Name  : SPI0_Trans
* Description    : ����һ�ֽ�����
* Input          : None
* Output         : None
* Return         : data -���յ�������
*******************************************************************************/

UINT8 SPI0_Recv( void )
{
    UINT8 data;
    R32_SPI0_FIFO = 0xff;
    R16_SPI0_TOTAL_CNT = 0x01;
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* �ȴ����ݻ��� */
    data = R8_SPI0_BUFFER;
    return data;
}

/*******************************************************************************
* Function Name  : Read_Status_Register
* Description    : ������ȡ״̬�Ĵ���,������״̬�Ĵ�����ֵ
* Input          : None
* Output         : None
* Return         : byte -�Ĵ���״ֵ̬
*******************************************************************************/

UINT8 Read_Status_Register( void )   
{   
    UINT8 byte = 0;
    R32_PB_CLR |=  SCS ;                                                        /* ʹ���豸 */   
    SPI0_Trans(0x05);                                                           /* ���Ͷ�״̬�Ĵ��������� */   
    byte = SPI0_Recv( );                                                        /* ��ȡ״̬�Ĵ��� */  
    R32_PB_OUT |=  SCS ;                                                        /* ��ֹ�豸 */   
    return byte;   
}
   
/*******************************************************************************
* Function Name  : Wait_Busy
* Description    : �ȴ�оƬ����(��ִ��Byte-Program, Sector-Erase, Block-Erase, Chip-Erase������)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Wait_Busy( void )   
{   
    while ((Read_Status_Register())&0x01 == 0x01 )                              /* waste time until not busy */   
          Read_Status_Register( );   
}

/*******************************************************************************
* Function Name  : WRSR
* Description    : ��״̬�Ĵ�����дһ���ֽ�
* Input          : byte -д�������
* Output         : None
* Return         : None
*******************************************************************************/

void WRSR( UINT8 byte )   
{   
    R32_PB_CLR |=  SCS ;                                                        /* ʹ���豸 */  
    SPI0_Trans(0x01);                                                           /* ����д״̬�Ĵ��� */   
    SPI0_Trans(byte);                                                           /* �ı�Ĵ�����BPx����BPL (ֻ��2,3,4,5,7λ���Ը�д) */   
    R32_PB_OUT |=  SCS ;                                                        /* ��ֹ�豸 */   
}
 
/*******************************************************************************
* Function Name  : WREN
* Description    : дʹ��,ͬ����������ʹ��д״̬�Ĵ��� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void WREN( void )   
{   
   R32_PB_CLR |=  SCS ;         
   SPI0_Trans(0x06);                                                            /* ����WREN���� */  
   R32_PB_OUT |=  SCS ;             
}

/*******************************************************************************
* Function Name  : WREN_Check
* Description    : ����д����ǰWELλ�Ƿ�Ϊ1 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void WREN_Check( void )   
{   
    UINT8 byte;   
    byte = Read_Status_Register( );                                             /* ��ȡ״̬register */  
    if ((byte&0x02) != 0x02)                                                    /* ���WELλ��λ */  
    {   
        WREN( );                                                                /* ���δ��1������Ӧ����,��������дʹ�ò��� */
    }   
}
 
/*******************************************************************************
* Function Name  : Read
* Description    : ��ȡһ����ַ��һ���ֽڵ�����.���ض�ȡ������ 
* Input          : Dst -Destination Address 000000H - 1FFFFFH
* Output         : None
* Return         : byte -��ȡ������
*******************************************************************************/

UINT8 Read(UINT32 Dst)    
{   
    UINT8 byte = 0;    
    R32_PB_CLR |=  SCS ;                                                        /* enable device */   
    SPI0_Trans(0x03);                                                           /* read command */
    SPI0_Trans(((Dst & 0xFFFFFF) >> 16));                                       /* send 3 address bytes */ 
    SPI0_Trans(((Dst & 0xFFFF) >> 8));
    SPI0_Trans(Dst & 0xFF);
    byte = SPI0_Recv();   
    R32_PB_OUT |=  SCS ;                                                        /* disable device */   
    return byte;                                                                /* return one byte read */
} 

/*******************************************************************************
* Function Name  : Byte_Program
* Description    : д����
* Input          : Dst  -Destination Address 000000H - 1FFFFFH
*                  byte -Ҫд�������
* Output         : None
* Return         : None
*******************************************************************************/

void Byte_Program(UINT32 Dst, UINT8 byte)
{
    WREN();
    R32_PB_CLR |=  SCS ;                                                        /* оƬʹ�� */
    SPI0_Trans(0x02);                                                           /* ����д����ָ�� */
    SPI0_Trans(((Dst & 0xFFFFFF) >> 16));                                       /* ����3�ֽڵ�ַ */
    SPI0_Trans(((Dst & 0xFFFF) >> 8));
    SPI0_Trans(Dst & 0xFF);
    SPI0_Trans(byte);                                                           /* ����Ҫд������ */
    R32_PB_OUT |=  SCS ;
    Wait_Busy();
}
 
/*******************************************************************************
* Function Name  : Chip_Erase
* Description    : ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Chip_Erase( void )   
{                          
    WREN_Check();   
    R32_PB_CLR |=  SCS ;            
    SPI0_Trans(0x60);                                                           /* ���� Chip Erase���� (60h or C7h) */    
    R32_PB_OUT |=  SCS ;  
    Wait_Busy();   
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

    UINT8 buf[1024];
    UINT32 i;

    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* ������LED��һ����ʾ���� */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* ����1��ʼ�� */
    PRINT("START SPI FLASH\n");
    SPI_MASTER_INIT ( );                                                        /* SPI0����ģʽ��ʼ�� */
    WREN( );                                                                    /* FLASHдʹ�� */
    WRSR(0x00);                                                                 /* д�Ĵ��� */
    Chip_Erase( );                                                              /* FLASH��Ƭ���� */
    PRINT("Chip_Erase over\n");
    for(i=0;i<1024;i++){
        Byte_Program(i,i%256);                                                  /* ��FLASH��ַ0x00000001д������ */
    }
    PRINT("Write over\n");
    for(i=0;i<1024;i++){
        if(i%16 == 0) PRINT("\n");
        buf[i] = Read(i);                                                       /* ��FLASH��ַ0x00000001�������� */
        PRINT("%-8x", buf[i]);
    }
    while(1);
}

/*********************************** endfile **********************************/